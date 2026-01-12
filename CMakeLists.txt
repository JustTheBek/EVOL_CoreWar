cmake_minimum_required(VERSION 3.10)
project(corewar_ga)

# Use C++17 for your CoreWar project
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include project headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Include GALib headers (parent dir of ga/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/galib247)

# Path to prebuilt GALib static library
set(GALIB_LIB "${CMAKE_CURRENT_SOURCE_DIR}/galib247/ga/libga.a")

# Ensure the library exists before linking
if(NOT EXISTS "${GALIB_LIB}")
    message(FATAL_ERROR "GALib library not found at ${GALIB_LIB}. Build it first using 'make' in galib247/ga")
endif()

# Project source files
add_executable(corewar_ga
    src/main.cpp
    src/CoreWarEvaluator.cpp
    src/WarriorEncoder.cpp
)

# MAGIC: This is a tricky flag. libga.a was built with C++98, because it's an old stuff.
# But I build CoreWar with C++17 to have some modern features. Down below I will link the
# (C++98 compiled) libga binary to the (C++17 compiled) corewar_ga target. This should be
# no problem. However, corewar_ga needs the headers of libga, which I defined above. But
# unfortunately the developers of libga made some dirty stuff and they included some .c
# files in many of their interface .h files. And sadly these .c files contain a feature
# which is not supported in C++17 anymore, and therefore my corewar_ga doesn't compile.
# This feature is the 'register' keyword, therefore I define it here as an empty compiler
# directive to compensate this nasty stuff the libga developers made in their days. :P
target_compile_definitions(corewar_ga PRIVATE register=)


# Link GALib (C++98-built) to your C++17 project
target_link_libraries(corewar_ga "${GALIB_LIB}")

# Optional: Show include paths during build for debugging
# set(CMAKE_VERBOSE_MAKEFILE ON)
