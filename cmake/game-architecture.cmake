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
    set(mode "SOURCES")
    set(lib_sources "")
    set(lib_deps "")

    foreach(arg ${ARGN})
        if(arg STREQUAL "LINK_LIBRARIES")
            set(mode "DEPS")
        elseif(mode STREQUAL "SOURCES")
            list(APPEND lib_sources "${arg}")
        else()
            list(APPEND lib_deps "${arg}")
        endif()
    endforeach()

    add_library(${NAME} SHARED ${lib_sources})
    
    target_compile_definitions(${NAME} PRIVATE 
        BUILD_LIBTYPE_SHARED
        RAYLIB_SHARED
    )
    
    target_link_libraries(${NAME} 
        PRIVATE 
            raylib
            ${lib_deps}
    )
    
    target_include_directories(${NAME} PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
    )
endfunction()

function(game_project NAME)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${NAME}" PARENT_SCOPE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${NAME}")

    set(mode "SOURCES")
    set(game_sources "")
    set(game_deps "")

    foreach(arg ${ARGN})
        if(arg STREQUAL "LINK_LIBRARIES")
            set(mode "DEPS")
        elseif(mode STREQUAL "SOURCES")
            list(APPEND game_sources "${arg}")
        else()
            list(APPEND game_deps "${arg}")
        endif()
    endforeach()

    add_executable(${NAME} ${game_sources})
    target_compile_definitions(${NAME} PRIVATE RAYLIB_SHARED)
    target_link_libraries(${NAME}
        PRIVATE
            raylib
            ${game_deps}
    )

    add_custom_command(
        TARGET ${NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy -t "$<TARGET_FILE_DIR:${NAME}>" 
                "$<TARGET_RUNTIME_DLLS:${NAME}>"
        COMMAND_EXPAND_LISTS
        COMMENT "Automatically detecting and deploying runtime dependencies for ${NAME}..."
    )

    target_copy_assets(${NAME})
endfunction()
