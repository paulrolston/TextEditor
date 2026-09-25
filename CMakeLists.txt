cmake_minimum_required(VERSION 3.20)
project(text C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")


set(SDLTTF_VENDORED ON)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SDL_SHARED OFF CACHE BOOL "Build shared library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build static library" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(text src/xxhash.c src/main.c src/editor.c src/text_window.c src/toolbar.c src/button.c src/toast.c src/toastmanager.c)

target_link_libraries(text PRIVATE SDL3_ttf::SDL3_ttf-static SDL3::SDL3-static)
