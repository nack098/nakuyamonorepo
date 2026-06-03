function(target_copy_assets TARGET_NAME)
    get_filename_component(ASSETS_ABS_PATH "${PROJECT_SOURCE_DIR}/assets/${TARGET_NAME}" ABSOLUTE)
    set(DEST_DIR "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets")
    add_custom_command(
        TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${ASSETS_ABS_PATH}"
                "${DEST_DIR}"
        COMMENT "Copying assets for ${TARGET_NAME} next to its executable..."
        VERBATIM
    )
endfunction()

function(add_shared_api NAME)
    add_library(${NAME} SHARED ${ARGN})
    target_link_libraries(${NAME} PRIVATE raylib)
    target_include_directories(${NAME} PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
    )
endfunction()

function(game_project NAME SOURCES)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${NAME}" PARENT_SCOPE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${NAME}")
    add_executable(${NAME}
        ${SOURCES}
    )
    target_link_libraries(
        ${NAME}
        PRIVATE
            raylib
    )
    target_copy_assets(${NAME})
endfunction()
