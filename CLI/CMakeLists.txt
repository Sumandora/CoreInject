project(CoreCLI)

add_executable(CoreCLI Source/CoreCLI.cpp)
target_include_directories(CoreCLI PUBLIC "Include")

add_subdirectory("../NeedsRoot" "NeedsRoot")

include("FetchContent")

set(ARGPARSE_INSTALL OFF CACHE BOOL "" FORCE)
set(ARGPARSE_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ARGPARSE_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
        GIT_PROGRESS TRUE
        GIT_TAG v3.0)
FetchContent_MakeAvailable(argparse)

target_link_libraries(CoreCLI CoreInject CoreInjectNeedsRoot argparse)

install(TARGETS CoreCLI)
