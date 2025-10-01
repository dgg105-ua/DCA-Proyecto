#include <MainGameState.hpp>
#include <iostream>
#include <GameOverState.hpp>

MainGameState::MainGameState()
{
}

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

    camera.target = { GetScreenWidth()/2.0f, player.y };
    camera.offset = { GetScreenWidth()/2.0f, GetScreenHeight()*0.60f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    lava.vy = 20.0f;
    Rectangle lavaRect = {0, 300, (float)GetScreenWidth(), 300};
    lava.rect = lavaRect;

    Estructura estructura;
    estructura.rect.x = 0;
    estructura.rect.y = -40;
    estructura.rect.width = GetScreenWidth();
    estructura.rect.height = 40;
    estructuras.push_back(estructura);

    estructura.rect.x = 200;
    estructura.rect.y = -150;
    estructura.rect.width = 200;
    estructura.rect.height = 20;
    estructuras.push_back(estructura);
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
    if (IsKeyDown(KEY_SPACE) && player.canJump){
        player.vy = -500;
        player.canJump = false;
    }
}

void MainGameState::update(float deltaTime)
{
    handleInput(deltaTime);

    const int gravedad = 1000;

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

    if(!enSuelo){
        player.vy += gravedad * deltaTime;
        player.y += player.vy * deltaTime;
    }

    player.boundingBox.x = player.x - player.width/2;
    player.boundingBox.y = player.y - player.height/2;

    if(player.y < -GetScreenHeight()*0.40f){
        camera.target = { GetScreenWidth()/2.0f, player.y };
    }
    else{
        camera.target = { GetScreenWidth()/2.0f, -GetScreenHeight()*0.40f };
    }

    lava.rect.y -= lava.vy * deltaTime;
    if(CheckCollisionRecs(player.boundingBox, lava.rect)){
        auto gameOverState = std::make_unique<GameOverState>();
        gameOverState->setStateMachine(state_machine);
        //gameOverState->setPuntuacion(0); // En un futuro, pasar tiempo de partida
        state_machine->add_state(std::move(gameOverState), true);
    }
}

void MainGameState::render()
{
    BeginDrawing();
        ClearBackground(DARKGRAY);
        BeginMode2D(camera);

            DrawRectangleRec(player.boundingBox, RED);

            for(auto& estructura : estructuras) {
                DrawRectangleRec(estructura.rect, LIGHTGRAY);
            }

            DrawRectangleRec(lava.rect, ORANGE);
        
        EndMode2D();
    EndDrawing();
}