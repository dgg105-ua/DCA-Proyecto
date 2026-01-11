#include <iostream>
#include <string>
#include <deque>
#include <cmath>

#include <MainGameState.hpp>
#include <GameLogic.hpp>

extern "C" {
  void Test_SetRandomSeed(unsigned int seed);
  void Test_SetScreenSize(int w, int h);
  void Test_SetKeyDown(int key, bool down);
}

static bool feq(float a, float b, float eps = 1e-3f) { return std::fabs(a - b) <= eps; }

#define CHECK(cond) do { \
  if (!(cond)) { \
    std::cerr << "Fallo: " << #cond << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
    return false; \
  } \
} while(0)

static Player make_player(float cx, float cy, float w, float h)
{
  Player p{};
  p.x = cx; p.y = cy;
  p.vx = 0; p.vy = 0;
  p.width = w; p.height = h;
  p.canJump = true;
  p.jumpBufferTime = 0.0f;
  p.coyoteTime = 0.0f;
  p.boundingBox = { cx - w*0.5f, cy - h*0.5f, w, h };
  return p;
}

static void sync_bb(Player& p)
{
  p.boundingBox.x = p.x - p.width*0.5f;
  p.boundingBox.y = p.y - p.height*0.5f;
  p.boundingBox.width  = p.width;
  p.boundingBox.height = p.height;
}


static bool t_collision_ground()
{
  Player p = make_player(50, 100, 10, 20);
  sync_bb(p);

  Estructura plat{};
  plat.rect = {0, 100, 200, 20};
  std::deque<Estructura> es{plat};

  p.vy = 123;
  bool enSuelo = gestionarColisiones(es, p);

  CHECK(enSuelo == true);
  CHECK(feq(p.vy, 0.0f));
  CHECK(feq(p.y, 90.0f));
  return true;
}

static bool t_collision_wall_push()
{
  Player p = make_player(100, 60, 10, 20);
  sync_bb(p);

  Estructura wall{};
  wall.rect = {100, 0, 10, 200};
  std::deque<Estructura> es{wall};

  bool enSuelo = gestionarColisiones(es, p);
  CHECK(enSuelo == false);
  CHECK(p.x <= 100.0f);
  return true;
}

static bool t_gravity_moves_down()
{
  Player p = make_player(0, 0, 50, 80);
  bool enSuelo = false;

  float y0 = p.y;
  gestionarSalto(p, 0.1f, enSuelo);

  CHECK(p.vy > 0.0f);
  CHECK(p.y > y0);
  return true;
}

static bool t_jump_buffer_triggers()
{
  Player p = make_player(0, 0, 50, 80);
  bool enSuelo = true;

  p.jumpBufferTime = 0.05f;
  p.coyoteTime = 0.0f;
  p.vy = 0.0f;

  gestionarSalto(p, 0.016f, enSuelo);

  CHECK(feq(p.vy, -500.0f));
  CHECK(p.canJump == false);
  CHECK(feq(p.jumpBufferTime, 0.0f));
  CHECK(feq(p.coyoteTime, 0.0f));
  return true;
}

static bool t_jump_cut_when_space_held()
{
  Test_SetKeyDown(KEY_SPACE, true);

  Player p = make_player(0, 0, 50, 80);
  bool enSuelo = false;

  p.vy = -300.0f;
  float vy0 = p.vy;

  gestionarSalto(p, 0.1f, enSuelo);

  CHECK(p.vy > vy0);
  CHECK(p.vy < (vy0 + 150.0f));
  Test_SetKeyDown(KEY_SPACE, false);
  return true;
}

static bool t_powerup_spawn_by_timer()
{
  Test_SetScreenSize(800, 600);
  Test_SetRandomSeed(1);

  Player p = make_player(400, 1000, 50, 80);

  PowerUp pu{};
  pu.active = false;
  pu.radius = 20.0f;

  float timer = 7.99f;
  float interval = 8.0f;

  gestionarPowerUp(pu, p, 0.02f, timer, interval);

  CHECK(pu.active == true);
  CHECK(feq(timer, 0.0f));
  CHECK(interval >= 5.0f && interval <= 8.0f);
  CHECK(pu.x >= 100.0f && pu.x <= 700.0f);
  return true;
}

