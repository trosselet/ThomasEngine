project(Gameplay LANGUAGES CXX)


file(GLOB_RECURSE GAMEPLAY_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Header/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Header/*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.inl
)

add_executable(Gameplay ${GAMEPLAY_SOURCES})

target_include_directories(Gameplay PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
)

target_link_libraries(Gameplay PRIVATE Engine Tools)



# Fonction simple pour générer une cible de copie automatique
function(add_auto_copy NAME SOURCE_DIR DEST_DIR)
    file(GLOB_RECURSE FILES_TO_WATCH "${SOURCE_DIR}/*")

    add_custom_target(${NAME}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_DIR} ${DEST_DIR}
        DEPENDS ${FILES_TO_WATCH}
        COMMENT "Copying: ${SOURCE_DIR} -> ${DEST_DIR}"
    )

    add_dependencies(Gameplay ${NAME})
endfunction()

# Dossiers sources
set(SHADERS_SRC   "${CMAKE_SOURCE_DIR}/res/shaders")
set(ASSETS_SRC    "${CMAKE_SOURCE_DIR}/res/obj")
set(TEXTURES_SRC  "${CMAKE_SOURCE_DIR}/res/textures")

set(OUTPUT_DIR "$<TARGET_FILE_DIR:Gameplay>")

add_auto_copy(copy_shaders   ${SHADERS_SRC}   "${OUTPUT_DIR}/shaders")
add_auto_copy(copy_assets    ${ASSETS_SRC}    "${OUTPUT_DIR}/assets")
add_auto_copy(copy_textures  ${TEXTURES_SRC}  "${OUTPUT_DIR}/textures")
