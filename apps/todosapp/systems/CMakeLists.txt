file(GLOB
    TODOS_CORE
    "./*.c"
    "./*.h"
)

add_shared_api(todos-app-core
    ${TODOS_CORE}
    LINK_LIBRARIES
        nakuyadebugger
)
