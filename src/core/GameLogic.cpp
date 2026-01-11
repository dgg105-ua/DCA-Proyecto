#include <GameLogic.hpp>
#include <cmath>

void generarEstructura(std::deque<Estructura>& estructuras, float x, float y, float width, float height) {
    Estructura estructura;
    estructura.rect.x = x;
    estructura.rect.y = y;
    estructura.rect.width = width;
    estructura.rect.height = height;
    estructuras.push_back(estructura);
}

bool gestionarColisiones(std::deque<Estructura>& estructuras, Player& player) {
    bool enSuelo = false;
    for(auto& estructura : estructuras) {
        if (CheckCollisionRecs(player.boundingBox, estructura.rect)) {
            float overlapLeft   = (player.boundingBox.x + player.boundingBox.width) - estructura.rect.x;
            float overlapRight  = (estructura.rect.x + estructura.rect.width) - player.boundingBox.x;
            float overlapTop    = (player.boundingBox.y + player.boundingBox.height) - estructura.rect.y;
            float overlapBottom = (estructura.rect.y + estructura.rect.height) - player.boundingBox.y;

            float minOverlapX = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;
            float minOverlapY = (overlapTop < overlapBottom) ? overlapTop : overlapBottom;

            if (minOverlapX < minOverlapY) {
                if (overlapLeft < overlapRight) {
                    player.x -= overlapLeft;
                } else {
                    player.x += overlapRight;
                }
            } else {
                if (overlapTop < overlapBottom) {
                    player.y -= overlapTop;
                    player.canJump = true;
                    enSuelo = true;
                } else {
                    player.y += overlapBottom;
                }
                player.vy = 0;
            }
        }
    }
    return enSuelo;
}

void gestionarSalto(Player& player, float deltaTime, bool& enSuelo) {
    const int gravedad = 1500;

    player.jumpBufferTime -= deltaTime;
    player.coyoteTime -= deltaTime;

    if (enSuelo) {
        player.coyoteTime = 0.1f;
    }

    if (player.jumpBufferTime > 0 && player.coyoteTime > 0) {
        player.vy = -500;
        player.canJump = false;
        player.jumpBufferTime = 0;
        player.coyoteTime = 0;
    }

    if (!enSuelo) {
        if (IsKeyDown(KEY_SPACE) && player.vy < 0) {
            player.vy += gravedad * 0.3f * deltaTime;
        } else {
            player.vy += gravedad * deltaTime;
        }
        player.y += player.vy * deltaTime;
    }
}

void generarPowerUp(PowerUp& powerUp, float playerY) {
    powerUp.active = true;
    powerUp.x = GetRandomValue(100, GetScreenWidth() - 100);
    powerUp.y = playerY - GetRandomValue(800, 1200);
}

void gestionarPowerUp(PowerUp& powerUp, Player& player, float deltaTime, float& spawnTimer, float& spawnInterval) {
    spawnTimer += deltaTime;

    if (!powerUp.active && spawnTimer >= spawnInterval) {
        generarPowerUp(powerUp, player.y);
        spawnTimer = 0.0f;
        spawnInterval = GetRandomValue(5, 8);
    }

    if (powerUp.active) {
        float playerCenterX = player.x;
        float playerCenterY = player.y;
        float distance = sqrtf(powf(playerCenterX - powerUp.x, 2) + powf(playerCenterY - powerUp.y, 2));

        if (distance < powerUp.radius + player.width / 2) {
            player.vy = -1500;
            player.jumpBufferTime = 0;
            player.coyoteTime = 0;

            powerUp.active = false;
            spawnTimer = 0.0f;
        }

        if (powerUp.y > player.y + GetScreenHeight()) {
            powerUp.active = false;
        }
    }
}

void generarShieldPU(PowerUp& shieldPU, float playerY) {
    shieldPU.active = true;
    shieldPU.x = GetRandomValue(100, GetScreenWidth() - 100);
    shieldPU.y = playerY - GetRandomValue(900, 1400);
}

