cmake_minimum_required(VERSION 3.20.0)

# Initialize Zephyr
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# Create project
project(zephyr_experiments LANGUAGES C)

# Include header
target_include_directories(app PRIVATE "src")

# Append every C file
file(GLOB SOURCES "src/main.c" "src/error/error.c" "src/hid/hid.c")

# Target source files
target_sources(app PRIVATE ${SOURCES})
