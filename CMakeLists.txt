##################################################
#
#      Define project global configuration.
#
##################################################

 cmake_minimum_required(VERSION 3.12)               # Minimum CMake version required.
      project(OliverLang VERSION 0.1)               # Name and version of the project.
           set(CMAKE_CXX_STANDARD 20)               # Set the minimum required C++
set(CMAKE_CXX_STANDARD_REQUIRED True)               # standard to use.

# Set the location for static and shared code output.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

##################################################
#
#              Include sub projects.
#
##################################################
add_subdirectory ("Main_Function")                   # Define the main fucntion.