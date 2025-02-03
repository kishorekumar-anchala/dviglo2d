cmake_minimum_required(VERSION 3.10)
project(dviglo2d)

# Set CMake folder for IDE targets
set(CMAKE_FOLDER "библиотеки")

# Find SDL2
find_package(SDL2 REQUIRED)

# Include directories and libraries for SDL2
include_directories(${SDL2_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARIES})

# Add all subdirectories (exclude from all ensures libraries are built only if needed)
dv_add_all_subdirs(EXCLUDE_FROM_ALL)

# Link SDL2 to your application
add_executable(app src/app.cpp)
target_link_libraries(app PRIVATE ${SDL2_LIBRARIES})

# Any other necessary flags or paths
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
