#pragma once
#include <GameState.hpp>

extern "C" {
    #include <raylib.h>
}

class MainMenuState : public GameState
{
    public:
        MainMenuState();
        ~MainMenuState() = default;

        void init() override;
        void handleInput() override;
        void update(float deltaTime) override;
        void render() override;

        void pause(){};
        void resume(){};

        void loadBackground();

    private:
        int selectedOption;
        Rectangle playButton;
        Rectangle exitButton;

        //sprites
        // Texturas del menú cargadas po ResourceManager
        Texture2D menuBackgroundTex{};
        Texture2D playButtonTex{};
        Texture2D exitButtonTex{};

        // Fuente para el título del juego
        Font uiFont{};
        //sprites
};
