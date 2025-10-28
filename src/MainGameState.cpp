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


void MainGameState::init()
{
    // Inicializar Jugador
    player.x = GetScreenWidth() / 2;
    player.y = -100;
    player.vx = 350.0f;
    player.vy = 0;
    player.height = 60;
    player.width = 40;

    Rectangle boundingBox = {player.x - player.width/2, player.y - player.height/2, player.width, player.height};
    player.boundingBox = boundingBox;

    // Inicializar Cámara
    camera.target = { GetScreenWidth()/2.0f, -100 };
    camera.offset = { GetScreenWidth()/2.0f, GetScreenHeight()*0.60f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Inicializar Lava
    lava.vy = 150.0f;
    Rectangle lavaRect = {0, 300, (float)GetScreenWidth(), 1000};
    lava.rect = lavaRect;

    // Inicializar puntuación
    puntuacion = 0.0f;

    // Inicializar power-up (inactivo al inicio)
    shieldPU.active = false;
    shieldPU.radius = 22.0f;
    shieldPU.color  = BLUE;
    shieldSpawnTimer = 0.0f;
    shieldSpawnInterval = GetRandomValue(10, 14);
    shieldActive = false;
    ufo = {};

    powerUp.active = false;
    powerUp.radius = 20.0f;
    powerUp.color = GREEN;
    powerUpSpawnTimer = 0.0f;
    powerUpSpawnInterval = GetRandomValue(5, 8); // Primera aparición entre 5-8 segundos (más frecuente)

    // Generación de estructuras
    generarEstructura(estructuras, 0, -50, GetScreenWidth(), 50); // Suelo inicial
    generarEstructura(estructuras, 0, -10000, 80, 10000); // Pared izquierda
    generarEstructura(estructuras, GetScreenWidth()-80, -10000, 80, 10000); // Pared derecha

    // Iconos arriba derecha
    float hudSize = 48.0f;
    float margin = 10.0f;
    float screenW = (float)GetScreenWidth();
    hudShieldRect = { screenW - hudSize - margin, margin, hudSize, hudSize };
    hudSlowRect   = { screenW - (hudSize*2.0f) - (margin*2.0f), margin, hudSize, hudSize };

    // Iconos Arriba derecha PowerUps
    if (FileExists("assets/shield.png")) {
        hudShieldTex = LoadTexture("assets/shield.png");
        hudShieldLoaded = (hudShieldTex.id != 0);
    }
    if (FileExists("assets/slow.png")) {
        hudSlowTex = LoadTexture("assets/slow.png");
        hudSlowLoaded = (hudSlowTex.id != 0);
    }

}

// No se usa, usar el de abajo (con deltaTime)
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
        // Añade 1ms al buffer de salto
        player.jumpBufferTime = 0.1f;
    }
}

void MainGameState::update(float deltaTime)
{

    puntuacion += deltaTime;

    // Gestión de colisiones
    bool enSuelo = gestionarColisiones(estructuras, player);

    gestionarSalto(player, deltaTime, enSuelo);

    handleInput(deltaTime);

    // Actualizar bounding box del jugador
    player.boundingBox.x = player.x - player.width/2;
    player.boundingBox.y = player.y - player.height/2;

    // Actualizar cámara
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
        // Interpolación lineal para suavizar el movimiento de la cámara
        float smoothFactor = 10.0f; // Factor de suavizado
        camera.target.x += (desiredCameraTarget.x - camera.target.x) * smoothFactor * deltaTime;
        camera.target.y += (desiredCameraTarget.y - camera.target.y) * smoothFactor * deltaTime;
    }
    if (!ufo.active && shieldActive && CheckCollisionRecs(player.boundingBox, lava.rect)) {
        shieldActive = false;
        iniciarRescateOVNI(player, lava.rect, this);
        player.boundingBox.x = player.x - player.width/2;
        player.boundingBox.y = player.y - player.height/2;
    }

    gestionarLava(deltaTime, lava, player, state_machine, puntuacion, estructuras);

    // Gestionar power-up
    gestionarPowerUp(powerUp, player, deltaTime, powerUpSpawnTimer, powerUpSpawnInterval);

    gestionarShieldPU(shieldPU, player, deltaTime, shieldSpawnTimer, shieldSpawnInterval, shieldActive);
    if (ufo.active) {
        actualizarRescateOVNI(player, deltaTime, this);
        player.boundingBox.x = player.x - player.width/2;
        player.boundingBox.y = player.y - player.height/2;
    }


    // Generar nuevas estructuras si es necesario
    while (ultimoY > player.y - GetScreenHeight()) {
        // Establecer ancho y alto de la plataforma
        float ancho = GetRandomValue(80, 150);
        float alto = 20;

        // Establecer distancia horizontal
        float x = ultimoX + GetRandomValue(-plataformasGapX, plataformasGapX); // X cercana a la anterior
        if (x < 80) x = GetScreenWidth()/4; // No salirse por la izquierda
        if (x > GetScreenWidth() - 160) x = (GetScreenWidth()/4)*3; // No salirse por la derecha
        ultimoX = x;

        generarEstructura(estructuras, x, ultimoY, ancho, alto);

        ultimoY -= plataformasGapY; // Siguiente más arriba
    }
}

