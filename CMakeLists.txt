cmake_minimum_required(VERSION 3.20)

include_guard()

project(CoreInject)

add_library(CoreInject STATIC Source/CoreInject.cpp Source/ElfFile.cpp Source/Process.cpp Source/GDBProcess.cpp)
target_compile_features(CoreInject PUBLIC cxx_std_23)
target_include_directories(CoreInject PUBLIC "Include")

add_subdirectory("Settings")
target_link_libraries(CoreInject CoreInjectSettings)

if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory("CLI")
endif()