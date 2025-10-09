cmake_minimum_required(VERSION 3.15)
project(game)

set(CMAKE_CXX_STANDARD 20)

# Platform-specific SFML hints (optional - CMake will search standard locations)
if(APPLE)
    # Check common Homebrew locations for SFML
    if(EXISTS "/opt/homebrew/opt/sfml@2")
        set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/sfml@2")
    elseif(EXISTS "/usr/local/opt/sfml@2")
        set(CMAKE_PREFIX_PATH "/usr/local/opt/sfml@2")
    endif()
endif()


set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
set(IMGUI_SFML_DIR ${CMAKE_SOURCE_DIR}/external/imgui-sfml-2.6.1)

file(GLOB IMGUI_SOURCES
        ${IMGUI_DIR}/*.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

file(GLOB IMGUI_SFML_SOURCES
        ${IMGUI_SFML_DIR}/*.cpp

)


find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

#add_subdirectory(external/imgui-sfml-2.6.1)
#include_directories(external/imgui)



add_executable(game src/main.cpp
        src/Character.cpp
        include/Character.h
        src/Tile.cpp
        include/Tile.h
        src/IsoCamera.cpp
        include/IsoCamera.h
        src/AssetManager.cpp
        include/AssetManager.h
        ${IMGUI_SOURCES}
        ${IMGUI_SFML_SOURCES}
        src/LayerManager.cpp
        include/LayerManager.h
        include/WorldSerializer.h
        src/Entity.cpp
        include/Entity.h
        src/Player.cpp
        include/Player.h
        src/EntityManager.cpp
        include/EntityManager.h
        src/World.cpp
        include/World.h
        src/collision/Collider.cpp
        src/collision/Collider.h
        src/Animation.cpp
        include/Animation.h
        src/Animator.cpp
        include/Animator.h
        src/editor/SpriteWindow.cpp
        src/editor/SpriteWindow.h
        src/editor/EditorContext.h
        src/events/EventRegion.cpp
        src/events/EventRegion.h
        src/events/EventManager.cpp
        src/events/EventManager.h
        include/util/math.h
        src/collision/CollisionManager.cpp
        src/collision/CollisionManager.h
        src/editor/EditorWindow.cpp
        src/editor/EditorWindow.h
        src/WorldSerializer.cpp
        include/Tree.h
        src/Tree.cpp)

#target_include_directories(game PRIVATE imgui imgui-sfml-2.6.1)

target_include_directories(game PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${IMGUI_SFML_DIR}
)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(game PRIVATE
        OpenGL::GL
        glfw
)

target_link_libraries(game PRIVATE sfml-graphics sfml-window sfml-system)

# Copy assets to build directory so the executable can find them
add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:game>/assets
    COMMENT "Copying assets to build directory"
)
