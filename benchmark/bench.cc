#include <Box2D/Box2D.h>
#include <chrono>
#include <iostream>

void stack_test()
{
  b2Vec2 gravity(0.0f, -10.0f);
  b2World world(gravity);
  b2BodyDef groundBodyDef;
  groundBodyDef.position.Set(0.0f, -10.0f);
  b2Body* groundBody = world.CreateBody(&groundBodyDef);
  b2PolygonShape groundBox;
  groundBox.SetAsBox(50.0f, 10.0f);
  groundBody->CreateFixture(&groundBox, 0.0f);
  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position.Set(0.0f, 4.0f);
  b2PolygonShape dynamicBox;
  dynamicBox.SetAsBox(1.0f, 1.0f);
  b2FixtureDef fixtureDef;
  fixtureDef.shape = &dynamicBox;
  fixtureDef.density = 1.0f;
  fixtureDef.friction = 0.3f;
  for (int i = 0; i < 10; ++i)
  {
    b2Body* body = world.CreateBody(&bodyDef);
    body->CreateFixture(&fixtureDef);
  }

  auto start = std::chrono::steady_clock::now();

  for (int i = 0; i < 1000000; ++i)
  {
    float32 timeStep = 1.0f / 60.0f;
    int32 velocityIterations = 6;
    int32 positionIterations = 2;
    world.Step(timeStep, velocityIterations, positionIterations);
  }


  auto end = std::chrono::steady_clock::now();
  auto diff = end - start;
  std::cout << "stack: " << std::chrono::duration <double, std::milli> (diff).count() << "ms" << std::endl;
}

void heap_test()
{
  b2Vec2 gravity(0.0f, -10.0f);
  b2World* pworld = new b2World(gravity);
  b2World& world = *pworld; 
  b2BodyDef groundBodyDef;
  groundBodyDef.position.Set(0.0f, -10.0f);
  b2Body* groundBody = world.CreateBody(&groundBodyDef);
  b2PolygonShape groundBox;
  groundBox.SetAsBox(50.0f, 10.0f);
  groundBody->CreateFixture(&groundBox, 0.0f);
  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position.Set(0.0f, 4.0f);
  b2PolygonShape dynamicBox;
  dynamicBox.SetAsBox(1.0f, 1.0f);
  b2FixtureDef fixtureDef;
  fixtureDef.shape = &dynamicBox;
  fixtureDef.density = 1.0f;
  fixtureDef.friction = 0.3f;
  for (int i = 0; i < 10; ++i)
  {
    b2Body* body = world.CreateBody(&bodyDef);
    body->CreateFixture(&fixtureDef);
  }

  auto start = std::chrono::steady_clock::now();

  for (int i = 0; i < 1000000; ++i)
  {
    float32 timeStep = 1.0f / 60.0f;
    int32 velocityIterations = 6;
    int32 positionIterations = 2;
    world.Step(timeStep, velocityIterations, positionIterations);
  }


  auto end = std::chrono::steady_clock::now();
  auto diff = end - start;
  std::cout << "heap: " << std::chrono::duration <double, std::milli> (diff).count() << "ms" << std::endl;
}

int main()
{
  stack_test();
  heap_test();
}
