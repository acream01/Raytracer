cmake_minimum_required ( VERSION 3.15)
project(Raytracer)


#Collect all cpp and header files
set(SOURCES 
"src/main.cpp"
"src/color.h" 
"src/vec3.h"
"src/ray.h"
"src/hittable.h"
"src/sphere.h"
"src/hittable_list.h"
"src/rtweekend.h"
"src/camera.h" 
"src/interval.h"
"src/material.h"
)


# Add the executable target
add_executable(${PROJECT_NAME} ${SOURCES}   )

target_include_directories(${PROJECT_NAME} PRIVATE third_party)

#C++ version 11 
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)