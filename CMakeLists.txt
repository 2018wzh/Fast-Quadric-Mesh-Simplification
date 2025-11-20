cmake_minimum_required(VERSION 3.10)
project(FastQuadricMeshSimplification)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# vcpkg Dependencies
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(DevIL REQUIRED)

# Include directories
include_directories(
    src/core
    src/gui
    src/include
)

# Add the Simplify.CMD project
add_executable(Simplify_CMD
    src/core/Main.cpp
)

# Add the Simplify.GL project
add_executable(Simplify_GL
    src/gui/Main.cpp
    src/gui/Bmp.cpp
    src/gui/Core.cpp
    src/gui/VecMath.cpp
)

# Link libraries
target_link_libraries(Simplify_GL PRIVATE GLUT::GLUT GLEW::GLEW DevIL::ILU DevIL::ILUT)