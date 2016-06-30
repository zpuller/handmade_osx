#include "handmade.h"

void RenderGradient(void* pixels, int width, int height, unsigned char xOffset, unsigned char yOffset)
{
  unsigned char* pixel = (unsigned char*)pixels;

  for (int i = 0; i < height; ++i)
  {
    for (int j = 0; j < width; ++j)
    {
      *pixel = 0; 
      ++pixel;

      *pixel = i + 5 * yOffset;
      ++pixel;

      *pixel = j + 5 * xOffset;
      ++pixel;

      *pixel = 255;
      ++pixel;
    }
  }  
}

void Initialize(GameMemory& memory)
{
  PermanentState zeroState = {};
  memory.permanentState = zeroState;
  memory.isInitialized = true;
}

void GameUpdateAndRender(GameMemory& memory, GameOffscreenBuffer& offscreenBuffer, GameInput& input)
{
  if (input.wKey)
    ++memory.permanentState.yOffset;
  if (input.aKey)
    ++memory.permanentState.xOffset;
  if (input.sKey)
    --memory.permanentState.yOffset;
  if (input.dKey)
    --memory.permanentState.xOffset;

  RenderGradient(offscreenBuffer.pixels, offscreenBuffer.width, offscreenBuffer.height, memory.permanentState.xOffset, memory.permanentState.yOffset);
}

Fns fns = { Initialize, GameUpdateAndRender };
