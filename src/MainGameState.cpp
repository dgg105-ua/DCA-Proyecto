#include <MainGameState.hpp>
#include <iostream>

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
    if (IsKeyDown(KEY_A)) player.x -= player.vx * deltaTime;
    if (IsKeyDown(KEY_D)) player.x += player.vx * deltaTime;
    if (IsKeyPressed(KEY_SPACE)) {
        player.vy = -500;
        //player.canJump = false;
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
        
        EndMode2D();
    EndDrawing();
}