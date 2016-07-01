struct PermanentState
{
  unsigned char xOffset;
  unsigned char yOffset;
};

struct TransientState 
{
};

struct GameMemory
{
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
};

// hack
struct Fns
{
  void (*Initialize)(GameMemory&);
  void (*GameUpdateAndRender)(GameMemory&, GameOffscreenBuffer&, GameInput&);
};
