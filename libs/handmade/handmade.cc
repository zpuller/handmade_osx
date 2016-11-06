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
  pixel += (4 * (minHeight * frameWidth));
  pixel += (4 * minWidth);

  for (int i = minHeight; i < maxHeight; ++i)
  {
    for (int j = minWidth; j < maxWidth; ++j)
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

    pixel += (4 * (frameWidth - maxWidth + minWidth));
  }  
}

void Initialize(GameMemory& memory)
{
  b2Vec2 gravity(0.0f, -10.0f);
  memory.world.reset(new b2World(gravity));
  b2BodyDef groundBodyDef;
  groundBodyDef.position.Set(0.0f, -10.0f);
  b2Body* groundBody = memory.world->CreateBody(&groundBodyDef);
  b2PolygonShape groundBox;
  groundBox.SetAsBox(50.0f, 10.0f);
  groundBody->CreateFixture(&groundBox, 0.0f);
  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position.Set(0.0f, 4.0f);
  memory.body = memory.world->CreateBody(&bodyDef);
  b2PolygonShape dynamicBox;
  dynamicBox.SetAsBox(1.0f, 1.0f);
  b2FixtureDef fixtureDef;
  fixtureDef.shape = &dynamicBox;
  fixtureDef.density = 1.0f;
  fixtureDef.friction = 0.3f;
  memory.body->CreateFixture(&fixtureDef);

  memory.permanentState = {}; 
  memory.isInitialized = true;
}

void GameUpdateAndRender(GameMemory& memory, GameOffscreenBuffer& offscreenBuffer, GameInput& input)
{
  float32 timeStep = 1.0f / 60.0f;
  int32 velocityIterations = 6;
  int32 positionIterations = 2;
  memory.world->Step(timeStep, velocityIterations, positionIterations);
  b2Vec2 position = memory.body->GetPosition();

  float posx = position.x;
  float posy = position.y;
  float ox = posx;
  float oy = posy;

  // clear screen
  DrawSquare(offscreenBuffer.pixels, offscreenBuffer.width, offscreenBuffer.height, 0, 0, offscreenBuffer.width, offscreenBuffer.height, black); 

  int scale = 50;
  memory.permanentState.xOffset = posx * scale;
  memory.permanentState.yOffset = (1-posy) * scale;

  int cubeX = .1 * offscreenBuffer.width + memory.permanentState.xOffset; 
  int cubeY = .8 * offscreenBuffer.height  + memory.permanentState.yOffset;
  int cubeHeight = .1 * offscreenBuffer.height + 1; 
  int cubeWidth = cubeHeight; 

  Vec3 color = green;
  bool& clicked = memory.permanentState.clicked;
  clicked &= input.lMouse;
  if (clicked || (input.lMouse && input.mouseX > cubeX && input.mouseX < cubeX + cubeWidth && input.mouseY > cubeY && input.mouseY < cubeY + cubeHeight))
  {
    color = blue;
    clicked = true;
    posx = (input.mouseX - (.13 * offscreenBuffer.width)) / scale;
    posy = std::max(0.0, 1 - ((input.mouseY - (.85 * offscreenBuffer.height)) / scale));

    memory.body->SetAwake(true);
    memory.body->SetTransform(b2Vec2(posx, posy), 0.0f);
    memory.body->SetLinearVelocity(20*b2Vec2(posx-ox, posy-oy));
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
