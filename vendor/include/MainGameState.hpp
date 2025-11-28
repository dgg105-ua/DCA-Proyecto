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

        friend void iniciarRescateOVNI(Player& player, const Rectangle& lavaRect, MainGameState* self);
        friend void actualizarRescateOVNI(Player& player, float deltaTime, MainGameState* self);


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
        float powerUpSpawnInterval = 8.0f;

        PowerUp shieldPU;
        float shieldSpawnTimer = 0.0f;
        float shieldSpawnInterval = 10.0f;
        bool  shieldActive = false;

        // UFO PU escudo
        struct RescueUFO {
            bool active = false;
            float x = 0.0f;
            float y = 0.0f;
            float vy = -800.0f;
            float timer = 0.0f;
            float duration = 1.2f;
        } ufo;

        // PU realentizador
        PowerUp slowPU;
        float slowSpawnTimer = 0.0f;
        float slowSpawnInterval = 12.0f;
        bool  slowActive = false;
        float slowTimeLeft = 0.0f;
        const float slowDuration = 10.0f;
        float timeScale = 1.0f;

        // PU x2
        PowerUp doublePU;
        float doubleSpawnTimer = 0.0f;
        float doubleSpawnInterval = 14.0f;
        bool  doubleActive = false;
        float doubleTimeLeft = 0.0f;
        const float doubleDuration = 10.0f;
        float puntuacionX = 1.0f;

        // ICONOS ARRIBA DERECHA DE LOS POWERUPS
        Texture2D hudShieldTex = {0}; bool hudShieldLoaded = false;
        Rectangle hudShieldRect = {0};
        Texture2D hudSlowTex   = {0}; bool hudSlowLoaded   = false;
        Rectangle hudSlowRect   = {0};
        Texture2D hudDoubleTex = {0}; bool hudDoubleLoaded = false;
        Rectangle hudDoubleRect = {0};

        float plataformasGapX = 400.0f;   // Distancia horizontal entre plataformas
        float plataformasGapY = 150.0f;   // Distancia vertical entre plataformas
        float ultimoX = GetScreenWidth()/2; // Posición X de primera plataforma generada
        float ultimoY = -200; // Posición Y de primera plataforma generada

        bool primerFrame = true;

        //SPRITE
        // Texturas y animación del personaje (mono)
        Texture2D playerIdleTexture = {0};
        Texture2D playerRunTexture  = {0};
        Texture2D playerJumpTexture = {0};

        enum PlayerAnimState { PLAYER_IDLE, PLAYER_RUN, PLAYER_JUMP };
        PlayerAnimState playerAnimState = PLAYER_IDLE;

        int   playerCurrentFrame = 0;
        float playerFrameTimer   = 0.0f;
        float playerFrameSpeed   = 12.0f; // fps de la animación
        bool  playerFacingRight  = true;

        static constexpr int PLAYER_SPRITE_WIDTH  = 32;
        static constexpr int PLAYER_SPRITE_HEIGHT = 32;
        static constexpr int PLAYER_IDLE_FRAMES   = 18;
        static constexpr int PLAYER_RUN_FRAMES    = 8;
        static constexpr int PLAYER_JUMP_FRAMES   = 4;
        //SPRITE
};
