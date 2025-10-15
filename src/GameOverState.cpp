#include <GameOverState.hpp>
#include <iostream>
#include <MainGameState.hpp>
#include <MainMenuState.hpp>
#include <StateMachine.hpp>

GameOverState::GameOverState() : selectedOption(0)
{
}

void GameOverState::init()
{
    float buttonWidth = 250;
    float buttonHeight = 50;
    float spacing = 20;

    playAgainButton = {
        (GetScreenWidth() - buttonWidth) / 2,
        GetScreenHeight() / 2 + 50,
        buttonWidth,
        buttonHeight
    };

    returnToMenuButton = {
        (GetScreenWidth() - buttonWidth) / 2,
        playAgainButton.y + buttonHeight + spacing,
        buttonWidth,
        buttonHeight
    };
}

void GameOverState::handleInput()
{
    bool keyboardUsed = false;
    
    // Keyboard navigation
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        selectedOption = (selectedOption + 1) % 2;
        keyboardUsed = true;
    }
    else if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        selectedOption = (selectedOption - 1 + 2) % 2;
        keyboardUsed = true;
    }

    // Mouse navigation (only if keyboard wasn't used this frame)
    if (!keyboardUsed) {
        Vector2 mousePos = GetMousePosition();
        
        if (CheckCollisionPointRec(mousePos, playAgainButton)) {
            selectedOption = 0;
        }
        else if (CheckCollisionPointRec(mousePos, returnToMenuButton)) {
            selectedOption = 1;
        }
    }

    // Selection with Enter key
    if (IsKeyPressed(KEY_ENTER)) {
        if (selectedOption == 0) { // Play Again
            auto mainGameState = std::make_unique<MainGameState>();
            mainGameState->setStateMachine(state_machine);
            state_machine->add_state(std::move(mainGameState), true);
        }
        else if (selectedOption == 1) { // Return to Menu
            auto mainMenuState = std::make_unique<MainMenuState>();
            mainMenuState->setStateMachine(state_machine);
            state_machine->add_state(std::move(mainMenuState), true);
        }
    }

    // Selection with mouse click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        
        if (CheckCollisionPointRec(mousePos, playAgainButton)) {
            auto mainGameState = std::make_unique<MainGameState>();
            mainGameState->setStateMachine(state_machine);
            state_machine->add_state(std::move(mainGameState), true);
        }
        else if (CheckCollisionPointRec(mousePos, returnToMenuButton)) {
            auto mainMenuState = std::make_unique<MainMenuState>();
            mainMenuState->setStateMachine(state_machine);
            state_machine->add_state(std::move(mainMenuState), true);
        }
    }
}

void GameOverState::update(float deltaTime)
{
    // handleInput is already called from main loop
}

void GameOverState::render()
{
    BeginDrawing();
        ClearBackground(SKYBLUE);
        
        // Game Over title
        const char* gameOverText = "GAME OVER";
        int gameOverFontSize = 80;
        int gameOverWidth = MeasureText(gameOverText, gameOverFontSize);
        DrawText(gameOverText, (GetScreenWidth() - gameOverWidth) / 2, GetScreenHeight() / 4, gameOverFontSize, RED);
        
        // Score
        const char* scoreText = TextFormat("Puntuación: %d", puntuacion);
        int scoreFontSize = 40;
        int scoreWidth = MeasureText(scoreText, scoreFontSize);
        DrawText(scoreText, (GetScreenWidth() - scoreWidth) / 2, GetScreenHeight() / 4 + 100, scoreFontSize, BLACK);

        // Draw buttons
        DrawRectangleRec(playAgainButton, (selectedOption == 0) ? GREEN : DARKGREEN);
        const char* playAgainText = "PLAY AGAIN";
        DrawText(playAgainText, 
                 playAgainButton.x + (playAgainButton.width - MeasureText(playAgainText, 20)) / 2, 
                 playAgainButton.y + (playAgainButton.height - 20) / 2, 
                 20, WHITE);

        DrawRectangleRec(returnToMenuButton, (selectedOption == 1) ? ORANGE : BROWN);
        const char* menuText = "RETURN TO MENU";
        DrawText(menuText, 
                 returnToMenuButton.x + (returnToMenuButton.width - MeasureText(menuText, 20)) / 2, 
                 returnToMenuButton.y + (returnToMenuButton.height - 20) / 2, 
                 20, WHITE);

    EndDrawing();
}