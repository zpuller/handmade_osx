#include "handmade.h"
#include <algorithm>

void DrawSquare(void* pixels, int frameWidth, int frameHeight, int x, int y, int width, int height, Vec3 color)
{
  using namespace std;

  int minWidth = max(x, 0);
  int minHeight = max(y, 0);
  int maxWidth = min(x + width, frameWidth);
  int maxHeight = min(y + height, frameHeight);

  unsigned char* pixel = (unsigned char*)pixels;
  pixel += (4 * (y * frameWidth));
  pixel += (4 * x);

  for (int i = y; i < maxHeight; ++i)
  {
    for (int j = x; j < maxWidth; ++j)
    {
      *pixel = color.r; 
      ++pixel;

      *pixel = color.g;
      ++pixel;

      *pixel = color.b;
      ++pixel;

      *pixel = 255;
      ++pixel;
    }

    pixel += (4 * (frameWidth - width));
  }  
}

void Initialize(GameMemory& memory)
{
  memory.permanentState = {}; 
  memory.isInitialized = true;
}

void GameUpdateAndRender(GameMemory& memory, GameOffscreenBuffer& offscreenBuffer, GameInput& input, float& x, float& y, bool& clicked)
{
  // clear screen
  DrawSquare(offscreenBuffer.pixels, offscreenBuffer.width, offscreenBuffer.height, 0, 0, offscreenBuffer.width, offscreenBuffer.height, black); 

  int scale = 50;
  memory.permanentState.xOffset = x * scale;
  memory.permanentState.yOffset = (1-y) * scale;

  int cubeX = .1 * offscreenBuffer.width + memory.permanentState.xOffset; 
  int cubeY = .8 * offscreenBuffer.height  + memory.permanentState.yOffset;
  int cubeHeight = .1 * offscreenBuffer.height + 1; 
  int cubeWidth = cubeHeight; 

  Vec3 color = green;
  clicked &= input.lMouse;
  if (clicked || (input.lMouse && input.mouseX > cubeX && input.mouseX < cubeX + cubeWidth && input.mouseY > cubeY && input.mouseY < cubeY + cubeHeight))
  {
    color = blue;
    clicked = true;
    x = (input.mouseX - (.13 * offscreenBuffer.width)) / scale;
    y = std::max(0.0, 1 - ((input.mouseY - (.85 * offscreenBuffer.height)) / scale));
  }
  else
  {
    clicked = false;
  }

  DrawSquare(offscreenBuffer.pixels, offscreenBuffer.width, offscreenBuffer.height, cubeX, cubeY, cubeWidth, cubeHeight, color);

  int floorX = 0; 
  int floorY = .9 * offscreenBuffer.height;
  int floorWidth = offscreenBuffer.width;
  int floorHeight = .1 * offscreenBuffer.height; 

  DrawSquare(offscreenBuffer.pixels, offscreenBuffer.width, offscreenBuffer.height, floorX, floorY, floorWidth, floorHeight, blue); 
}

Fns fns = { Initialize, GameUpdateAndRender };
