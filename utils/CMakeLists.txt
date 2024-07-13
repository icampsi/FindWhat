# =================================================== *
# ====         Copyright (c) 2024 icampsi        ==== *
# ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
# =================================================== */

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(utils VERSION 0.1.0 LANGUAGES CXX)

# Warning level
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-nonportable-include-path -Wno-microsoft-include)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt Widgets
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Add the utils library
add_library(${PROJECT_NAME} STATIC
    USerialize.cpp
    USystem.cpp
    UText.h         UText.cpp
    CPagedText.h    CPagedText.cpp
)

# Set the output directory for the library (during build time)
set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Add current source dir as an include path
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>  # Specify where to install headers
)

# Link Qt Widgets to the utils library
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Enable testing and add the test directory
include(CTest)
enable_testing()

add_subdirectory(test)

