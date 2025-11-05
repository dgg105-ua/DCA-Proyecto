#include <MainMenuState.hpp>
#include <iostream>
#include <StateMachine.hpp>
#include <MainGameState.hpp>
using namespace std;

extern "C" {
    #include <raylib.h>
}

MainMenuState::MainMenuState() : selectedOption(0)
{
}

void MainMenuState::init()
{
    float buttonWidth = 200;
    float buttonHeight = 50;
    float spacing = 20;

    playButton = {
        (GetScreenWidth() - buttonWidth) / 2,
        (GetScreenHeight() - (buttonHeight * 2 + spacing)) / 2,
        buttonWidth,
        buttonHeight
    };

    exitButton = {
        (GetScreenWidth() - buttonWidth) / 2,
        playButton.y + buttonHeight + spacing,
        buttonWidth,
        buttonHeight
    };
}

void MainMenuState::loadBackground()
{

}

void MainMenuState::handleInput()
{
    bool keyboardUsed = false;
    
    // Keyboard navigation
    if (IsKeyPressed(KEY_S)) {
        selectedOption = (selectedOption + 1) % 2;
        keyboardUsed = true;
    }
    else if (IsKeyPressed(KEY_W)) {
        selectedOption = (selectedOption - 1 + 2) % 2;
        keyboardUsed = true;
    }

    // Mouse navigation (only if keyboard wasn't used this frame)
    if (!keyboardUsed) {
        Vector2 mousePos = GetMousePosition();
        
        if (CheckCollisionPointRec(mousePos, playButton)) {
            selectedOption = 0;
        }
        else if (CheckCollisionPointRec(mousePos, exitButton)) {
            selectedOption = 1;
        }
    }

    // Selection with Enter key
    if (IsKeyPressed(KEY_ENTER)) {
        if (selectedOption == 0) { // Play
            auto mainGameState = std::make_unique<MainGameState>();
            mainGameState->setStateMachine(state_machine);
            state_machine->add_state(std::move(mainGameState), true);
        }
        else if (selectedOption == 1) { // Exit
            state_machine->remove_state(true);
        }
    }

    // Selection with mouse click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        
        if (CheckCollisionPointRec(mousePos, playButton)) {
            auto mainGameState = std::make_unique<MainGameState>();
            mainGameState->setStateMachine(state_machine);
            state_machine->add_state(std::move(mainGameState), true);
        }
        else if (CheckCollisionPointRec(mousePos, exitButton)) {
            state_machine->remove_state(true);
        }
    }
}

void MainMenuState::update(float deltaTime)
{
}

void MainMenuState::render()
{
    BeginDrawing();
        ClearBackground(SKYBLUE);

        // Title
        const char* title = "SALTA BINNY";
        int titleFontSize = 80;
        int titleWidth = MeasureText(title, titleFontSize);
        DrawText(title, (GetScreenWidth() - titleWidth) / 2, GetScreenHeight() / 4, titleFontSize, DARKBLUE);

        // Draw buttons
        DrawRectangleRec(playButton, (selectedOption == 0) ? GREEN : DARKGREEN);
        DrawText("Play", playButton.x + (playButton.width - MeasureText("Play", 20)) / 2, playButton.y + (playButton.height - 20) / 2, 20, WHITE);

        DrawRectangleRec(exitButton, (selectedOption == 1) ? RED : MAROON);
        DrawText("Exit", exitButton.x + (exitButton.width - MeasureText("Exit", 20)) / 2, exitButton.y + (exitButton.height - 20) / 2, 20, WHITE);

    EndDrawing();
}