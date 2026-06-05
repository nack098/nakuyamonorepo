add_subdirectory(game)
file(GLOB
    HAT_GAME
    "./*.c"
    "./*.h"
)

game_project(hat-game
    ${HAT_GAME}
    LINK_LIBRARIES
        app
        nakuyadebugger
)

set_target_properties(app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)
