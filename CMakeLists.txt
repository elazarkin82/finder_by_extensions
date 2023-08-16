cmake_minimum_required(VERSION 3.10)
project(find_files)

set(CMAKE_CXX_STANDARD 11)

# Find the Python libraries and include directories
find_package(Python3 REQUIRED COMPONENTS Development)

# Add the include directories for Python
include_directories(${Python3_INCLUDE_DIRS})

# Add the find_files library target
add_library(findfiles SHARED cpp/find_files.cpp)

# Link the find_files library with the Python libraries
target_link_libraries(findfiles ${Python3_LIBRARIES})

# Add the executable target for the main function
add_executable(find_files_tester cpp/find_files.cpp)