static bool t_powerup_pickup_jump()
{
  Test_SetScreenSize(800, 600);

  Player p = make_player(200, 300, 50, 80);

  PowerUp pu{};
  pu.active = true;
  pu.radius = 10.0f;
  pu.x = p.x;
  pu.y = p.y;

  float timer = 3.0f;
  float interval = 8.0f;

  gestionarPowerUp(pu, p, 0.016f, timer, interval);

  CHECK(pu.active == false);
  CHECK(feq(timer, 0.0f));
  CHECK(feq(p.vy, -1500.0f));
  return true;
}

static bool t_shield_pickup_sets_flag()
{
  Player p = make_player(200, 300, 50, 80);

  PowerUp shield{};
  shield.active = true;
  shield.radius = 10.0f;
  shield.x = p.x;
  shield.y = p.y;

  float timer = 0.0f;
  float interval = 10.0f;
  bool shieldActive = false;

  gestionarShieldPU(shield, p, 0.016f, timer, interval, shieldActive);

  CHECK(shield.active == false);
  CHECK(shieldActive == true);
  CHECK(feq(p.vy, -600.0f));
  return true;
}

static bool t_slow_pickup_sets_timescale()
{
  Player p = make_player(200, 300, 50, 80);

  PowerUp slow{};
  slow.active = true;
  slow.radius = 10.0f;
  slow.x = p.x;
  slow.y = p.y;

  float timer = 0.0f;
  float interval = 12.0f;
  bool slowActive = false;
  float slowLeft = 0.0f;
  float timeScale = 1.0f;
  float slowDuration = 5.0f;

  gestionarSlowPU(slow, p, 0.016f, timer, interval, slowActive, slowLeft, slowDuration, timeScale);

  CHECK(slow.active == false);
  CHECK(slowActive == true);
  CHECK(feq(slowLeft, slowDuration));
  CHECK(feq(timeScale, 0.4f));
  return true;
}

static bool t_double_pickup_sets_multiplier()
{
  Player p = make_player(200, 300, 50, 80);

  PowerUp dbl{};
  dbl.active = true;
  dbl.radius = 10.0f;
  dbl.x = p.x;
  dbl.y = p.y;

  float timer = 0.0f;
  float interval = 14.0f;
  bool doubleActive = false;
  float doubleLeft = 0.0f;
  float scoreMult = 1.0f;
  float doubleDuration = 6.0f;

  gestionarDoublePU(dbl, p, 0.016f, timer, interval, doubleActive, doubleLeft, doubleDuration, scoreMult);

  CHECK(dbl.active == false);
  CHECK(doubleActive == true);
  CHECK(feq(doubleLeft, doubleDuration));
  CHECK(feq(scoreMult, 2.0f));
  return true;
}

#undef CHECK

struct Test { const char* name; bool (*fn)(); };

static const Test tests[] = {
  {"collision_ground", t_collision_ground},
  {"collision_wall_push", t_collision_wall_push},
  {"gravity_moves_down", t_gravity_moves_down},
  {"jump_buffer_triggers", t_jump_buffer_triggers},
  {"jump_cut_when_space_held", t_jump_cut_when_space_held},
  {"powerup_spawn_by_timer", t_powerup_spawn_by_timer},
  {"powerup_pickup_jump", t_powerup_pickup_jump},
  {"shield_pickup_sets_flag", t_shield_pickup_sets_flag},
  {"slow_pickup_sets_timescale", t_slow_pickup_sets_timescale},
  {"double_pickup_sets_multiplier", t_double_pickup_sets_multiplier},
};

static const Test* find_test(const std::string& name)
{
  for (auto& t : tests) if (name == t.name) return &t;
  return nullptr;
}

int main(int argc, char** argv)
{
  if (argc < 3 || std::string(argv[1]) != "--run") {
    std::cerr << "Uso: " << argv[0] << " --run <test_name>\n";
    return 2;
  }
  const auto* t = find_test(argv[2]);
  if (!t) { std::cerr << "No existe test: " << argv[2] << "\n"; return 2; }

  bool ok = t->fn();
  if (!ok) { std::cerr << "[FAIL] " << t->name << "\n"; return 1; }
  std::cout << "[OK] " << t->name << "\n"; return 0;
}
