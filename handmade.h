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

void GameUpdateAndRender(GameMemory& memory, GameOffscreenBuffer& offscreenBuffer, GameInput& input);
