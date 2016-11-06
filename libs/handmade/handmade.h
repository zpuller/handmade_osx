#include <Box2D/Box2D.h>

#include <memory>

struct Vec3
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
};

static const Vec3 red = { 255, 0, 0 };
static const Vec3 green = { 0, 255, 0 };
static const Vec3 blue = { 0, 0, 255 };
static const Vec3 black = { 0, 0, 0 };

struct PermanentState
{
  int xOffset;
  int yOffset;
  bool clicked;
};

struct TransientState 
{
};

struct GameMemory
{
  std::unique_ptr<b2World> world;
  b2Body* body;
  bool isInitialized;
  PermanentState permanentState;
  TransientState transientState;
};

struct GameOffscreenBuffer
{
  void* pixels;
  int width;
  int height;
};

struct GameInput
{
  bool wKey;
  bool aKey;
  bool sKey;
  bool dKey;
  
  bool lMouse;
  bool rMouse;

  int mouseX;
  int mouseY;
};

void Initialize(GameMemory&);
void GameUpdateAndRender(GameMemory&, GameOffscreenBuffer&, GameInput&);
