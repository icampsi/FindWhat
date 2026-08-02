# =================================================== *
# ====         Copyright (c) 2024 icampsi        ==== *
# ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
# =================================================== */

cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

find_package(Catch2 REQUIRED)
include(Catch)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(TEST_SOURCES
    test_PdfToText.cpp
)

set(PROJECT_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
add_definitions(-DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} PRIVATE poppler_interface utils Catch2::Catch2WithMain Qt${QT_VERSION_MAJOR}::Widgets)
    catch_discover_tests(${test_name} TEST_PREFIX "poppler_interface.")
endforeach()

include(CTest)
