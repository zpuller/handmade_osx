#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <chrono>
#include <assert.h>
#include <cstdlib>
#include <unistd.h>
#include <math.h>
#include <sys/stat.h>

#include <mach/mach_init.h>
#include <mach/vm_map.h>

#include <handmade/handmade.h>

#define Kilobytes(x) 1024 * x
#define Megabytes(x) 1024 * Kilobytes(x)
#define Gigabytes(x) 1024 * Megabytes(x)

static const int bufWidth = 1366;
static const int bufHeight = 768;
static const int bytesPerPixel = 4;

static const int pageSize = Kilobytes(4);

static const float PI = 3.14159265;

unsigned long long CycleCount() 
{
  unsigned long long d;
  __asm__ __volatile__ ("rdtsc" : "=A" (d) );
  return d;
}

void* Allocate(size_t size)
{
  void* data;
  kern_return_t err;

  assert(size != 0);
  assert((size % pageSize) == 0);

  err = vm_allocate(  (vm_map_t) mach_task_self(),
                      (vm_address_t*) &data,
                      size,
                      VM_FLAGS_ANYWHERE);

  assert(err == KERN_SUCCESS);
  if(err != KERN_SUCCESS)
      data = NULL;

  return data;
}

void* PlaySound(sf::Sound& sound, sf::SoundBuffer& buffer)
{
  int samplesPerSecond = 44100;
  int freq = 261;
  int samplesPerVibration = samplesPerSecond / freq;
  int numVibrations = 1;
  int numChannels = 1;
  int numSamples = samplesPerVibration * numVibrations * numChannels;
  int bufSizeBytes = numSamples * sizeof(short);
  void* sampleBuf = malloc(bufSizeBytes);
  short* sample = (short*)sampleBuf;
  short volume = 5000;
  for (int i = 0; i < numVibrations; ++i)
  {
    float theta;
    for (int j = 0; j < (samplesPerVibration / 2); ++j)
    {
      theta = PI * (float)j / ((float)samplesPerVibration / 2.);
      *sample = volume * sin(theta);
      ++sample;
    }
    for (int k = 0; k < (samplesPerVibration / 2); ++k)
    {
      theta = PI * (float)k / ((float)samplesPerVibration / 2.);
      *sample = -volume * sin(theta);
      ++sample;
    }
  }

  buffer.loadFromSamples((const short*)sampleBuf, numSamples, numChannels, 44100);
  sound.setBuffer(buffer);
  sound.setLoop(true);

  sound.play();

  return sampleBuf;
}

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define LIBSDIR STR(LIBS_DIR)
int main(int argc, char const** argv)
{
  sf::Sound sound;
  sf::SoundBuffer buffer;
  //void* sampleBuf = PlaySound(sound, buffer);

  int bufMemSize = bufHeight * bufWidth * bytesPerPixel;
  void* bufMem = Allocate((1 + (bufMemSize / pageSize)) * pageSize);
  GameOffscreenBuffer offscreenBuffer;
  offscreenBuffer.width = bufWidth; 
  offscreenBuffer.height = bufHeight;
  offscreenBuffer.pixels = bufMem;

  GameInput input = {};

  GameMemory* gameMem = (GameMemory*)Allocate(Kilobytes(4));
  Initialize(*gameMem);

#ifdef DEBUG
  sf::Font MyFont;
  if (!MyFont.loadFromFile("/Library/Fonts//Arial.ttf"))
  {
    printf("font not found!\n");
    exit(1);
  }

  sf::Text text("", MyFont, 20);
#endif

  auto last = std::chrono::steady_clock::now();
  auto now = std::chrono::steady_clock::now();
  auto diff = now - last;
  int msElapsed;
  int fps;
  int fpsTarget = 60;
  int msTarget = 1000 / fpsTarget;

  int windowWidth = 1366;
  int windowHeight= 768;
  sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "handmade", sf::Style::Fullscreen);
  window.setKeyRepeatEnabled(false);
  while (window.isOpen())
  {
    GameUpdateAndRender(*gameMem, offscreenBuffer, input);

    sf::Image image;
    image.create(bufWidth,bufHeight,(unsigned char*)bufMem);
    sf::Texture texture;
    texture.create(bufWidth,bufHeight);
    texture.update(image);
    sf::Sprite sprite(texture);

    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      if (event.type == sf::Event::KeyPressed)
      {
        if (event.key.code == sf::Keyboard::W)
          input.wKey = true;
        if (event.key.code == sf::Keyboard::A)
          input.aKey = true;
        if (event.key.code == sf::Keyboard::S)
          input.sKey = true;
        if (event.key.code == sf::Keyboard::D)
          input.dKey = true;
      }

      if (event.type == sf::Event::KeyReleased)
      {
        if (event.key.code == sf::Keyboard::W)
          input.wKey = false;
        if (event.key.code == sf::Keyboard::A)
          input.aKey = false;
        if (event.key.code == sf::Keyboard::S)
          input.sKey = false;
        if (event.key.code == sf::Keyboard::D)
          input.dKey = false;
      }

      if (event.type == sf::Event::MouseButtonPressed)
      {
        input.lMouse = true;
      }

      if (event.type == sf::Event::MouseButtonReleased)
      {
        input.lMouse = false;
      }

      if (event.type == sf::Event::MouseMoved)
      {
        input.mouseX = event.mouseMove.x;
        input.mouseY = event.mouseMove.y;
      }
    }

    window.clear();
    window.draw(sprite);

    now = std::chrono::steady_clock::now();
    diff = now - last;
    msElapsed = std::chrono::duration <double, std::milli> (diff).count();
    if (msElapsed < msTarget)
    {
      usleep(1000 * (msTarget - msElapsed - 1));
      now = std::chrono::steady_clock::now();
      diff = now - last;
      msElapsed = std::chrono::duration <double, std::milli> (diff).count();
    }
    else
    {
      //TODO missed rate!
    }
    last = now;
    fps = 1000/msElapsed;

#ifdef DEBUG
    char fpsStr[2];
    sprintf(fpsStr, "%i fps", fps); 
    text.setString(fpsStr);
    window.draw(text);
#endif

    window.display();
  }

  return 0;
}
