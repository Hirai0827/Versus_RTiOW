cmake_minimum_required(VERSION 3.17)
project(RayTracing)

set(CMAKE_CXX_STANDARD 14)
FIND_PACKAGE( OpenMP REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
add_executable(RayTracing rayt.cpp rayt.h vec3.h ray.h hittable.h sphere.h hittable_list.h math_utils.h camera.h material.h moving_sphere.h aabb.h bvh.h texture.h aarect.h box.h)