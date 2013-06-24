cmake_minimum_required(VERSION 2.8)

project(Inpaint)

################################################################################
# Build main.cpp
################################################################################

# Create a list of all the ".cpp" files and save it to |sources|.
file(GLOB sources RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.cpp")

# Print a message containing the list.
message(STATUS "sources = ${sources}")

# Tell CMake to build main.
add_executable(inpaint_main ${sources})

# Find the OpenCV libraries.
find_package(OpenCV)

# Link the executable against them.
target_link_libraries(inpaint_main ${OpenCV_LIBS})

