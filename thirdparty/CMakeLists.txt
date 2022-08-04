cmake_minimum_required(VERSION 3.22)

# EigenRand
add_library(EigenRand INTERFACE)
add_library(EigenRand::EigenRand ALIAS EigenRand)

target_include_directories(RayTracing PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/EigenRand
)
