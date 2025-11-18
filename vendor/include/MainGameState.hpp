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
    float coyoteTime = 0.0f;        // Tiempo de coyote: Permite saltar un poco después de haber salido del suelo
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

        // =========================
        //   SPRITES / ANIMACIÓN
        // =========================
        enum class PlayerAnimState {
            Idle,
            Run,
            Jump
        };

        Texture2D playerIdleTex = {0};
        Texture2D playerRunTex  = {0};
        Texture2D playerJumpTex = {0};

        Rectangle playerSrcRect = {0};
        Rectangle playerDestRect = {0};

        PlayerAnimState playerAnimState = PlayerAnimState::Idle;
        int   playerDir = 1;          //  1 = derecha, -1 = izquierda
        int   playerFrame = 0;
        float playerFrameTime = 0.0f;

        int   playerIdleFrames  = 0;
        int   playerRunFrames   = 0;
        int   playerJumpFrames  = 0;

        float playerIdleFrameDur  = 0.022f; // mayor = más lento
        float playerRunFrameDur   = 0.016f;
        float playerJumpFrameDur  = 0.018f;

        bool  playerMovingHoriz = false;
        bool  playerIsOnGround  = false;

        // Tamaño visual del sprite (32x32 * escala)
        float playerSpriteScale = 2.5f; // cambia esto para hacer el mono más grande/pequeño

        // Hitbox más pequeña que el mono
        float hitboxScale = 0.8f;       // 0.6 = 60% del ancho/alto del jugador     
};
