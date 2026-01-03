#include <GameOverState.hpp>
#include <iostream>
#include <MainGameState.hpp>
#include <MainMenuState.hpp>
#include <StateMachine.hpp>
#include <ResourceManager.hpp>
using namespace std;

extern "C" {
    #include <raylib.h>
}



GameOverState::GameOverState() : selectedOption(0)
{
}

void GameOverState::init()
{
    //sprites
    // Cargar recursos gráficos del Game Over con el gestor de recursos
    ResourceManager& rm = ResourceManager::instance();

    goBackgroundTex   = rm.getTexture("assets/img/gameover/gameover_background.png");
    newGameButtonTex  = rm.getTexture("assets/img/gameover/New game Button.png");
    menuButtonTex     = rm.getTexture("assets/img/gameover/Menu Button.png");
    uiFont            = rm.getFont("assets/fonts/ui.ttf");

    // sprite = tamaño del boton
    float newW  = (newGameButtonTex.id != 0) ? (float)newGameButtonTex.width  : 250.0f;
    float newH  = (newGameButtonTex.id != 0) ? (float)newGameButtonTex.height : 50.0f;
    float menuW = (menuButtonTex.id    != 0) ? (float)menuButtonTex.width    : newW;
    float menuH = (menuButtonTex.id    != 0) ? (float)menuButtonTex.height   : newH;

    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();

    playAgainButton = {
        (screenW - newW) / 2.0f,
        screenH * 0.45f - newH / 2.0f,
        newW,
        newH
    };

    returnToMenuButton = {
        (screenW - menuW) / 2.0f,
        screenH * 0.65f - menuH / 2.0f,
        menuW,
        menuH
    };
    //sprites

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

    if (!keyboardUsed) {
        Vector2 mousePos = GetMousePosition();
        
        if (CheckCollisionPointRec(mousePos, playAgainButton)) {
            selectedOption = 0;
        }
        else if (CheckCollisionPointRec(mousePos, returnToMenuButton)) {
            selectedOption = 1;
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        if (selectedOption == 0) { // Play Again
            auto mainGameState = make_unique<MainGameState>();
            mainGameState->setStateMachine(state_machine);
            state_machine->add_state(move(mainGameState), true);
        }
        else if (selectedOption == 1) { // Return to Menu
            auto mainMenuState = make_unique<MainMenuState>();
            mainMenuState->setStateMachine(state_machine);
            state_machine->add_state(move(mainMenuState), true);
        }
    }

    // Selection with mouse click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        
        if (CheckCollisionPointRec(mousePos, playAgainButton)) {
            auto mainGameState = make_unique<MainGameState>();
            mainGameState->setStateMachine(state_machine);
            state_machine->add_state(move(mainGameState), true);
        }
        else if (CheckCollisionPointRec(mousePos, returnToMenuButton)) {
            auto mainMenuState = make_unique<MainMenuState>();
            mainMenuState->setStateMachine(state_machine);
            state_machine->add_state(move(mainMenuState), true);
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

        //sprites
        // Fondo del Game Over
        ClearBackground(BLACK);
        if (goBackgroundTex.id != 0) {
            float texW    = (float)goBackgroundTex.width;
            float texH    = (float)goBackgroundTex.height;
            float screenW = (float)GetScreenWidth();
            float screenH = (float)GetScreenHeight();

            Rectangle src = { 0.0f, 0.0f, texW, texH };
            Rectangle dst = { 0.0f, 0.0f, screenW, screenH };
            Vector2 origin = { 0.0f, 0.0f };

            DrawTexturePro(goBackgroundTex, src, dst, origin, 0.0f, WHITE);
        }

        // Título y puntuación
        const char* gameOverText = "GAME OVER";
        float titleFontSize = 72.0f;
        float titleSpacing  = 2.0f;
        Vector2 titleSize = MeasureTextEx(uiFont, gameOverText, titleFontSize, titleSpacing);
        Vector2 titlePos  = {
            (GetScreenWidth()  - titleSize.x) / 2.0f,
            GetScreenHeight() * 0.12f
        };
        DrawTextEx(uiFont, gameOverText, titlePos, titleFontSize, titleSpacing, RAYWHITE);

        char scoreText[64];
        snprintf(scoreText, sizeof(scoreText), "PUNTUACION: %d", puntuacion);
        float scoreFontSize = 36.0f;
        float scoreSpacing  = 1.5f;
        Vector2 scoreSize = MeasureTextEx(uiFont, scoreText, scoreFontSize, scoreSpacing);
        Vector2 scorePos  = {
            (GetScreenWidth()  - scoreSize.x) / 2.0f,
            titlePos.y + titleSize.y + 20.0f
        };
        DrawTextEx(uiFont, scoreText, scorePos, scoreFontSize, scoreSpacing, RAYWHITE);

        // Botón NEW GAME
        Vector2 mousePos = GetMousePosition();
        bool newHover  = CheckCollisionPointRec(mousePos, playAgainButton);
        bool menuHover = CheckCollisionPointRec(mousePos, returnToMenuButton);

        Color newTint  = (selectedOption == 0 || newHover)  ? WHITE : Fade(WHITE, 0.6f);
        Color menuTint = (selectedOption == 1 || menuHover) ? WHITE : Fade(WHITE, 0.6f);

        if (newGameButtonTex.id != 0) {
            Rectangle srcNew = { 0.0f, 0.0f, (float)newGameButtonTex.width, (float)newGameButtonTex.height };
            Rectangle dstNew = { playAgainButton.x, playAgainButton.y, playAgainButton.width, playAgainButton.height };
            DrawTexturePro(newGameButtonTex, srcNew, dstNew, Vector2{0.0f, 0.0f}, 0.0f, newTint);
        } else {
            DrawRectangleRec(playAgainButton, (selectedOption == 0) ? GREEN : DARKGREEN);
            const char* txt = "PLAY AGAIN";
            DrawText(txt,
                     playAgainButton.x + (playAgainButton.width  - MeasureText(txt, 20)) / 2,
                     playAgainButton.y + (playAgainButton.height - 20) / 2,
                     20, WHITE);
        }

        // Botón MENU 
        if (menuButtonTex.id != 0) {
            Rectangle srcMenu = { 0.0f, 0.0f, (float)menuButtonTex.width, (float)menuButtonTex.height };
            Rectangle dstMenu = { returnToMenuButton.x, returnToMenuButton.y, returnToMenuButton.width, returnToMenuButton.height };
            DrawTexturePro(menuButtonTex, srcMenu, dstMenu, Vector2{0.0f, 0.0f}, 0.0f, menuTint);
        } else {
            DrawRectangleRec(returnToMenuButton, (selectedOption == 1) ? ORANGE : BROWN);
            const char* txt = "RETURN TO MENU";
            DrawText(txt,
                     returnToMenuButton.x + (returnToMenuButton.width  - MeasureText(txt, 20)) / 2,
                     returnToMenuButton.y + (returnToMenuButton.height - 20) / 2,
                     20, WHITE);
        }
        //sprites

    EndDrawing();
}
