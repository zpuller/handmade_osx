#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <chrono>
#include <assert.h>
#include <cstdlib>
#include <math.h>

#include <mach/mach_init.h>
#include <mach/vm_map.h>

static void* bufMem;
static const int bufWidth = 1024;
static const int bufHeight = 768;
static const int bytesPerPixel = 4;
static const int bufMemSize = bufHeight * bufWidth * bytesPerPixel;
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
  assert((size % 4096) == 0);

  err = vm_allocate(  (vm_map_t) mach_task_self(),
                      (vm_address_t*) &data,
                      size,
                      VM_FLAGS_ANYWHERE);

  assert(err == KERN_SUCCESS);
  if(err != KERN_SUCCESS)
      data = NULL;

  return data;
}

void RenderGradient(unsigned char xOffset, unsigned char yOffset)
{
  unsigned char* pixel = (unsigned char*)bufMem;

  for (int i = 0; i < bufHeight; ++i)
  {
    for (int j = 0; j < bufWidth; ++j)
    {
      *pixel = 0; 
      ++pixel;

      *pixel = i + 3 * yOffset;
      ++pixel;

      *pixel = j + 3 * xOffset;
      ++pixel;

      *pixel = 255;
      ++pixel;
    }
  }  
}

void* PlaySound(sf::Sound& sound, sf::SoundBuffer& buffer)
{
  int samplesPerSecond = 44100;
  int freq = 261;
  int samplesPerVibration = samplesPerSecond / freq;
  int numVibrations = 10;
  int numChannels = 2;
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
      *sample = volume * sin(theta);
      ++sample;
    }
    for (int k = 0; k < (samplesPerVibration / 2); ++k)
    {
      theta = PI * (float)k / ((float)samplesPerVibration / 2.);
      *sample = -volume * sin(theta);
      ++sample;
      *sample = -volume * sin(theta);
      ++sample;
    }
  }

  buffer.loadFromSamples((const short*)sampleBuf, numSamples, 2, 44100);
  sound.setBuffer(buffer);
  sound.setLoop(true);
  sound.play();

  return sampleBuf;
}

int main(int, char const**)
{
  const int page_size = 4096;
  bufMem = Allocate((bufMemSize / page_size) * page_size);

  sf::Sound sound;
  sf::SoundBuffer buffer;
  void* sampleBuf = PlaySound(sound, buffer);

#ifdef DEBUG
  sf::Font MyFont;
  if (!MyFont.loadFromFile("/Library/Fonts//Arial.ttf"))
  {
    printf("font not found!\n");
    exit(1);
  }

  sf::Text text("", MyFont, 20);
  auto last = std::chrono::steady_clock::now();
  auto now = std::chrono::steady_clock::now();
  auto diff = now - last;
  int ms_elapsed;
  int fps;

  int mouseX;
  int mouseY;
#endif

  sf::RenderWindow window(sf::VideoMode(800, 600), "handmade");
  window.setKeyRepeatEnabled(false);
  unsigned char xOffset = 0;
  unsigned char yOffset = 0;
  bool wKey, aKey, sKey, dKey = false;
  while (window.isOpen())
  {
    if (wKey)
      ++yOffset;
    if (aKey)
      ++xOffset;
    if (sKey)
      --yOffset;
    if (dKey)
      --xOffset;
    RenderGradient(xOffset, yOffset);

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

      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        window.close();
      }

      if (event.type == sf::Event::KeyPressed)
      {
        if (event.key.code == sf::Keyboard::W)
          wKey = true;
        if (event.key.code == sf::Keyboard::A)
          aKey = true;
        if (event.key.code == sf::Keyboard::S)
          sKey = true;
        if (event.key.code == sf::Keyboard::D)
          dKey = true;
      }

      if (event.type == sf::Event::KeyReleased)
      {
        if (event.key.code == sf::Keyboard::W)
          wKey = false;
        if (event.key.code == sf::Keyboard::A)
          aKey = false;
        if (event.key.code == sf::Keyboard::S)
          sKey = false;
        if (event.key.code == sf::Keyboard::D)
          dKey = false;
      }

      if (event.type == sf::Event::MouseMoved)
      {
#ifdef DEBUG
        mouseX = event.mouseMove.x;
        mouseY = event.mouseMove.y;
#endif
      }
    }

    window.clear();
    window.draw(sprite);
#ifdef DEBUG
    now = std::chrono::steady_clock::now();
    diff = now - last;
    last = now;
    ms_elapsed = std::chrono::duration <double, std::milli> (diff).count();
    fps = 1000/ms_elapsed;
    char fpsStr[2];
    sprintf(fpsStr, "%i fps . mouseX: %i . mouseY: %i", fps, mouseX, mouseY);
    text.setString(fpsStr);
    window.draw(text);
#endif
    window.display();

  }

  return 0;
}
