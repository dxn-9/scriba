cmake_minimum_required(VERSION 3.11)

project(scribaC)

# Find SDL3 and SDL3_ttf packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3)
pkg_check_modules(SDL3_TTF REQUIRED sdl3-ttf)

# Set source files
set(SOURCES
    main.c
    utils.c
    vector.c
    application.c
    cursor.c
    text.c
    bottom_bar.c
    action.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories for SDL3 and SDL3_TTF
target_include_directories(${PROJECT_NAME} PRIVATE
    ${SDL3_INCLUDE_DIRS}
    ${SDL3_TTF_INCLUDE_DIRS}
)

# Link directories for SDL3 and SDL3_TTF
target_link_directories(${PROJECT_NAME} PRIVATE
    ${SDL3_LIBRARY_DIRS}
    ${SDL3_TTF_LIBRARY_DIRS}
)

# Link the SDL3 and SDL3_TTF libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${SDL3_LIBRARIES}
    ${SDL3_TTF_LIBRARIES}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -g
    -Wall
)