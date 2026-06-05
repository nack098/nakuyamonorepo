file(GLOB
    GAME_CORE
    "./*.c"
    "./*.h"
)

add_shared_api(app
    ${GAME_CORE}
    LINK_LIBRARIES
        nakuyadebugger
)
