#pragma once
#include <GameState.hpp>

extern "C" {
    #include <raylib.h>
}

class GameOverState : public GameState
{
    public:
        GameOverState();
        ~GameOverState() = default;

        void init() override;
        void handleInput() override;
        void update(float deltaTime) override;
        void render() override;

        void pause(){};
        void resume(){};

        void setPuntuacion(int p) { puntuacion = p; }
    
    private:
        int puntuacion = 0;
        int selectedOption = 0;
        Rectangle playAgainButton;
        Rectangle returnToMenuButton;
};