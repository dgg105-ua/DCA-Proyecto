#include <StateMachine.hpp>
#include <MainMenuState.hpp>
#include <MainGameState.hpp>
#include <memory>
#include <chrono>

extern "C" {
    #include <raylib.h>
}

int main()
{ 
    float delta_time = 0.0f;

    StateMachine state_machine = StateMachine();
    //InitWindow(1280, 720, "DCA Practica Grupal");
    SetConfigFlags(FLAG_FULLSCREEN_MODE);  
    InitWindow(1920, 1080, "DCA Practica Grupal");
    SetTargetFPS(60);

    auto mainMenu = std::make_unique<MainMenuState>();
    mainMenu->setStateMachine(&state_machine);

    mainMenu->loadBackground();

    state_machine.add_state(std::move(mainMenu), false);
    state_machine.handle_state_changes(delta_time);

    while (!state_machine.is_game_ending() && !WindowShouldClose())
    {
        delta_time = GetFrameTime();

        state_machine.handle_state_changes(delta_time);
        state_machine.getCurrentState()->handleInput();
        state_machine.getCurrentState()->update(delta_time);
        state_machine.getCurrentState()->render();       
    }

    CloseWindow();

    return 0;
}