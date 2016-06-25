#include "handmade.h"

void RenderGradient(void* buffer, int width, int height, unsigned char xOffset, unsigned char yOffset)
{
  unsigned char* pixel = (unsigned char*)buffer;

  for (int i = 0; i < height; ++i)
  {
    for (int j = 0; j < width; ++j)
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
