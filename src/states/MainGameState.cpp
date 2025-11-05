#include <MainGameState.hpp>
#include <iostream>
#include <GameOverState.hpp>
#include <cmath>

MainGameState::MainGameState()
{
}

void generarEstructura(std::deque<Estructura>& estructuras, float x, float y, float width, float height);
bool gestionarColisiones(std::deque<Estructura>& estructuras, Player& player);
void gestionarSalto(Player& player, float deltaTime, bool& enSuelo);
void gestionarLava(float deltaTime, Lava& lava, Player& player, StateMachine* state_machine, float& puntuacion, std::deque<Estructura>& estructuras);
void generarPowerUp(PowerUp& powerUp, float playerY);
void gestionarPowerUp(PowerUp& powerUp, Player& player, float deltaTime, float& spawnTimer, float& spawnInterval);
void generarShieldPU(PowerUp& shieldPU, float playerY);
void gestionarShieldPU(PowerUp& shieldPU, Player& player, float deltaTime, float& spawnTimer, float& spawnInterval, bool& shieldActive);
void iniciarRescateOVNI(Player& player, const Rectangle& lavaRect, MainGameState* self);
void actualizarRescateOVNI(Player& player, float deltaTime, MainGameState* self);
void generarSlowPU(PowerUp& slowPU, float playerY);
void gestionarSlowPU(PowerUp& slowPU, Player& player, float deltaTime, float& spawnTimer, float& spawnInterval, bool& slowActive, float& slowTimeLeft, float slowDuration, float& timeScale);
void generarDoublePU(PowerUp& doublePU, float playerY);
void gestionarDoublePU(PowerUp& doublePU, Player& player, float deltaTime, float& spawnTimer, float& spawnInterval, bool& doubleActive, float& doubleTimeLeft, float doubleDuration, float& puntuacionX);

