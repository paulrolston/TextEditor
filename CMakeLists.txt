cmake_minimum_required(VERSION 3.15)
project(text C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(SDLTTF_VENDORED ON)

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(text src/main.c src/editor.c src/text_window.c src/toolbar.c src/button.c src/toast.c)

target_link_libraries(text PRIVATE SDL3_ttf::SDL3_ttf SDL3::SDL3)
