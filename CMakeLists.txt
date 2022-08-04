cmake_minimum_required(VERSION 3.22)

include(FetchContent)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
  CACHE STRING "Vcpkg toolchain file")

project(RayTracing VERSION 0.1 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

# Executable
qt_add_executable(RayTracing MANUAL_FINALIZATION)

# Sources
target_sources(RayTracing PRIVATE
    main.cc
    mainwindow.cc
    mainwindow.hh
    mainwindow.ui
)

add_subdirectory(RayTracing)
add_subdirectory(thirdparty)

target_include_directories(RayTracing PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Compilation options
target_compile_features(RayTracing PRIVATE cxx_std_20)
set_target_properties(RayTracing PROPERTIES CXX_EXTENSIONS OFF)
target_compile_options(RayTracing PRIVATE /W4)
## Enable address sanitizer in debug mode
target_compile_options(RayTracing PUBLIC $<$<CONFIG:Debug>:-fsanitize=address>)
target_link_options(RayTracing PUBLIC $<$<CONFIG:Debug>:-fsanitize=address>)
## Qt specific
set_target_properties(RayTracing PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Libraries
find_package(Eigen3)
target_link_libraries(RayTracing PRIVATE Eigen3::Eigen)

FetchContent_Declare(
    tracy
    GIT_REPOSITORY https://github.com/wolfpld/tracy.git
    GIT_TAG        v0.8.2.1
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(tracy)

option(TRACY_ENABLE "" ON)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)

target_link_libraries(RayTracing PRIVATE
    Tracy::TracyClient
    fmt::fmt spdlog::spdlog
    EnTT::EnTT
    EigenRand::EigenRand
)

target_link_libraries(RayTracing PRIVATE Qt6::Widgets)
qt_finalize_executable(RayTracing)