void MainGameState::init()
{
    player.x = GetScreenWidth() / 2;
    player.y = -100;
    player.vx = 350.0f;
    player.vy = 0;
    player.height = 60;
    player.width = 40;

    Rectangle boundingBox = {player.x - player.width/2, player.y - player.height/2, player.width, player.height};
    player.boundingBox = boundingBox;

    camera.target = { GetScreenWidth()/2.0f, -100 };
    camera.offset = { GetScreenWidth()/2.0f, GetScreenHeight()*0.60f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    lava.vy = 150.0f;
    Rectangle lavaRect = {0, 300, (float)GetScreenWidth(), 1000};
    lava.rect = lavaRect;

    puntuacion = 0.0f;

    shieldPU.active = false;
    shieldPU.radius = 22.0f;
    shieldPU.color  = BLUE;
    shieldSpawnTimer = 0.0f;
    shieldSpawnInterval = GetRandomValue(8, 12);
    shieldActive = false;
    ufo = {};

    slowPU.active = false;
    slowPU.radius = 22.0f;
    slowPU.color  = PURPLE;
    slowSpawnTimer = 0.0f;
    slowSpawnInterval = GetRandomValue(12, 16);
    slowActive = false;
    slowTimeLeft = 0.0f;
    timeScale = 1.0f;

    doublePU.active = false;
    doublePU.radius = 22.0f;
    doublePU.color  = GOLD;
    doubleSpawnTimer = 0.0f;
    doubleSpawnInterval = GetRandomValue(14, 20);
    doubleActive = false;
    doubleTimeLeft = 0.0f;
    puntuacionX = 1.0f;

    powerUp.active = false;
    powerUp.radius = 20.0f;
    powerUp.color = GREEN;
    powerUpSpawnTimer = 0.0f;
    powerUpSpawnInterval = GetRandomValue(5, 8);

    generarEstructura(estructuras, 0, -50, GetScreenWidth(), 50);
    generarEstructura(estructuras, 0, -10000, 80, 10000);
    generarEstructura(estructuras, GetScreenWidth()-80, -10000, 80, 10000);

    float hudSize = 48.0f;
    float margin = 10.0f;
    float screenW = (float)GetScreenWidth();
    hudShieldRect = { screenW - hudSize - margin, margin, hudSize, hudSize };
    hudSlowRect   = { screenW - (hudSize*2.0f) - (margin*2.0f), margin, hudSize, hudSize };
    hudDoubleRect = { screenW - (hudSize*3.0f) - (margin*3.0f), margin, hudSize, hudSize };

    const char* SHIELD_CANDIDATES[] = { "assets/shield.png", "src/assets/shield.png" };
    for (const char* p : SHIELD_CANDIDATES) {
        if (FileExists(p)) { hudShieldTex = LoadTexture(p); hudShieldLoaded = (hudShieldTex.id != 0); break; }
    }

    const char* SLOW_CANDIDATES[] = { "assets/slow.png", "src/assets/slow.png" };
    for (const char* p : SLOW_CANDIDATES) {
        if (FileExists(p)) { hudSlowTex = LoadTexture(p); hudSlowLoaded = (hudSlowTex.id != 0); break; }
    }

    const char* DOUBLE_CANDIDATES[] = { "assets/double.png", "src/assets/double.png" };
        for (const char* p : DOUBLE_CANDIDATES) {
            if (FileExists(p)) { hudDoubleTex = LoadTexture(p); hudDoubleLoaded = (hudDoubleTex.id != 0); break; }
        }
}

void MainGameState::handleInput(){
}

void MainGameState::handleInput(float deltaTime)
{
    if (IsKeyDown(KEY_A)){
        if(player.x > 0) player.x -= player.vx * deltaTime;
    }
    if (IsKeyDown(KEY_D)){
        if(player.x < GetScreenWidth()) player.x += player.vx * deltaTime;
    }
    if (IsKeyPressed(KEY_SPACE)){
        player.jumpBufferTime = 0.1f;
    }
}

void MainGameState::update(float deltaTime)
{
    puntuacion += deltaTime * puntuacionX;

    timeScale = slowActive ? 0.4f : 1.0f;
    float dt = deltaTime * timeScale;

    bool enSuelo = gestionarColisiones(estructuras, player);

    gestionarSalto(player, dt, enSuelo);
    handleInput(dt);

    player.boundingBox.x = player.x - player.width/2;
    player.boundingBox.y = player.y - player.height/2;

    Vector2 desiredCameraTarget;
    if(player.y < -GetScreenHeight()*0.40f){
        desiredCameraTarget = { GetScreenWidth()/2.0f, player.y };
    }
    else{
        desiredCameraTarget = { GetScreenWidth()/2.0f, -GetScreenHeight()*0.40f };
    }

    if(primerFrame){
        camera.target = desiredCameraTarget;
        primerFrame = false;
    } else{
        float smoothFactor = 10.0f;
        camera.target.x += (desiredCameraTarget.x - camera.target.x) * smoothFactor * dt;
        camera.target.y += (desiredCameraTarget.y - camera.target.y) * smoothFactor * dt;
    }

    if (!ufo.active && shieldActive && CheckCollisionRecs(player.boundingBox, lava.rect)) {
        shieldActive = false;
        iniciarRescateOVNI(player, lava.rect, this);
        player.boundingBox.x = player.x - player.width/2;
        player.boundingBox.y = player.y - player.height/2;
    }

    gestionarLava(dt, lava, player, state_machine, puntuacion, estructuras);

    gestionarPowerUp(powerUp, player, dt, powerUpSpawnTimer, powerUpSpawnInterval);

    gestionarShieldPU(shieldPU, player, dt, shieldSpawnTimer, shieldSpawnInterval, shieldActive);

    if (ufo.active) {
        actualizarRescateOVNI(player, dt, this);
        player.boundingBox.x = player.x - player.width/2;
        player.boundingBox.y = player.y - player.height/2;
    }

    gestionarSlowPU(slowPU, player, dt, slowSpawnTimer, slowSpawnInterval,
                    slowActive, slowTimeLeft, slowDuration, timeScale);

    if (slowActive && !ufo.active) {
        slowTimeLeft -= deltaTime;
        if (slowTimeLeft <= 0.0f) {
            slowActive = false;
            timeScale = 1.0f;
            slowTimeLeft = 0.0f;
        }
    }

    while (ultimoY > player.y - GetScreenHeight()) {
        float ancho = GetRandomValue(80, 150);
        float alto = 20;
        float x = ultimoX + GetRandomValue(-plataformasGapX, plataformasGapX);
        if (x < 80) x = GetScreenWidth()/4;
        if (x > GetScreenWidth() - 160) x = (GetScreenWidth()/4)*3;
        ultimoX = x;
        generarEstructura(estructuras, x, ultimoY, ancho, alto);
        ultimoY -= plataformasGapY;
    }

    gestionarDoublePU(doublePU, player, dt, doubleSpawnTimer, doubleSpawnInterval,doubleActive, doubleTimeLeft, doubleDuration, puntuacionX);

    if (doubleActive) {
    doubleTimeLeft -= deltaTime;
    if (doubleTimeLeft <= 0.0f) {
        doubleActive = false;
        doubleTimeLeft = 0.0f;
        puntuacionX = 1.0f;
    }
}

}

void MainGameState::render()
{
    BeginDrawing();
    ClearBackground(DARKGRAY);

    BeginMode2D(camera);

        DrawRectangleRec(player.boundingBox, RED);

        for(auto& estructura : estructuras) {
            if(estructura.rect.y > camera.target.y - GetScreenHeight()/2 - 200 &&
               estructura.rect.y < camera.target.y + GetScreenHeight()/2 + 200) {
                DrawRectangleRec(estructura.rect, GRAY);
            }
            else if (estructura.rect.y < camera.target.y &&
                     estructura.rect.y + estructura.rect.height > camera.target.y) {
                DrawRectangleRec(estructura.rect, GRAY);
            }
        }

        DrawRectangleRec(lava.rect, ORANGE);

        if (powerUp.active) {
            DrawCircle(powerUp.x, powerUp.y, powerUp.radius, GREEN);
            DrawCircle(powerUp.x, powerUp.y, powerUp.radius + 5, Fade(GREEN, 0.3f));
        }

        if (shieldPU.active) {
            DrawCircle(shieldPU.x, shieldPU.y, shieldPU.radius, BLUE);
            DrawCircleLines(shieldPU.x, shieldPU.y, shieldPU.radius + 6, DARKBLUE);
        }

        if (slowPU.active) {
            DrawCircle(slowPU.x, slowPU.y, slowPU.radius, PURPLE);
            DrawCircle(slowPU.x, slowPU.y, slowPU.radius + 5, Fade(PURPLE, 0.3f));
        }

        if (doublePU.active) {
            DrawCircle(doublePU.x, doublePU.y, doublePU.radius, GOLD);
            DrawCircle(doublePU.x, doublePU.y, doublePU.radius + 5, Fade(GOLD, 0.3f));
        }

        DrawText(TextFormat("Puntuacion: %d", (int)puntuacion),
                 (int)(camera.target.x - camera.offset.x + 10),
                 (int)(camera.target.y - camera.offset.y + 10),
                 24, BLACK);

        if (ufo.active) {
            DrawEllipse(ufo.x, ufo.y, 40, 14, GRAY);
            DrawEllipseLines(ufo.x, ufo.y, 40, 14, DARKGRAY);
            DrawCircle(ufo.x, ufo.y - 6, 8, LIGHTGRAY);
            Vector2 a = { ufo.x - 20, ufo.y + 8 };
            Vector2 b = { ufo.x + 20, ufo.y + 8 };
            Vector2 c = { player.x,   player.y + player.height/2 + 10 };
            DrawTriangle(a, b, c, Fade(SKYBLUE, 0.35f));
            DrawTriangleLines(a, b, c, Fade(DARKBLUE, 0.5f));
        }

        if (shieldActive) {
            Vector2 cc = { player.x, player.y };
            float base = ((player.width > player.height) ? player.width : player.height) * 0.75f + 12.0f;
            float t = (float)GetTime();
            float pulse = 2.5f * sinf(t * 3.0f);
            float r = base + pulse;
            DrawCircleLines((int)cc.x, (int)cc.y, r, BLUE);
            DrawCircle((int)cc.x, (int)cc.y, r + 4.0f, Fade(BLUE, 0.18f));
        }

        if (doublePU.active) {
            DrawCircle(doublePU.x, doublePU.y, doublePU.radius, GOLD);
            DrawCircle(doublePU.x, doublePU.y, doublePU.radius + 5, Fade(GOLD, 0.3f));
        }

    EndMode2D();

    {
        float hudSize = 48.0f;
        float margin  = 10.0f;
        float screenW = (float)GetScreenWidth();

        Rectangle shieldRect = { screenW - hudSize - margin, margin, hudSize, hudSize };
        Rectangle slowRect   = { screenW - (hudSize*2.0f) - (margin*2.0f), margin, hudSize, hudSize };

        if (shieldActive) {
            DrawRectangleRec(shieldRect, Fade(DARKBLUE, 0.25f));
            DrawRectangleLinesEx(shieldRect, 2.0f, BLUE);
            if (hudShieldLoaded) {
                Rectangle src = { 0, 0, (float)hudShieldTex.width, (float)hudShieldTex.height };
                DrawTexturePro(hudShieldTex, src, shieldRect, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
            } else {
                DrawText("SH", (int)(shieldRect.x + 14), (int)(shieldRect.y + 12), 24, BLUE);
            }
        }

        if (slowActive) {
            bool blink = (slowTimeLeft <= 3.0f) && (((int)(GetTime()*6.0f)) % 2 == 0);
            if (blink) {
                DrawRectangleLinesEx(slowRect, 2.0f, PURPLE);
            } else {
                DrawRectangleRec(slowRect, Fade(PURPLE, 0.25f));
                DrawRectangleLinesEx(slowRect, 2.0f, PURPLE);
                if (hudSlowLoaded) {
                    Rectangle src = { 0, 0, (float)hudSlowTex.width, (float)hudSlowTex.height };
                    DrawTexturePro(hudSlowTex, src, slowRect, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
                } else {
                    DrawText("SL", (int)(slowRect.x + 14), (int)(slowRect.y + 12), 24, PURPLE);
                }
            }
        }

        if (doubleActive) {
            bool blink = (doubleTimeLeft <= 3.0f) && (((int)(GetTime()*6.0f)) % 2 == 0);
            if (blink) {
                DrawRectangleLinesEx(hudDoubleRect, 2.0f, GOLD);
            } else {
                DrawRectangleRec(hudDoubleRect, Fade(GOLD, 0.25f));
                DrawRectangleLinesEx(hudDoubleRect, 2.0f, GOLD);
                if (hudDoubleLoaded) {
                    Rectangle src = { 0, 0, (float)hudDoubleTex.width, (float)hudDoubleTex.height };
                    DrawTexturePro(hudDoubleTex, src, hudDoubleRect, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
                } else {
                    DrawText("x2", (int)(hudDoubleRect.x + 12), (int)(hudDoubleRect.y + 12), 24, GOLD);
                }
            }
        }
    }

    EndDrawing();
}

void generarEstructura(std::deque<Estructura>& estructuras, float x, float y, float width, float height) {
    Estructura estructura;
    estructura.rect.x = x;
    estructura.rect.y = y;
    estructura.rect.width = width;
    estructura.rect.height = height;
    estructuras.push_back(estructura);
}

bool gestionarColisiones(std::deque<Estructura>& estructuras, Player& player) {
    bool enSuelo = false;
    for(auto& estructura : estructuras) {
        if (CheckCollisionRecs(player.boundingBox, estructura.rect)) {
            float overlapLeft   = (player.boundingBox.x + player.boundingBox.width) - estructura.rect.x;
            float overlapRight  = (estructura.rect.x + estructura.rect.width) - player.boundingBox.x;
            float overlapTop    = (player.boundingBox.y + player.boundingBox.height) - estructura.rect.y;
            float overlapBottom = (estructura.rect.y + estructura.rect.height) - player.boundingBox.y;

            float minOverlapX = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;
            float minOverlapY = (overlapTop < overlapBottom) ? overlapTop : overlapBottom;

            if (minOverlapX < minOverlapY) {
                if (overlapLeft < overlapRight) {
                    player.x -= overlapLeft;
                } else {
                    player.x += overlapRight;
                }
            } else {
                if (overlapTop < overlapBottom) {
                    player.y -= overlapTop;
                    player.canJump = true;
                    enSuelo = true;
                } else {
                    player.y += overlapBottom;
                }
                player.vy = 0;
            }
        }
    }
    return enSuelo;
}

void gestionarSalto(Player& player, float deltaTime, bool& enSuelo) {
    const int gravedad = 2000;

    player.jumpBufferTime -= deltaTime;
    player.coyoteTime -= deltaTime;

    if (enSuelo) {
        player.coyoteTime = 0.1f;
    }

    if (player.jumpBufferTime > 0 && player.coyoteTime > 0) {
        player.vy = -500;
        player.canJump = false;
        player.jumpBufferTime = 0;
        player.coyoteTime = 0;
    }

    if (!enSuelo) {
        if (IsKeyDown(KEY_SPACE) && player.vy < 0) {
            player.vy += gravedad * 0.3f * deltaTime;
        } else {
            player.vy += gravedad * deltaTime;
        }
        player.y += player.vy * deltaTime;
    }
}

void gestionarLava(float deltaTime, Lava& lava, Player& player, StateMachine* state_machine, float& puntuacion, std::deque<Estructura>& estructuras) {
    for (auto it = estructuras.begin(); it != estructuras.end(); ) {
        if (it->rect.y > lava.rect.y) {
            it = estructuras.erase(it);
        } else {
            ++it;
        }
    }

    lava.rect.y -= lava.vy * deltaTime;
    if(CheckCollisionRecs(player.boundingBox, lava.rect)){
        auto gameOver = std::make_unique<GameOverState>();
        gameOver->setStateMachine(state_machine);
        gameOver->setPuntuacion(puntuacion);
        state_machine->add_state(std::move(gameOver), true);
    }
}

void generarPowerUp(PowerUp& powerUp, float playerY) {
    powerUp.active = true;
    powerUp.x = GetRandomValue(100, GetScreenWidth() - 100);
    powerUp.y = playerY - GetRandomValue(800, 1200);
}

void gestionarPowerUp(PowerUp& powerUp, Player& player, float deltaTime, float& spawnTimer, float& spawnInterval) {
    spawnTimer += deltaTime;

    if (!powerUp.active && spawnTimer >= spawnInterval) {
        generarPowerUp(powerUp, player.y);
        spawnTimer = 0.0f;
        spawnInterval = GetRandomValue(5, 8);
    }

    if (powerUp.active) {
        float playerCenterX = player.x;
        float playerCenterY = player.y;
        float distance = sqrtf(powf(playerCenterX - powerUp.x, 2) + powf(playerCenterY - powerUp.y, 2));

        if (distance < powerUp.radius + player.width / 2) {
            player.vy = -1500;
            player.jumpBufferTime = 0;
            player.coyoteTime = 0;

            powerUp.active = false;
            spawnTimer = 0.0f;
        }

        if (powerUp.y > player.y + GetScreenHeight()) {
            powerUp.active = false;
        }
    }
}

void generarShieldPU(PowerUp& shieldPU, float playerY) {
    shieldPU.active = true;
    shieldPU.x = GetRandomValue(100, GetScreenWidth() - 100);
    shieldPU.y = playerY - GetRandomValue(900, 1400);
}

void gestionarShieldPU(PowerUp& shieldPU, Player& player, float deltaTime, float& spawnTimer, float& spawnInterval, bool& shieldActive) {
    spawnTimer += deltaTime;
    if (!shieldPU.active && !shieldActive && spawnTimer >= spawnInterval) {
        generarShieldPU(shieldPU, player.y);
        spawnTimer = 0.0f;
        spawnInterval = GetRandomValue(10, 14);
    }
    if (shieldPU.active) {
        float dx = player.x - shieldPU.x;
        float dy = player.y - shieldPU.y;
        float dist2 = dx*dx + dy*dy;
        float sumR = shieldPU.radius + player.width * 0.5f;
        if (dist2 < sumR * sumR) {
            shieldActive = true;
            player.vy = -600;
            shieldPU.active = false;
            spawnTimer = 0.0f;
        }
        if (shieldPU.y > player.y + GetScreenHeight()) {
            shieldPU.active = false;
        }
    }
}

void iniciarRescateOVNI(Player& player, const Rectangle& lavaRect, MainGameState* self) {
    auto& ufo = self->ufo;
    ufo.active = true;
    ufo.timer = 0.0f;
    ufo.duration = 1.2f;
    ufo.vy = -900.0f;

    ufo.x = player.x;
    ufo.y = lavaRect.y - 80;

    player.y = lavaRect.y - player.height - 6;
    player.vy = -900.0f;
}

void actualizarRescateOVNI(Player& player, float deltaTime, MainGameState* self) {
    auto& ufo = self->ufo;

    ufo.timer += deltaTime;
    ufo.y += ufo.vy * deltaTime;

    float objetivoJugadorY = ufo.y + 70.0f;
    player.y += (objetivoJugadorY - player.y) * 10.0f * deltaTime;
    player.vy = -900.0f;

    player.x += (ufo.x - player.x) * 8.0f * deltaTime;

    if (ufo.timer >= ufo.duration) {
        ufo.active = false;
    }
}

void generarSlowPU(PowerUp& slowPU, float playerY) {
    slowPU.active = true;
    slowPU.x = GetRandomValue(100, GetScreenWidth() - 100);
    slowPU.y = playerY - GetRandomValue(900, 1400);
}

void gestionarSlowPU(PowerUp& slowPU, Player& player, float deltaTime,
                     float& spawnTimer, float& spawnInterval,
                     bool& slowActive, float& slowTimeLeft, float slowDuration, float& timeScale) {
    spawnTimer += deltaTime;

    if (!slowPU.active && !slowActive && spawnTimer >= spawnInterval) {
        generarSlowPU(slowPU, player.y);
        spawnTimer = 0.0f;
        spawnInterval = GetRandomValue(12, 16);
    }

    if (slowPU.active) {
        float dx = player.x - slowPU.x;
        float dy = player.y - slowPU.y;
        float dist2 = dx*dx + dy*dy;
        float sumR = slowPU.radius + player.width * 0.5f;

        if (dist2 < sumR * sumR) {
            slowActive   = true;
            slowTimeLeft = slowDuration;
            timeScale    = 0.4f;
            player.vy   -= 200.0f;
            slowPU.active = false;
            spawnTimer = 0.0f;
        }

        if (slowPU.y > player.y + GetScreenHeight()) {
            slowPU.active = false;
        }
    }
}

void generarDoublePU(PowerUp& doublePU, float playerY) {
    doublePU.active = true;
    doublePU.x = GetRandomValue(100, GetScreenWidth() - 100);
    doublePU.y = playerY - GetRandomValue(900, 1400);
}

void gestionarDoublePU(PowerUp& doublePU, Player& player, float deltaTime,
                       float& spawnTimer, float& spawnInterval,
                       bool& doubleActive, float& doubleTimeLeft,
                       float doubleDuration, float& scoreRateMult) {
    spawnTimer += deltaTime;

    if (!doublePU.active && !doubleActive && spawnTimer >= spawnInterval) {
        generarDoublePU(doublePU, player.y);
        spawnTimer = 0.0f;
        spawnInterval = GetRandomValue(14, 20);
    }

    if (doublePU.active) {
        float dx = player.x - doublePU.x;
        float dy = player.y - doublePU.y;
        float dist2 = dx*dx + dy*dy;
        float sumR = doublePU.radius + player.width * 0.5f;

        if (dist2 < sumR * sumR) {
            doubleActive   = true;
            doubleTimeLeft = doubleDuration;
            scoreRateMult  = 2.0f;
            doublePU.active = false;
            spawnTimer = 0.0f;
        }

        if (doublePU.y > player.y + GetScreenHeight()) {
            doublePU.active = false;
        }
    }
}