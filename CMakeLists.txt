cmake_minimum_required(VERSION 3.0.0)
project(JiuJitsuFight)

set(CMAKE_CXX_STANDARD 17)

# Declarar CADMIUM explícitamente dentro de CMake
set(CADMIUM "/home/vayrhon/Desktop/Cadmium_Projects/cadmium_v2/include")

include_directories(${CADMIUM})
include_directories(main/include)

add_executable(JiuJitsuFight 
    main/main.cpp
)