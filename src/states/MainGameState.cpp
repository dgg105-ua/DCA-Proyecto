#include <MainGameState.hpp>
#include <iostream>
#include <GameOverState.hpp>
#include <cmath>
#include <ResourceManager.hpp>

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
    player.height = 80;
    player.width = 70;

    Rectangle boundingBox = {player.x - player.width/2, player.y - player.height/2, player.width, player.height};
    player.boundingBox = boundingBox;

    camera.target = { GetScreenWidth()/2.0f, -100 };
    camera.offset = { GetScreenWidth()/2.0f, GetScreenHeight()*0.60f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    lava.vy = 150.0f;
    Rectangle lavaRect = {0, 300, (float)GetScreenWidth(), 500};
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

    float worldWidth  = GetScreenWidth() * 0.80f;
    float marginX     = (GetScreenWidth() - worldWidth) / 2.0f;
    float wallWidth   = 80.0f;
    float floorHeight = 50.0f;

    // Mapa original, ancho completo
    generarEstructura(estructuras, 0, -50, GetScreenWidth(), 50);
    generarEstructura(estructuras, 0, -10000, 80, 10000);
    generarEstructura(estructuras, GetScreenWidth()-80, -10000, 80, 10000);

    float hudSize = 48.0f;
    float margin = 10.0f;
    float screenW = (float)GetScreenWidth();
    hudShieldRect = { screenW - hudSize - margin, margin, hudSize, hudSize };
    hudSlowRect   = { screenW - (hudSize*2.0f) - (margin*2.0f), margin, hudSize, hudSize };
    hudDoubleRect = { screenW - (hudSize*3.0f) - (margin*3.0f), margin, hudSize, hudSize };

    const char* SHIELD_CANDIDATES[] = {
        "assets/img/powerupsc/shield.png",
        "src/assets/img/powerupsc/shield.png"
    };
    for (const char* p : SHIELD_CANDIDATES) {
        if (FileExists(p)) { hudShieldTex = LoadTexture(p); hudShieldLoaded = (hudShieldTex.id != 0); break; }
    }

    const char* SLOW_CANDIDATES[] = {
        "assets/img/powerupsc/slow.png",
        "src/assets/img/powerupsc/slow.png"
    };
    for (const char* p : SLOW_CANDIDATES) {
        if (FileExists(p)) { hudSlowTex = LoadTexture(p); hudSlowLoaded = (hudSlowTex.id != 0); break; }
    }

    const char* DOUBLE_CANDIDATES[] = {
        "assets/img/powerupsc/double.png",
        "src/assets/img/powerupsc/double.png"
    };
    for (const char* p : DOUBLE_CANDIDATES) {
        if (FileExists(p)) { hudDoubleTex = LoadTexture(p); hudDoubleLoaded = (hudDoubleTex.id != 0); break; }
    }

    //sprites
    // Cargar sprites a través del gestor de recursos 
    ResourceManager& rm = ResourceManager::instance();

    // Personaje
    playerIdleTexture = rm.getTexture("assets/img/player/Idle.png");
    playerRunTexture  = rm.getTexture("assets/img/player/Run.png");
    playerJumpTexture = rm.getTexture("assets/img/player/Jump.png");

    // Plataforma
    platformTexture   = rm.getTexture("assets/img/world/platform.png");

    // Fuente para HUD
    uiFont = rm.getFont("assets/fonts/ui.ttf");

    // PU
    jumpPUTexture        = rm.getTexture("assets/img/powerups/jump.png");
    shieldPUWorldTexture = rm.getTexture("assets/img/powerups/shield.png");
    slowPUTexture        = rm.getTexture("assets/img/powerups/slow.png");
    doublePUTexture      = rm.getTexture("assets/img/powerups/double.png");

    // OVNI de rescate
    ufoTexture = ResourceManager::instance().getTexture("assets/img/world/ufo.png");

    // lava
    lavaTexture = rm.getTexture("assets/img/world/lava.png");

    // Inicializar estado de animación del jugador
    playerAnimState    = PLAYER_IDLE;
    playerCurrentFrame = 0;
    playerFrameTimer   = 0.0f;
    playerFrameSpeed   = 8.0f;
    playerFacingRight  = true;
    //sprites
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
    if (deltaTime > 0.05f) deltaTime = 0.05f;

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

    // lava borra pu y estructuras
    auto borraSiColisionaConLava = [&](PowerUp& pu) {
        if (!pu.active) return;

        float r = pu.radius;
        Rectangle puRect = {
            pu.x - r,
            pu.y - r,
            2.0f * r,
            2.0f * r
        };

        if (CheckCollisionRecs(lava.rect, puRect)) {
            pu.active = false;
        }
    };

    borraSiColisionaConLava(powerUp);
    borraSiColisionaConLava(shieldPU);
    borraSiColisionaConLava(slowPU);
    borraSiColisionaConLava(doublePU);

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

    //sprites
    // animación del jugador

    // Estado: suelo / aire
    if (!enSuelo) {
        playerAnimState = PLAYER_JUMP;
    } else {
        bool moviendo = IsKeyDown(KEY_A) || IsKeyDown(KEY_D);
        playerAnimState = moviendo ? PLAYER_RUN : PLAYER_IDLE;
    }

    // Dirección en la que mira el mono
    if (IsKeyDown(KEY_A)) {
        playerFacingRight = false;
    } else if (IsKeyDown(KEY_D)) {
        playerFacingRight = true;
    }

    // Avance de los frames de animación
    playerFrameTimer += dt;
    int maxFrames = 1;
    switch (playerAnimState) {
        case PLAYER_IDLE: maxFrames = PLAYER_IDLE_FRAMES; break;
        case PLAYER_RUN:  maxFrames = PLAYER_RUN_FRAMES;  break;
        case PLAYER_JUMP: maxFrames = PLAYER_JUMP_FRAMES; break;
    }
    if (maxFrames <= 0) maxFrames = 1;

    if (playerFrameTimer >= 1.0f / playerFrameSpeed) {
        playerFrameTimer = 0.0f;
        playerCurrentFrame++;
        if (playerCurrentFrame >= maxFrames) {
            playerCurrentFrame = 0;
        }
    }
    //sprites
}

