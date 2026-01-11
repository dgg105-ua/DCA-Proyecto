extern "C" {
  #include <raylib.h>
}

#include <cstdint>

static int g_screen_w = 800;
static int g_screen_h = 600;
static uint32_t g_seed = 1;

static bool g_keys[512] = {false};

extern "C" void Test_SetScreenSize(int w, int h)
{
  if (w > 0) g_screen_w = w;
  if (h > 0) g_screen_h = h;
}

extern "C" void Test_SetRandomSeed(unsigned int seed)
{
  g_seed = (seed == 0) ? 1u : seed;
}

extern "C" void Test_SetKeyDown(int key, bool down)
{
  if (key >= 0 && key < (int)(sizeof(g_keys)/sizeof(g_keys[0]))) g_keys[key] = down;
}

static uint32_t lcg()
{
  g_seed = 1664525u * g_seed + 1013904223u;
  return g_seed;
}

extern "C" int GetScreenWidth(void)  { return g_screen_w; }
extern "C" int GetScreenHeight(void) { return g_screen_h; }

extern "C" int GetRandomValue(int min, int max)
{
  if (max < min) { int t = min; min = max; max = t; }
  const uint32_t span = (uint32_t)(max - min + 1);
  return min + (int)(lcg() % span);
}

extern "C" bool IsKeyDown(int key)
{
  if (key >= 0 && key < (int)(sizeof(g_keys)/sizeof(g_keys[0]))) return g_keys[key];
  return false;
}

extern "C" bool CheckCollisionRecs(Rectangle a, Rectangle b)
{
  return (a.x < b.x + b.width) &&
         (a.x + a.width > b.x) &&
         (a.y < b.y + b.height) &&
         (a.y + a.height > b.y);
}