void gestionarShieldPU(PowerUp& shieldPU, Player& player, float deltaTime, float& spawnTimer, float& spawnInterval, bool& shieldActive) {
    spawnTimer += deltaTime;
    if (!shieldPU.active && !shieldActive && spawnTimer >= spawnInterval) {
        generarShieldPU(shieldPU, player.y);
        spawnTimer = 0.0f;
        spawnInterval = GetRandomValue(10, 14);
    }
    if (shieldPU.active) {
        float dx = player.x - shieldPU.x;
        float dy = player.y - shieldPU.y;
        float dist2 = dx*dx + dy*dy;
        float sumR = shieldPU.radius + player.width * 0.5f;
        if (dist2 < sumR * sumR) {
            shieldActive = true;
            player.vy = -600;
            shieldPU.active = false;
            spawnTimer = 0.0f;
        }
        if (shieldPU.y > player.y + GetScreenHeight()) {
            shieldPU.active = false;
        }
    }
}

void iniciarRescateOVNI(Player& player, const Rectangle& lavaRect, MainGameState* self) {
    auto& ufo = self->ufo;
    ufo.active = true;
    ufo.timer = 0.0f;
    ufo.duration = 1.2f;
    ufo.vy = -900.0f;

    ufo.x = player.x;
    ufo.y = lavaRect.y - 80;

    player.y = lavaRect.y - player.height - 6;
    player.vy = -900.0f;
}

void actualizarRescateOVNI(Player& player, float deltaTime, MainGameState* self) {
    auto& ufo = self->ufo;

    ufo.timer += deltaTime;
    ufo.y += ufo.vy * deltaTime;

    float objetivoJugadorY = ufo.y + 70.0f;
    player.y += (objetivoJugadorY - player.y) * 10.0f * deltaTime;
    player.vy = -900.0f;

    player.x += (ufo.x - player.x) * 8.0f * deltaTime;

    if (ufo.timer >= ufo.duration) {
        ufo.active = false;
    }
}

void generarSlowPU(PowerUp& slowPU, float playerY) {
    slowPU.active = true;
    slowPU.x = GetRandomValue(100, GetScreenWidth() - 100);
    slowPU.y = playerY - GetRandomValue(900, 1400);
}

void gestionarSlowPU(PowerUp& slowPU, Player& player, float deltaTime,
                     float& spawnTimer, float& spawnInterval,
                     bool& slowActive, float& slowTimeLeft, float slowDuration, float& timeScale) {
    spawnTimer += deltaTime;

    if (!slowPU.active && !slowActive && spawnTimer >= spawnInterval) {
        generarSlowPU(slowPU, player.y);
        spawnTimer = 0.0f;
        spawnInterval = GetRandomValue(12, 16);
    }

    if (slowPU.active) {
        float dx = player.x - slowPU.x;
        float dy = player.y - slowPU.y;
        float dist2 = dx*dx + dy*dy;
        float sumR = slowPU.radius + player.width * 0.5f;

        if (dist2 < sumR * sumR) {
            slowActive   = true;
            slowTimeLeft = slowDuration;
            timeScale    = 0.4f;
            player.vy   -= 200.0f;
            slowPU.active = false;
            spawnTimer = 0.0f;
        }

        if (slowPU.y > player.y + GetScreenHeight()) {
            slowPU.active = false;
        }
    }
}

void generarDoublePU(PowerUp& doublePU, float playerY) {
    doublePU.active = true;
    doublePU.x = GetRandomValue(100, GetScreenWidth() - 100);
    doublePU.y = playerY - GetRandomValue(900, 1400);
}

void gestionarDoublePU(PowerUp& doublePU, Player& player, float deltaTime,
                       float& spawnTimer, float& spawnInterval,
                       bool& doubleActive, float& doubleTimeLeft,
                       float doubleDuration, float& scoreRateMult) {
    spawnTimer += deltaTime;

    if (!doublePU.active && !doubleActive && spawnTimer >= spawnInterval) {
        generarDoublePU(doublePU, player.y);
        spawnTimer = 0.0f;
        spawnInterval = GetRandomValue(14, 20);
    }

    if (doublePU.active) {
        float dx = player.x - doublePU.x;
        float dy = player.y - doublePU.y;
        float dist2 = dx*dx + dy*dy;
        float sumR = doublePU.radius + player.width * 0.5f;

        if (dist2 < sumR * sumR) {
            doubleActive   = true;
            doubleTimeLeft = doubleDuration;
            scoreRateMult  = 2.0f;
            doublePU.active = false;
            spawnTimer = 0.0f;
        }

        if (doublePU.y > player.y + GetScreenHeight()) {
            doublePU.active = false;
        }
    }
}