void MainGameState::render()
{
    BeginDrawing();
    ClearBackground(DARKGRAY);

    BeginMode2D(camera);

        // Fondo del nivel
        {
            ResourceManager& rm = ResourceManager::instance();
            Texture2D bgTex = rm.getTexture("assets/img/world/background.png");

            if (bgTex.id != 0) {
                float screenW = (float)GetScreenWidth();
                float screenH = (float)GetScreenHeight();

                Rectangle src = {
                    0.0f,
                    0.0f,
                    (float)bgTex.width,
                    (float)bgTex.height
                };

                Rectangle dst = {
                    camera.target.x - camera.offset.x,
                    camera.target.y - camera.offset.y,
                    screenW,
                    screenH
                };

                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(bgTex, src, dst, origin, 0.0f, WHITE);
            }
        }

        //sprites
        // Dibujar el personaje con sprites
        {
            Texture2D currentTexture = playerIdleTexture;
            int maxFrames = PLAYER_IDLE_FRAMES;

            switch (playerAnimState) {
                case PLAYER_IDLE:
                    currentTexture = playerIdleTexture;
                    maxFrames = PLAYER_IDLE_FRAMES;
                    break;
                case PLAYER_RUN:
                    currentTexture = (playerRunTexture.id != 0) ? playerRunTexture : playerIdleTexture;
                    maxFrames = (playerRunTexture.id != 0) ? PLAYER_RUN_FRAMES : PLAYER_IDLE_FRAMES;
                    break;
                case PLAYER_JUMP:
                    currentTexture = (playerJumpTexture.id != 0) ? playerJumpTexture : playerIdleTexture;
                    maxFrames = (playerJumpTexture.id != 0) ? PLAYER_JUMP_FRAMES : PLAYER_IDLE_FRAMES;
                    break;
            }

            if (maxFrames <= 0) maxFrames = 1;
            int frameIndex = playerCurrentFrame % maxFrames;

            Rectangle sourceRect = {
                (float)(frameIndex * PLAYER_SPRITE_W),
                0.0f,
                playerFacingRight ? (float)PLAYER_SPRITE_W : -(float)PLAYER_SPRITE_W,
                (float)PLAYER_SPRITE_H
            };

            // Escalamos el sprite para aproximarlo al tamaño de la hitbox
            float scaleX = player.width  / (float)PLAYER_SPRITE_W;
            float scaleY = player.height / (float)PLAYER_SPRITE_H;

            Rectangle destRect = {
                player.x,
                player.y,
                PLAYER_SPRITE_W * scaleX,
                PLAYER_SPRITE_H * scaleY
            };

            Vector2 origin = { destRect.width / 2.0f, destRect.height / 2.0f };

            DrawTexturePro(currentTexture, sourceRect, destRect, origin, 0.0f, WHITE);
        }

        // Dibujar estructuras
        for (auto& estructura : estructuras) {
            bool visible =
                (estructura.rect.y > camera.target.y - GetScreenHeight()/2 - 200 &&
                 estructura.rect.y < camera.target.y + GetScreenHeight()/2 + 200) ||
                (estructura.rect.y < camera.target.y &&
                 estructura.rect.y + estructura.rect.height > camera.target.y);

            if (!visible) continue;

            Rectangle r = estructura.rect;

            // El suelo es la primera estructura
            bool isFloor = (&estructura == &estructuras.front());

            bool isLeftWall = !isFloor &&
                              r.x <= 1.0f &&
                              r.width >= 70.0f && r.width <= 90.0f &&
                              r.height > 1000.0f;

            bool isRightWall = !isFloor &&
                               (r.x + r.width) >= (GetScreenWidth() - 1.0f) &&
                               r.width >= 70.0f && r.width <= 90.0f &&
                               r.height > 1000.0f;

            if (isFloor) {
                ResourceManager& rm = ResourceManager::instance();
                Texture2D floorTexture = rm.getTexture("assets/img/world/floor.png");

                if (floorTexture.id != 0) {
                    float scale     = r.height / (float)floorTexture.height;
                    float tileWidth = floorTexture.width * scale;
                    float startX    = r.x;
                    float endX      = r.x + r.width;

                    Rectangle srcBase = { 0.0f, 0.0f,
                                          (float)floorTexture.width,
                                          (float)floorTexture.height };

                    for (float x = startX; x < endX; x += tileWidth) {
                        float remaining = endX - x;
                        float drawWidth = tileWidth;
                        Rectangle src   = srcBase;

                        if (remaining < tileWidth) {
                            float ratio = remaining / tileWidth;
                            src.width  *= ratio;
                            drawWidth   = remaining;
                        }

                        Rectangle dst = {
                            x + drawWidth / 2.0f,
                            r.y + r.height / 2.0f,
                            drawWidth,
                            r.height
                        };
                        Vector2 origin = { dst.width / 2.0f, dst.height / 2.0f };

                        DrawTexturePro(floorTexture, src, dst, origin, 0.0f, WHITE);
                    }
                } else {
                    DrawRectangleRec(r, GRAY);
                }
            }
            else if (isLeftWall) {
                ResourceManager& rm = ResourceManager::instance();
                Texture2D leftWallTex = rm.getTexture("assets/img/world/wallizq.png");

                if (leftWallTex.id != 0) {
                    float scale      = r.width / (float)leftWallTex.width;
                    float tileHeight = leftWallTex.height * scale;
                    float startY     = r.y;
                    float endY       = r.y + r.height;

                    Rectangle srcBase = { 0.0f, 0.0f,
                                          (float)leftWallTex.width,
                                          (float)leftWallTex.height };

                    for (float y = startY; y < endY; y += tileHeight) {
                        float remaining  = endY - y;
                        float drawHeight = tileHeight;
                        Rectangle src    = srcBase;

                        if (remaining < tileHeight) {
                            float ratio = remaining / tileHeight;
                            src.height *= ratio;
                            drawHeight  = remaining;
                        }

                        Rectangle dst = {
                            r.x + r.width / 2.0f,
                            y + drawHeight / 2.0f,
                            r.width,
                            drawHeight
                        };
                        Vector2 origin = { dst.width / 2.0f, dst.height / 2.0f };

                        DrawTexturePro(leftWallTex, src, dst, origin, 0.0f, WHITE);
                    }
                } else {
                    DrawRectangleRec(r, GRAY);
                }
            }
            else if (isRightWall) {
                ResourceManager& rm = ResourceManager::instance();
                Texture2D rightWallTex = rm.getTexture("assets/img/world/wallder.png");

                if (rightWallTex.id != 0) {
                    float scale      = r.width / (float)rightWallTex.width;
                    float tileHeight = rightWallTex.height * scale;
                    float startY     = r.y;
                    float endY       = r.y + r.height;

                    Rectangle srcBase = { 0.0f, 0.0f,
                                          (float)rightWallTex.width,
                                          (float)rightWallTex.height };

                    for (float y = startY; y < endY; y += tileHeight) {
                        float remaining  = endY - y;
                        float drawHeight = tileHeight;
                        Rectangle src    = srcBase;

                        if (remaining < tileHeight) {
                            float ratio = remaining / tileHeight;
                            src.height *= ratio;
                            drawHeight  = remaining;
                        }

                        Rectangle dst = {
                            r.x + r.width / 2.0f,
                            y + drawHeight / 2.0f,
                            r.width,
                            drawHeight
                        };
                        Vector2 origin = { dst.width / 2.0f, dst.height / 2.0f };

                        DrawTexturePro(rightWallTex, src, dst, origin, 0.0f, WHITE);
                    }
                } else {
                    DrawRectangleRec(r, GRAY);
                }
            }
            else if (platformTexture.id != 0 && r.height <= 60.0f) {
                // Plataformas (rectángulos bajos) -> sprite de plataforma
                Rectangle src = {
                    0.0f,
                    0.0f,
                    (float)platformTexture.width,
                    (float)platformTexture.height
                };

                Rectangle dst = {
                    r.x,
                    r.y,
                    r.width,
                    r.height
                };

                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(platformTexture, src, dst, origin, 0.0f, WHITE);
            } else {
                // Cualquier otra estructura rara
                DrawRectangleRec(r, GRAY);
            }
        }
        //sprites

        if (lavaTexture.id != 0) {
            Rectangle src = {
                0.0f,
                0.0f,
                (float)lavaTexture.width,
                (float)lavaTexture.height
            };

            Rectangle dst = {
                lava.rect.x + lava.rect.width / 2.0f,
                lava.rect.y + lava.rect.height / 2.0f,
                lava.rect.width,
                lava.rect.height
            };

            Vector2 origin = { dst.width / 2.0f, dst.height / 2.0f };

            DrawTexturePro(lavaTexture, src, dst, origin, 0.0f, WHITE);
        } else {
            DrawRectangleRec(lava.rect, ORANGE);
        }


         //sprites PU

        // salto
        if (powerUp.active) {
            float size = powerUp.radius * 2.0f;

            if (jumpPUTexture.id != 0) {
                Rectangle src = {
                    0.0f,
                    0.0f,
                    (float)jumpPUTexture.width,
                    (float)jumpPUTexture.height
                };
                Rectangle dst = {
                    powerUp.x,
                    powerUp.y,
                    size,
                    size
                };
                Vector2 origin = { dst.width / 2.0f, dst.height / 2.0f };

                DrawTexturePro(jumpPUTexture, src, dst, origin, 0.0f, WHITE);
            }

            DrawCircleLines(powerUp.x, powerUp.y, powerUp.radius, GREEN);
        }

        // escudo
        if (shieldPU.active) {
            float size = shieldPU.radius * 2.0f;

            if (shieldPUWorldTexture.id != 0) {
                Rectangle src = {
                    0.0f,
                    0.0f,
                    (float)shieldPUWorldTexture.width,
                    (float)shieldPUWorldTexture.height
                };
                Rectangle dst = {
                    shieldPU.x,
                    shieldPU.y,
                    size,
                    size
                };
                Vector2 origin = { dst.width / 2.0f, dst.height / 2.0f };

                DrawTexturePro(shieldPUWorldTexture, src, dst, origin, 0.0f, WHITE);
            }

            DrawCircleLines(shieldPU.x, shieldPU.y, shieldPU.radius, BLUE);
        }

        // slow
        if (slowPU.active) {
            float size = slowPU.radius * 2.0f;

            if (slowPUTexture.id != 0) {
                Rectangle src = {
                    0.0f,
                    0.0f,
                    (float)slowPUTexture.width,
                    (float)slowPUTexture.height
                };
                Rectangle dst = {
                    slowPU.x,
                    slowPU.y,
                    size,
                    size
                };
                Vector2 origin = { dst.width / 2.0f, dst.height / 2.0f };

                DrawTexturePro(slowPUTexture, src, dst, origin, 0.0f, WHITE);
            }

            DrawCircleLines(slowPU.x, slowPU.y, slowPU.radius, PURPLE);
        }

        // doble puntuación
        if (doublePU.active) {
            float size = doublePU.radius * 2.0f;

            if (doublePUTexture.id != 0) {
                Rectangle src = {
                    0.0f,
                    0.0f,
                    (float)doublePUTexture.width,
                    (float)doublePUTexture.height
                };
                Rectangle dst = {
                    doublePU.x,
                    doublePU.y,
                    size,
                    size
                };
                Vector2 origin = { dst.width / 2.0f, dst.height / 2.0f };

                DrawTexturePro(doublePUTexture, src, dst, origin, 0.0f, WHITE);
            }

            DrawCircleLines(doublePU.x, doublePU.y, doublePU.radius, GOLD);
        }
        //sprites PU


        // Puntuación usando la fuente gestionada por el ResourceManager
        {
            const char* scoreText = TextFormat("Puntuacion: %d", (int)puntuacion);
            Vector2 scorePos = {
                camera.target.x - camera.offset.x + 10.0f,
                camera.target.y - camera.offset.y + 10.0f
            };
            DrawTextEx(uiFont, scoreText, scorePos, 24.0f, 1.0f, BLACK);
        }

        // OVNI con sprite a tamaño pequeño
        if (ufo.active && ufoTexture.id != 0) {
            float spriteW = (float)ufoTexture.width;
            float spriteH = (float)ufoTexture.height;

            // Queremos algo parecido al UFO antiguo (≈80 px de ancho)
            float targetWidth  = 80.0f;
            float scale        = targetWidth / spriteW;
            float targetHeight = spriteH * scale;

            Rectangle src = {
                0.0f,
                0.0f,
                spriteW,
                spriteH
            };

            Rectangle dst = {
                ufo.x,            // centro X
                ufo.y,            // centro Y
                targetWidth,
                targetHeight
            };

            Vector2 origin = { dst.width / 2.0f, dst.height / 2.0f };

            DrawTexturePro(ufoTexture, src, dst, origin, 0.0f, WHITE);

            // Rayo tractor debajo del ovni
            float beamBaseY      = ufo.y + targetHeight * 0.25f;
            float beamHalfWidth  = 20.0f;

            Vector2 a = { ufo.x - beamHalfWidth, beamBaseY };
            Vector2 b = { ufo.x + beamHalfWidth, beamBaseY };
            Vector2 c = { player.x, player.y + player.height/2 + 10 };

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
    const int gravedad = 1500;

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

void gestionarLava(float deltaTime, Lava& lava, Player& player,
                   StateMachine* state_machine, float& puntuacion,
                   std::deque<Estructura>& estructuras) {
    // De momento NO tocamos las estructuras (suelo, paredes, plataformas)
    (void)estructuras; // para evitar warning de parámetro sin usar

    // La lava sube (y disminuye)
    lava.rect.y -= lava.vy * deltaTime;

    // Si toca al jugador -> Game Over
    if (CheckCollisionRecs(player.boundingBox, lava.rect)) {
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
