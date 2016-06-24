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

unsigned char xOffset = 0;
unsigned char yOffset = 0;
void RenderGradient()
{
  ++xOffset;
  yOffset = yOffset + 2;
  unsigned char* pixel = (unsigned char*)bufMem;

  for (int i = 0; i < bufHeight; ++i)
  {
    for (int j = 0; j < bufWidth; ++j)
    {
      *pixel = 0; 
      ++pixel;

      *pixel = i + xOffset;
      ++pixel;

      *pixel = j + yOffset;
      ++pixel;

      *pixel = 255;
      ++pixel;
    }
  }  
}

void PlaySound(sf::Sound& sound, sf::SoundBuffer& buffer)
{
  int samplesPerSecond = 44100;
  int freq = 261;
  int samplesPerVibration = samplesPerSecond / freq;
  int numVibrations = 10;
  int numChannels = 2;
  int bufSizeBytes = samplesPerVibration * numVibrations * numChannels * sizeof(short); 
  void* buf = malloc(bufSizeBytes);
  short* sample = (short*)buf; 
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

  buffer.loadFromSamples((const short*)buf, bufSizeBytes / 2, 2, 44100);
  sound.setBuffer(buffer);
  sound.setLoop(true);
  sound.play();
}

int main(int, char const**)
{
  const int page_size = 4096;
  bufMem = Allocate((bufMemSize / page_size) * page_size);

  sf::Sound sound;
  sf::SoundBuffer buffer;
  PlaySound(sound, buffer);

  sf::RenderWindow window(sf::VideoMode(800, 600), "handmade");
  while (window.isOpen())
  {
    auto start = std::chrono::steady_clock::now();

    RenderGradient();

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
    }

    window.clear();
    window.draw(sprite);
    window.display();

    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    int ms_elapsed = std::chrono::duration <double, std::milli> (diff).count();
    int fps = 1000/ms_elapsed;
#ifdef DEBUG
    printf("fps: %i\n", fps);
#endif
  }

  return 0;
}
