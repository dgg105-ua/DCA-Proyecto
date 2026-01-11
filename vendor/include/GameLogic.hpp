#pragma once

#include <deque>
#include <MainGameState.hpp>

// Estructuras / colisiones / salto
void generarEstructura(std::deque<Estructura>& estructuras, float x, float y, float width, float height);
bool gestionarColisiones(std::deque<Estructura>& estructuras, Player& player);
void gestionarSalto(Player& player, float deltaTime, bool& enSuelo);

// Lava (en fichero separado para que los tests no dependan de GameOverState)
void gestionarLava(float deltaTime, Lava& lava, Player& player,
                   StateMachine* state_machine, float& puntuacion,
                   std::deque<Estructura>& estructuras);

// PowerUps (jump)
void generarPowerUp(PowerUp& powerUp, float playerY);
void gestionarPowerUp(PowerUp& powerUp, Player& player, float deltaTime, float& spawnTimer, float& spawnInterval);

// PowerUp Escudo
void generarShieldPU(PowerUp& shieldPU, float playerY);
void gestionarShieldPU(PowerUp& shieldPU, Player& player, float deltaTime,
                       float& spawnTimer, float& spawnInterval, bool& shieldActive);

// Rescate OVNI
void iniciarRescateOVNI(Player& player, const Rectangle& lavaRect, MainGameState* self);
void actualizarRescateOVNI(Player& player, float deltaTime, MainGameState* self);

// PowerUp Slow
void generarSlowPU(PowerUp& slowPU, float playerY);
void gestionarSlowPU(PowerUp& slowPU, Player& player, float deltaTime,
                     float& spawnTimer, float& spawnInterval,
                     bool& slowActive, float& slowTimeLeft, float slowDuration, float& timeScale);

// PowerUp Doble puntuación
void generarDoublePU(PowerUp& doublePU, float playerY);
void gestionarDoublePU(PowerUp& doublePU, Player& player, float deltaTime,
                       float& spawnTimer, float& spawnInterval,
                       bool& doubleActive, float& doubleTimeLeft,
                       float doubleDuration, float& scoreRateMult);
