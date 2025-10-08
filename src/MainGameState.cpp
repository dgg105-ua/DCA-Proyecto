#include <MainGameState.hpp>
#include <iostream>
#include <GameOverState.hpp>

MainGameState::MainGameState()
{
}

void generarEstructura(std::deque<Estructura>& estructuras, float x, float y, float width, float height);
bool gestionarColisiones(std::deque<Estructura>& estructuras, Player& player);
void gestionarSalto(Player& player, float deltaTime, bool& enSuelo);
void gestionarLava(float deltaTime, Lava& lava, Player& player, StateMachine* state_machine, float& puntuacion, std::deque<Estructura>& estructuras);

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
    camera.target = { GetScreenWidth()/2.0f, player.y };
    camera.offset = { GetScreenWidth()/2.0f, GetScreenHeight()*0.60f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Inicializar Lava
    lava.vy = 150.0f;
    Rectangle lavaRect = {0, 300, (float)GetScreenWidth(), 1000};
    lava.rect = lavaRect;

    // Inicializar puntuación
    puntuacion = 0.0f;

    // Generación de estructuras
    generarEstructura(estructuras, 0, -50, GetScreenWidth(), 50); // Suelo inicial
    generarEstructura(estructuras, 0, -10000, 80, 10000); // Pared izquierda
    generarEstructura(estructuras, GetScreenWidth()-80, -10000, 80, 10000); // Pared derecha

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
    if(player.y < -GetScreenHeight()*0.40f){
        camera.target = { GetScreenWidth()/2.0f, player.y };
    }
    else{
        camera.target = { GetScreenWidth()/2.0f, -GetScreenHeight()*0.40f };
    }

    gestionarLava(deltaTime, lava, player, state_machine, puntuacion, estructuras);

    // Generar nuevas estructuras si es necesario
    while (ultimoY > player.y - GetScreenHeight()) {
        // Establecer ancho y alto de la plataforma
        float ancho = GetRandomValue(80, 150);
        float alto = 20;

        // Establecer distancia horizontal
        float x = ultimoX + GetRandomValue(-plataformasGapX, plataformasGapX); // X cercana a la anterior
        if (x < 80) x = GetScreenWidth()/2; // No salirse por la izquierda
        if (x > GetScreenWidth() - 160) x = GetScreenWidth()/2; // No salirse por la derecha
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
                DrawRectangleRec(estructura.rect, LIGHTGRAY);
            }

            // Lava
            DrawRectangleRec(lava.rect, ORANGE);
        
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