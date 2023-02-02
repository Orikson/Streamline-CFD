#******************************************************************************
# 
# Streamline CFD, a cross-platform fluid engine built using OpenCL
#
# Copyright 2023 Eron Ristich
#
# This program is free software, distributed under the terms of the
# MIT License
# https://opensource.org/licenses/MIT
#
#******************************************************************************

cmake_minimum_required(VERSION 3.16)

project("Streamline-CFD" VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/Zc:rvalueCast /Zc:inline /Zc:strictStrings /Zc:throwingNew /permissive- /Zc:__cplusplus /Zc:externConstexpr /utf-8 /w34100 /w34189 /w44996 /w44456 /w44457 /w44458)
endif()

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenCL REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets Gui OpenGLWidgets)
qt_standard_project_setup()

add_library(defines STATIC "defines.h")
target_include_directories(defines INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(src/opencl)
add_subdirectory(src/streamline)

qt_add_resources(streamline resources
    PREFIX "/resources"
    FILES resources/pause.png resources/play.png resources/stop.png
)