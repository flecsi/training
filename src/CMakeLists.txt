cmake_minimum_required(VERSION 3.16)

project(heat2d LANGUAGES CXX C)

# ---- Option to build "version 4" (no physics/io, no link) ----
option(VERSION4 "Build VERSION4: compile sources except physics.cc/io.cc, no executable" OFF)

# ---- Code formatting (clang-format) ----
include("${CMAKE_SOURCE_DIR}/format.cmake")

# ---- C++ standard ----
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Headers ----
add_library(heatcore INTERFACE)
target_include_directories(heatcore INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ---- Sources ----
set(HEAT2D_SOURCES
    main.cc
    initialize.cc
    advance.cc
    analyze.cc
    finalize.cc
    io.cc
    physics.cc
)

# ---- VERSION4: only compile (no physics.cc/io.cc, no link) ----
if (VERSION4)

    # Build them as an object library so they are compiled but not linked
    add_library(heat2d_objs OBJECT main.cc)
    target_link_libraries(heat2d_objs PRIVATE heatcore)

else()
    # ---- Executable (normal build) ----
    add_executable(heat2d ${HEAT2D_SOURCES})
    target_link_libraries(heat2d PRIVATE heatcore)

endif()

# ---- Build type default (Release if not specified) ----
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
