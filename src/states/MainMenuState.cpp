#include <MainMenuState.hpp>
#include <iostream>
#include <StateMachine.hpp>
#include <MainGameState.hpp>
#include <ResourceManager.hpp>
using namespace std;

extern "C" {
    #include <raylib.h>
}


MainMenuState::MainMenuState() : selectedOption(0)
{
}

void MainMenuState::init()
{
    //sprites
    // Cargar recursos gráficos del menú con el gestor de recursos
    ResourceManager& rm = ResourceManager::instance();

    // Fondo del menú
    menuBackgroundTex = rm.getTexture("assets/img/menu/menu_background.png");

    // Botones exit y play (imagenes)
    playButtonTex = rm.getTexture("assets/img/menu/Play Button.png");
    exitButtonTex = rm.getTexture("assets/img/menu/Exit Button.png");

    // Fuente para el título del juego
    uiFont = rm.getFont("assets/fonts/ui.ttf");


    // Tamaños botones
    float playW  = (playButtonTex.id != 0) ? (float)playButtonTex.width  : 400.0f;
    float playH  = (playButtonTex.id != 0) ? (float)playButtonTex.height : 100.0f;
    float exitW  = (exitButtonTex.id != 0) ? (float)exitButtonTex.width  : playW;
    float exitH  = (exitButtonTex.id != 0) ? (float)exitButtonTex.height : playH;

    float screenW = (float)GetScreenWidth();
    float screenH = (float)GetScreenHeight();

    float spacing = 20.0f; // separación vertical entre botones

    // sprite play tamaño boton
    playButton = {
        (screenW - playW) / 2.0f,
        (screenH - (playH + spacing + exitH)) / 2.0f,
        playW,
        playH
    };

    // sprite exit tamaño boton
    exitButton = {
        (screenW - exitW) / 2.0f,
        playButton.y + playButton.height + spacing,
        exitW,
        exitH
    };
    //sprites
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

        //sprites
        // fondo del menu
        ClearBackground(BLACK);
        if (menuBackgroundTex.id != 0) {
            float texW    = (float)menuBackgroundTex.width;
            float texH    = (float)menuBackgroundTex.height;
            float screenW = (float)GetScreenWidth();
            float screenH = (float)GetScreenHeight();

            Rectangle src = { 0.0f, 0.0f, texW, texH };
            Rectangle dst = { 0.0f, 0.0f, screenW, screenH };
            Vector2 origin = { 0.0f, 0.0f };

            DrawTexturePro(menuBackgroundTex, src, dst, origin, 0.0f, WHITE);
        }
        const char* title = "Project Prime-8";
        float titleFontSize = 64.0f;
        float titleSpacing  = 2.0f;

        Vector2 titleSize = MeasureTextEx(uiFont, title, titleFontSize, titleSpacing);
        Vector2 titlePos = {
            (GetScreenWidth()  - titleSize.x) / 2.0f,
            GetScreenHeight() * 0.18f
        };
        DrawTextEx(uiFont, title, titlePos, titleFontSize, titleSpacing, RAYWHITE);


        // botones dibujados
        Vector2 mousePos = GetMousePosition();
        bool playHover = CheckCollisionPointRec(mousePos, playButton);
        bool exitHover = CheckCollisionPointRec(mousePos, exitButton);

        Color playTint = (selectedOption == 0 || playHover) ? WHITE : Fade(WHITE, 0.6f);
        Color exitTint = (selectedOption == 1 || exitHover) ? WHITE : Fade(WHITE, 0.6f);

        if (playButtonTex.id != 0) {
            Rectangle srcPlay = { 0.0f, 0.0f, (float)playButtonTex.width, (float)playButtonTex.height };
            Rectangle dstPlay = { playButton.x, playButton.y, playButton.width, playButton.height };
            DrawTexturePro(playButtonTex, srcPlay, dstPlay, Vector2{0.0f, 0.0f}, 0.0f, playTint);
        } else {
            DrawRectangleRec(playButton, (selectedOption == 0) ? GREEN : DARKGREEN);
            DrawText("Play",
                     playButton.x + (playButton.width - MeasureText("Play", 20)) / 2,
                     playButton.y + (playButton.height - 20) / 2,
                     20, WHITE);
        }

        if (exitButtonTex.id != 0) {
            Rectangle srcExit = { 0.0f, 0.0f, (float)exitButtonTex.width, (float)exitButtonTex.height };
            Rectangle dstExit = { exitButton.x, exitButton.y, exitButton.width, exitButton.height };
            DrawTexturePro(exitButtonTex, srcExit, dstExit, Vector2{0.0f, 0.0f}, 0.0f, exitTint);
        } else {
            DrawRectangleRec(exitButton, (selectedOption == 1) ? RED : MAROON);
            DrawText("Exit",
                     exitButton.x + (exitButton.width - MeasureText("Exit", 20)) / 2,
                     exitButton.y + (exitButton.height - 20) / 2,
                     20, WHITE);
        }
        //sprites

    EndDrawing();
}
