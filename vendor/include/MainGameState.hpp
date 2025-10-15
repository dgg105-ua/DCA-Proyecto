#pragma once
#include <GameState.hpp>
#include <StateMachine.hpp>
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

    // Mecanicas de salto
    float jumpBufferTime = 0.0f;    // Buffer de salto: Permite saltar un poco antes de tocar el suelo
    float coyoteTime = 0.0f;    // Tiempo de coyote: Permite saltar un poco después de haber salido del suelo
};

struct Estructura{
    Rectangle rect;
};

struct Lava{
    Rectangle rect;
    float vy;
};

struct PowerUp{
    float x;
    float y;
    float radius;
    bool active;
    Color color;
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
        float puntuacion = 0.0f;

        std::deque<Estructura> estructuras; // Estructuras en el juego
        Lava lava;

        // Sistema de power-ups
        PowerUp powerUp;
        float powerUpSpawnTimer = 0.0f;
        float powerUpSpawnInterval = 8.0f; // Aparece cada 8-15 segundos (aleatorio)

        float plataformasGapX = 400.0f;   // Distancia horizontal entre plataformas
        float plataformasGapY = 150.0f;   // Distancia vertical entre plataformas
        float ultimoX = GetScreenWidth()/2; // Posición X de primera plataforma generada
        float ultimoY = -200; // Posición Y de primera plataforma generada

        bool primerFrame = true;
};