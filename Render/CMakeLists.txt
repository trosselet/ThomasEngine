project(Render LANGUAGES CXX)

file(GLOB_RECURSE RENDER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Header/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Header/*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.inl

)

add_library(Render STATIC ${RENDER_SOURCES})

target_include_directories(Render PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
)

target_link_libraries(Render PUBLIC Tools)

# DirectX12
target_link_libraries(Render PUBLIC d3d12 dxgi dxguid)
