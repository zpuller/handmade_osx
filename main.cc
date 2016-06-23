#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <chrono>
#include <assert.h>

#include <mach/mach_init.h>
#include <mach/vm_map.h>

static void* bufMem;
static const int bufWidth = 1024;
static const int bufHeight = 768;
static const int bytesPerPixel = 4;
static const int bufMemSize = bufHeight * bufWidth * bytesPerPixel;

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

int main(int, char const**)
{
  const int page_size = 4096;
  bufMem = Allocate((bufMemSize / page_size) * page_size);
  sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");

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
    printf("%i\n", fps);
  }

  return 0;
}
