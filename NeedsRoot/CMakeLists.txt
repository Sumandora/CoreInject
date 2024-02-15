project(CoreInjectNeedsRoot)

add_library(CoreInjectNeedsRoot STATIC Source/NeedsRoot.cpp)
target_include_directories(CoreInjectNeedsRoot PUBLIC "Include")
