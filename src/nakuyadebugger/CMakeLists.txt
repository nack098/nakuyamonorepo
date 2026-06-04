file(GLOB
    NAKUYA_DEBUGGER
    "./*.c"
    "./*.h"
)

add_shared_api(nakuyadebugger ${NAKUYA_DEBUGGER})
