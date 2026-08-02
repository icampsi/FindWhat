# =================================================== *
# ====         Copyright (c) 2024 icampsi        ==== *
# ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
# =================================================== */

cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
set(MODULE_NAME poppler_interface)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt Widgets
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_library(${MODULE_NAME} STATIC
    PdfToText.h    PdfToText.cpp
)

target_link_libraries(${MODULE_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# POPPLER
find_library(POPPLER_CPP_LIB NAMES poppler-cpp)
if (POPPLER_CPP_LIB)
    message("Found Poppler library: ${POPPLER_CPP_LIB}")
    target_link_libraries(${MODULE_NAME} PRIVATE
        ${POPPLER_CPP_LIB}
    )
    # Poppler includes
    find_path(POPPLER_CPP_INCLUDE_DIR poppler/cpp)
    if (POPPLER_CPP_INCLUDE_DIR)
        message("Found Poppler include directory: ${POPPLER_CPP_INCLUDE_DIR}")
        target_include_directories(${MODULE_NAME} PRIVATE ${POPPLER_CPP_INCLUDE_DIR})
    else ()
        message(FATAL_ERROR "Poppler include directory not found")
    endif ()
else ()
    message(FATAL_ERROR "Poppler library not found. You can download and install it from https://gitlab.freedesktop.org/poppler/poppler")
endif ()

# Add current source dir and utils dir as include paths
target_include_directories(${MODULE_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../
)

# Enable testing and add the test directory
include(CTest)
enable_testing()

if(BUILD_TESTING)
    add_subdirectory(test)
endif()