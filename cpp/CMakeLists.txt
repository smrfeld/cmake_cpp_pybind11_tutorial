cmake_minimum_required(VERSION 3.0.0)
project(car VERSION 0.1.0)

# Include dir
include_directories(/usr/local/include)

# Src
AUX_SOURCE_DIRECTORY(src SRC_FILES)

# Headers
set(PROJECT_SOURCE_DIR "src")
set(PROJECT_INCLUDE_DIR "include/car_bits")

# Source files
set(SOURCE_FILES
    ${PROJECT_INCLUDE_DIR}/car.hpp
    ${PROJECT_SOURCE_DIR}/car.cpp
)

# Set up such that XCode organizes the files correctly
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

# Add library
add_library(car SHARED ${SOURCE_FILES})

# Include directories
target_include_directories(car PRIVATE include/car_bits)

# Install
install(TARGETS car DESTINATION lib)

# Install the headers
install(FILES include/car DESTINATION include)

# Create base directory
install(DIRECTORY include/car_bits DESTINATION include)