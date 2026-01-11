#include <GameLogic.hpp>
#include <GameOverState.hpp>
#include <memory>

void gestionarLava(float deltaTime, Lava& lava, Player& player,
                   StateMachine* state_machine, float& puntuacion,
                   std::deque<Estructura>& estructuras) {
    (void)estructuras;

    lava.rect.y -= lava.vy * deltaTime;

    if (CheckCollisionRecs(player.boundingBox, lava.rect)) {
        auto gameOver = std::make_unique<GameOverState>();
        gameOver->setStateMachine(state_machine);
        gameOver->setPuntuacion(puntuacion);
        state_machine->add_state(std::move(gameOver), true);
    }
}
