#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <chrono>
#include <assert.h>
#include <cstdlib>
#include <unistd.h>
#include <math.h>

#include <mach/mach_init.h>
#include <mach/vm_map.h>

#include "handmade.h"

static const int bufWidth = 1024;
static const int bufHeight = 768;
static const int bytesPerPixel = 4;

static const int pageSize = 4096;

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
  sf::Sound sound;
  sf::SoundBuffer buffer;
  void* sampleBuf = PlaySound(sound, buffer);

  int bufMemSize = bufHeight * bufWidth * bytesPerPixel;
  void* bufMem = Allocate((bufMemSize / pageSize) * pageSize);
  GameOffscreenBuffer offscreenBuffer;
  offscreenBuffer.width = bufWidth; 
  offscreenBuffer.height = bufHeight;
  offscreenBuffer.pixels = bufMem;

  GameInput input = {};

  GameMemory* gameMem = (GameMemory*)Allocate(4096);//TODO mem size macros

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
  int fpsTarget = 30;
  int msTarget = 1000 / fpsTarget;

  int mouseX;
  int mouseY;

  sf::RenderWindow window(sf::VideoMode(800, 600), "handmade");
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

      if (event.type == sf::Event::MouseMoved)
      {
        mouseX = event.mouseMove.x;
        mouseY = event.mouseMove.y;
      }
    }

    window.clear();
    window.draw(sprite);

    now = std::chrono::steady_clock::now();
    diff = now - last;
    msElapsed = std::chrono::duration <double, std::milli> (diff).count();
    if (msElapsed < msTarget)
    {
      usleep(1000 * (msTarget - msElapsed));
      now = std::chrono::steady_clock::now();
      diff = now - last;
      msElapsed = std::chrono::duration <double, std::milli> (diff).count();
      printf("ms: %i\n", msElapsed);
    }
    else
    {
      //TODO missed rate!
      printf("missed, ms: %i\n", msElapsed);
    }
    fps = 1000/msElapsed;
    last = now;

#ifdef DEBUG
    char fpsStr[2];
    sprintf(fpsStr, "%i fps . mouseX: %i . mouseY: %i", fps, mouseX, mouseY);
    text.setString(fpsStr);
    window.draw(text);
#endif

    window.display();
  }

  return 0;
}
