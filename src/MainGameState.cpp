#include <MainGameState.hpp>
#include <iostream>

MainGameState::MainGameState()
{
}

void MainGameState::init()
{
    player.x = GetScreenWidth() / 2;
    player.y = GetScreenHeight() - 100;
    player.vx = 350.0f;
    player.vy = 0;
    player.height = 60;
    player.width = 40;

    camera.target = { GetScreenWidth()/2.0f, player.y };
    camera.offset = { GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
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
    
    player.vy += gravedad * deltaTime;
    player.y += player.vy * deltaTime;

    camera.target = { (float)GetScreenWidth()/2, player.y };
}

void MainGameState::render()
{
    BeginDrawing();
        ClearBackground(DARKGRAY);
        BeginMode2D(camera);

            DrawEllipse(player.x, player.y, player.width/2, player.height/2, RED);

            DrawRectangle(GetScreenWidth()/2, GetScreenHeight()/2, 40, 40, GREEN);
        
        EndMode2D();
    EndDrawing();
}