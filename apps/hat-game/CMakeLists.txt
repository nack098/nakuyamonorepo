file(GLOB
    HAT_GAME
    "./*.c"
    "./*.h"
)

game_project(hat-game
    ${HAT_GAME}
    LINK_LIBRARIES
        nakuyadebugger
)
