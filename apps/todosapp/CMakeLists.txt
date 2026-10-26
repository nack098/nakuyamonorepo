add_subdirectory(systems)
file(GLOB
    TODOS_APP
    "./*.c"
    "./*.h"
)

game_project(todos-app
    ${TODOS_APP}
    LINK_LIBRARIES
        todos-app-core
        nakuyadebugger
)

set_target_properties(todos-app-core PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)
