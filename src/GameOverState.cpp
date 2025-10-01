#include <GameOverState.hpp>
#include <iostream>
#include <MainGameState.hpp>
#include <StateMachine.hpp>

GameOverState::GameOverState(){}

void GameOverState::init(){
    
}

void GameOverState::handleInput()
{
    if (GetKeyPressed() != 0) {
        auto mainGameState = std::make_unique<MainGameState>();
        mainGameState->setStateMachine(state_machine);
        state_machine->add_state(std::move(mainGameState), true);
    }
}

void GameOverState::update(float deltaTime)
{
    this->handleInput();
}

void GameOverState::render()
{
    ClearBackground(SKYBLUE);
    BeginDrawing();
        DrawText("Game Over", (GetScreenWidth() / 2 - MeasureText("Game Over", 52) / 2), 80, 52, RED);
        DrawText(TextFormat("Puntuación: %d", puntuacion),
         (GetScreenWidth() / 2 - MeasureText(TextFormat("Puntuación: %d", puntuacion), 32) / 2), 150, 32, BLACK);
    EndDrawing();
}