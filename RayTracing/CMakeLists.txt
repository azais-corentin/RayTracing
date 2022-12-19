cmake_minimum_required(VERSION 3.22)

add_library(RayTracing)
add_library(RayTracing::RayTracing ALIAS RayTracing)

add_subdirectory(includes)

target_include_directories(RayTracing PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/includes)

target_compile_features(RayTracing PRIVATE cxx_std_20)
set_target_properties(RayTracing PROPERTIES CXX_EXTENSIONS OFF)
if(MSVC)
  target_compile_options(RayTracing PRIVATE /W4)
else()
  target_compile_options(RayTracing PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_sources(RayTracing PRIVATE
    Ray.cc
    HitRecord.cc
    Camera.cc
    Components.cc
)

find_package(Eigen3)

target_link_libraries(RayTracing PUBLIC
    Eigen3::Eigen
    EigenRand::EigenRand
)
