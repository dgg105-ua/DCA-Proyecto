add_executable(ProjectPrime8_gameplay_tests
  gameplay_runner.cpp
  raylib_stubs.cpp
  ${CMAKE_SOURCE_DIR}/src/core/GameLogic.cpp
)

target_compile_features(ProjectPrime8_gameplay_tests PRIVATE cxx_std_17)

target_include_directories(ProjectPrime8_gameplay_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/vendor/include
)

if (UNIX AND NOT APPLE)
  target_link_libraries(ProjectPrime8_gameplay_tests PRIVATE m)
endif()

add_test(NAME gameplay_collision_ground               COMMAND ProjectPrime8_gameplay_tests --run collision_ground)
add_test(NAME gameplay_collision_wall_push            COMMAND ProjectPrime8_gameplay_tests --run collision_wall_push)
add_test(NAME gameplay_gravity_moves_down             COMMAND ProjectPrime8_gameplay_tests --run gravity_moves_down)
add_test(NAME gameplay_jump_buffer_triggers           COMMAND ProjectPrime8_gameplay_tests --run jump_buffer_triggers)
add_test(NAME gameplay_jump_cut_when_space_held       COMMAND ProjectPrime8_gameplay_tests --run jump_cut_when_space_held)
add_test(NAME gameplay_powerup_spawn_by_timer         COMMAND ProjectPrime8_gameplay_tests --run powerup_spawn_by_timer)
add_test(NAME gameplay_powerup_pickup_jump            COMMAND ProjectPrime8_gameplay_tests --run powerup_pickup_jump)
add_test(NAME gameplay_shield_pickup_sets_flag        COMMAND ProjectPrime8_gameplay_tests --run shield_pickup_sets_flag)
add_test(NAME gameplay_slow_pickup_sets_timescale     COMMAND ProjectPrime8_gameplay_tests --run slow_pickup_sets_timescale)
add_test(NAME gameplay_double_pickup_sets_multiplier  COMMAND ProjectPrime8_gameplay_tests --run double_pickup_sets_multiplier)
