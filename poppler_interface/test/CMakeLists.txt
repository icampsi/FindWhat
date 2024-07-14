# =================================================== *
# ====         Copyright (c) 2024 icampsi        ==== *
# ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
# =================================================== */

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# Locate Catch2 (Assuming Catch2 is already installed or provided by vcpkg)
find_package(Catch2 REQUIRED)
include(Catch)

# Find Qt Widgets
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Source files for each test executable
set(TEST_SOURCES
    test_PdfToText.cpp
)

# Set the relative path for my test_pdf.pdf
set(PROJECT_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
add_definitions(-DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

# Create executables for testing and link with necessary libraries
foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} PRIVATE poppler_interface utils Catch2::Catch2WithMain Qt${QT_VERSION_MAJOR}::Widgets)
    catch_discover_tests(${test_name} TEST_PREFIX "poppler_interface.")
endforeach()

# Enable CTest for running tests via `make test`
include(CTest)
