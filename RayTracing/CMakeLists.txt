cmake_minimum_required(VERSION 3.22)

target_sources(RayTracing PRIVATE
    Ray.hh Ray.cc
    HitRecord.hh HitRecord.cc
    Camera.hh Camera.cc
    Components.hh Components.cc
    Random.hh
)
