# =================================================== *
# ====         Copyright (c) 2024 icampsi        ==== *
# ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
# =================================================== */

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(dbManager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Core)

add_library(${PROJECT_NAME} STATIC
    WDbView.h               WDbView.cpp             WDbView.ui
    WSearcherHeader.h       WSearcherHeader.cpp
    DMemberEdit.h           DMemberEdit.cpp
    CDbConnection.h         CDbConnection.cpp
    WSqlMultiTable.h        WSqlMultiTable.cpp
    CSqlMultiTableDelegate.h   CSqlMultiTableDelegate.cpp
    CSqlMultiTableModel.h   CSqlMultiTableModel.cpp

)

target_link_libraries(${PROJECT_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Core)

# Add utils as a subdirectory and link against it
if(NOT TARGET utils)
    add_subdirectory(../utils ../utils)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC utils)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../
)

