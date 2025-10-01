#include <MainMenuState.hpp>
#include <iostream>
#include <StateMachine.hpp>
#include <MainGameState.hpp>

extern "C" {
    #include <raylib.h>
}

MainMenuState::MainMenuState()
{
}

void MainMenuState::init()
{
}

void MainMenuState::loadBackground()
{

}

void MainMenuState::handleInput()
{
    if (GetKeyPressed() != 0) { // Detecta cualquier tecla
        auto mainGameState = std::make_unique<MainGameState>();
        mainGameState->setStateMachine(state_machine);
        state_machine->add_state(std::move(mainGameState), true);
    }
}

void MainMenuState::update(float deltaTime)
{
    this->handleInput();
}

void MainMenuState::render()
{
    ClearBackground(SKYBLUE);

    BeginDrawing();
        DrawText("Menú", (GetScreenWidth() / 2 - MeasureText("Menú", 64) / 2), GetScreenHeight() / 2 , 64, RED);
    EndDrawing();
}