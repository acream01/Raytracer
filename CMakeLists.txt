cmake_minimum_required ( VERSION 3.15)
project(Raytracer)


#Collect all cpp and header files
set(SOURCES 
    src/main.cpp
)

# Add the executable target
add_executable(${PROJECT_NAME} ${SOURCES})

#C++ version 11 
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)