void MainGameState::render()
{
    BeginDrawing();
        ClearBackground(DARKGRAY);
        BeginMode2D(camera);

            // Jugador
            DrawRectangleRec(player.boundingBox, RED);

            // Estructura
            for(auto& estructura : estructuras) {
                // Solo dibujar si está dentro del área visible de la cámara (con un margen de 200px)
                if(estructura.rect.y > camera.target.y - GetScreenHeight()/2 - 200 && 
                   estructura.rect.y < camera.target.y + GetScreenHeight()/2 + 200) {
                    DrawRectangleRec(estructura.rect, GRAY);
                }
                else if (estructura.rect.y < camera.target.y &&
                         estructura.rect.y + estructura.rect.height > camera.target.y) {
                    DrawRectangleRec(estructura.rect, GRAY);
                }
            }

            // Lava
            DrawRectangleRec(lava.rect, ORANGE);

            // Power-up (si está activo)
            if (powerUp.active) {
                DrawCircle(powerUp.x, powerUp.y, powerUp.radius, powerUp.color);
                // Efecto de brillo (círculo más grande transparente)
                DrawCircle(powerUp.x, powerUp.y, powerUp.radius + 5, Fade(powerUp.color, 0.3f));
            }
            if (shieldPU.active) {
                DrawCircle(shieldPU.x, shieldPU.y, shieldPU.radius, shieldPU.color);
                DrawCircleLines(shieldPU.x, shieldPU.y, shieldPU.radius + 6, DARKBLUE);
            }

            // Puntuación
            DrawText(TextFormat("Puntuación: %d", (int)puntuacion),
                            camera.target.x - camera.offset.x + 10,
                            camera.target.y - camera.offset.y + 10,
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

            // Circulo Escudo alrededor del mono
            if (shieldActive) {
                Vector2 c = { player.x, player.y };

                float base = ((player.width > player.height) ? player.width : player.height) * 0.75f + 12.0f;

                float t = (float)GetTime();
                float pulse = 2.5f * sinf(t * 3.0f);

                float r = base + pulse;

                DrawCircleLines((int)c.x, (int)c.y, r, BLUE);

                DrawCircle((int)c.x, (int)c.y, r + 4.0f, Fade(BLUE, 0.18f));
            }
            
            //icono arriba derecha escudo
            if (shieldActive) {
                DrawRectangleRec(hudShieldRect, Fade(DARKBLUE, 0.25f));
                DrawRectangleLinesEx(hudShieldRect, 2.0f, BLUE);

                if (hudShieldLoaded) {
                    Rectangle src = { 0, 0, (float)hudShieldTex.width, (float)hudShieldTex.height };
                    DrawTexturePro(hudShieldTex, src, hudShieldRect, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
                } else {
                    DrawText("SH", (int)(hudShieldRect.x + 14), (int)(hudShieldRect.y + 12), 24, BLUE);
                }
            }

            //icono arriba derecha escudo
            if (slowActive) {
                // animacion parpadeo icono cuando quedan 3 segundos
                bool blink = (slowTimeLeft <= 3.0f) && (((int)(GetTime()*6.0f)) % 2 == 0);

                Color frameColor = PURPLE;
                if (blink) {
                    DrawRectangleLinesEx(hudSlowRect, 2.0f, frameColor);
                } else {
                    DrawRectangleRec(hudSlowRect, Fade(frameColor, 0.25f));
                    DrawRectangleLinesEx(hudSlowRect, 2.0f, frameColor);

                    if (hudSlowLoaded) {
                        Rectangle src = { 0, 0, (float)hudSlowTex.width, (float)hudSlowTex.height };
                        DrawTexturePro(hudSlowTex, src, hudSlowRect, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
                    } else {
                        DrawText("SL", (int)(hudSlowRect.x + 14), (int)(hudSlowRect.y + 12), 24, PURPLE);
                    }
                }
            }


}

        EndMode2D();
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
            // Calcular la diferencia en X e Y
            float overlapLeft   = (player.boundingBox.x + player.boundingBox.width) - estructura.rect.x;
            float overlapRight  = (estructura.rect.x + estructura.rect.width) - player.boundingBox.x;
            float overlapTop    = (player.boundingBox.y + player.boundingBox.height) - estructura.rect.y;
            float overlapBottom = (estructura.rect.y + estructura.rect.height) - player.boundingBox.y;

            // Determina la menor superposición
            float minOverlapX = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;
            float minOverlapY = (overlapTop < overlapBottom) ? overlapTop : overlapBottom;

            // Mueve el jugador fuera de la colisión hacia la dirección de la menor superposición
            if (minOverlapX < minOverlapY) {
                // Mueve en X
                if (overlapLeft < overlapRight) {
                    player.x -= overlapLeft;
                } else {
                    player.x += overlapRight;
                }
            } else {
                // Mueve en Y
                if (overlapTop < overlapBottom) {
                    player.y -= overlapTop;
                    player.canJump = true; // parado en el suelo
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

    // Actualiza temporizadores de salto
    player.jumpBufferTime -= deltaTime;
    player.coyoteTime -= deltaTime;

    // Si está en el suelo, reinicia el temporizador de coyote
    if (enSuelo) {
        player.coyoteTime = 0.1f; // margen de 100ms para saltar tras caer
    }

    // Lógica de salto inicial
    if (player.jumpBufferTime > 0 && player.coyoteTime > 0) {
        player.vy = -500;
        player.canJump = false;
        player.jumpBufferTime = 0;
        player.coyoteTime = 0;
    }

    // Impulso y gravedad
    if (!enSuelo) {
        if (IsKeyDown(KEY_SPACE) && player.vy < 0) {
            // Manteniendo el salto → gravedad reducida (subida más alta)
            player.vy += gravedad * 0.3f * deltaTime;
        } else {
            // Gravedad normal
            player.vy += gravedad * deltaTime;
        }
        player.y += player.vy * deltaTime;
    }
}

void gestionarLava(float deltaTime, Lava& lava, Player& player, StateMachine* state_machine, float& puntuacion, std::deque<Estructura>& estructuras) {

    // Eliminar estructuras que estén por debajo de la lava
    // Uso de iterador para evitar problemas al borrar mientras se itera
    for (auto it = estructuras.begin(); it != estructuras.end(); ) {
    if (it->rect.y > lava.rect.y) {
        it = estructuras.erase(it); // erase devuelve el siguiente iterador
    } else {
        ++it; // avanzar solo si no borramos
    }
}   

    // Actualizar lava
    lava.rect.y -= lava.vy * deltaTime;
    if(CheckCollisionRecs(player.boundingBox, lava.rect)){
        
        auto gameOver = std::make_unique<GameOverState>();
        gameOver->setStateMachine(state_machine);
        gameOver->setPuntuacion(puntuacion); // En un futuro, pasar tiempo de partida
        state_machine->add_state(std::move(gameOver), true);
    }
}

void generarPowerUp(PowerUp& powerUp, float playerY) {
    powerUp.active = true;
    
    // Generar posición aleatoria en X (evitando los bordes)
    powerUp.x = GetRandomValue(100, GetScreenWidth() - 100);
    
    // Generar posición en Y muy por encima del jugador (fuera de la vista inicial)
    powerUp.y = playerY - GetRandomValue(800, 1200);
}

void gestionarPowerUp(PowerUp& powerUp, Player& player, float deltaTime, float& spawnTimer, float& spawnInterval) {
    // Actualizar temporizador de aparición
    spawnTimer += deltaTime;
    
    // Si el power-up no está activo y ha pasado el tiempo de intervalo, generar uno nuevo
    if (!powerUp.active && spawnTimer >= spawnInterval) {
        generarPowerUp(powerUp, player.y);
        spawnTimer = 0.0f;
        // Establecer nuevo intervalo aleatorio para la próxima aparición (más frecuente)
        spawnInterval = GetRandomValue(5, 8);
    }
    
    // Si el power-up está activo, verificar colisión con el jugador
    if (powerUp.active) {
        // Calcular distancia entre el centro del jugador y el power-up
        float playerCenterX = player.x;
        float playerCenterY = player.y;
        
        float distance = sqrtf(powf(playerCenterX - powerUp.x, 2) + powf(playerCenterY - powerUp.y, 2));
        
        // Si hay colisión (distancia menor al radio + mitad del ancho del jugador)
        if (distance < powerUp.radius + player.width / 2) {
            // Aplicar super salto (mucho más alto)
            player.vy = -1500; // Salto súper alto (el normal es -500, antes era -1200)
            player.jumpBufferTime = 0;
            player.coyoteTime = 0;
            
            // Desactivar el power-up
            powerUp.active = false;
            spawnTimer = 0.0f;
        }
        
        // Si el power-up queda muy por debajo del jugador, desactivarlo
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
