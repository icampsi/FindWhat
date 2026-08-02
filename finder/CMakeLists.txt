# =================================================== *
# ====         Copyright (c) 2024 icampsi        ==== *
# ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
# =================================================== */

cmake_minimum_required(VERSION 4.0 FATAL_ERROR)
project(finder VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compile options
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-nonportable-include-path -Wno-microsoft-include)
endif()

# Find Qt Widgets
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Add the Finder library
add_library(${PROJECT_NAME} STATIC
    CData.cpp               CData.h
    CEsquema.cpp            CEsquema.h
    CFormula.cpp            CFormula.h
    CFunctionClasses.cpp    CFunctionClasses.h
)

# Set the output directory for the library
set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Add current source dir and utils dir as include paths
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../
)

# Link Qt Widgets
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Add utils as a subdirectory and link against it
if(NOT TARGET utils)
    add_subdirectory(../utils ../utils)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC utils)

# Enable testing and add the test directory
include(CTest)
enable_testing()
add_subdirectory(test)
