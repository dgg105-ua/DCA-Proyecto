#pragma once
#include <GameState.hpp>
#include <deque>

extern "C" {
    #include <raylib.h>
}

struct Player{
    float x;
    float y;
    float vx;
    float vy;
    float height;
    float width;
    bool canJump = true;
    Rectangle boundingBox;
};

struct Estructura{
    Rectangle rect;
};

struct Lava{
    Rectangle rect;
    float vy;
}; 

class MainGameState : public GameState
{
    public:
        MainGameState();
        ~MainGameState() = default;

        void init() override;
        void handleInput() override;
        void handleInput(float deltaTime);
        void update(float deltaTime) override;
        void render() override;

        void pause(){};
        void resume(){};

    
    private:
        char entered_key;

        Player player;
        Camera2D camera;

        std::deque<Estructura> estructuras; // Estructuras en el juego
        Lava lava;
};