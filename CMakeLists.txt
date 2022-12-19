cmake_minimum_required(VERSION 3.22)

include(FetchContent)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
  CACHE STRING "Vcpkg toolchain file")

project(RayTracingInAWeekend VERSION 0.1 LANGUAGES CXX)

option(BUILD_TESTS "Build the tests" ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

# Executable
qt_add_executable(RayTracingInAWeekend MANUAL_FINALIZATION)

# Sources
target_sources(RayTracingInAWeekend PRIVATE
    main.cc
    mainwindow.cc
    mainwindow.hh
    mainwindow.ui
)

add_subdirectory(RayTracing)
add_subdirectory(thirdparty)

target_include_directories(RayTracingInAWeekend PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Compilation options
target_compile_features(RayTracingInAWeekend PRIVATE cxx_std_20)
set_target_properties(RayTracingInAWeekend PROPERTIES CXX_EXTENSIONS OFF)
if(MSVC)
  target_compile_options(RayTracingInAWeekend PRIVATE /W4)
else()
  target_compile_options(RayTracingInAWeekend PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
## Enable address sanitizer in debug mode
#target_compile_options(RayTracingInAWeekend PUBLIC $<$<CONFIG:Debug>:-fstack-protector-all -fno-omit-frame-pointer>)
#target_link_options(RayTracingInAWeekend PUBLIC $<$<CONFIG:Debug>:-fstack-protector-all -fno-omit-frame-pointer>)
## Qt specific
set_target_properties(RayTracingInAWeekend PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Libraries
option(TRACY_ENABLE "" OFF) # Toggle as needed then reload cache
FetchContent_Declare(
    tracy
    GIT_REPOSITORY https://github.com/wolfpld/tracy.git
    GIT_TAG        v0.8.2.1
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(tracy)

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)

target_link_libraries(RayTracingInAWeekend PRIVATE
    RayTracing::RayTracing
    Tracy::TracyClient
    fmt::fmt spdlog::spdlog
    EnTT::EnTT
)

target_link_libraries(RayTracingInAWeekend PRIVATE Qt6::Widgets)
qt_finalize_executable(RayTracingInAWeekend)


# Testing only available if this is the main app
if(BUILD_TESTS)
  enable_testing()

  add_subdirectory(tests)
endif()
