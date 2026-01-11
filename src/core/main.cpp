#include <StateMachine.hpp>
#include <MainMenuState.hpp>
#include <MainGameState.hpp>
#include <memory>
#include <chrono>
#include <ResourceManager.hpp>
#ifndef _WIN32
#include <libintl.h>
#endif
#include <locale.h>

extern "C" {
    #include <raylib.h>
}

const char* PACKAGE = "ProjectPrime8";
const char * LOCALEDIR = "./locale";

// Fallback gettext for Windows (no translations)
#ifdef _WIN32
static inline const char* gettext(const char* s) { return s; }
#endif

int main()
{ 
#ifndef _WIN32
    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(PACKAGE, "UTF-8");
    textdomain(PACKAGE);
#endif

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

    ResourceManager::instance().unloadAll();

    CloseWindow();

    return 0;
}