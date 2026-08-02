# =================================================== *
# ====         Copyright (c) 2024 icampsi        ==== *
# ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
# =================================================== */

cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
project(FindWhat VERSION 0.2.0 LANGUAGES CXX)

# Warning level
if(MSVC)
    add_compile_options(/W4 /WX)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
        -Wall
        -Wextra
        -pedantic
        -Werror
        -Wno-nonportable-include-path
        -Wno-microsoft-include
    )
else()
    add_compile_options(
        -Wall
        -Wextra
        -pedantic
        -Werror
    )
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")

# Find QT Widgets
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

option(ENABLE_DBMANAGER "Enable dbManager module" ON)

# TESTING
include(CTest)

if(BUILD_TESTING)
    include(FetchContent)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.8.1
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(Catch2)
endif()

# SUBDIRECTORIES
add_subdirectory(utils)

if(NOT TARGET finder)
    add_subdirectory(finder)
endif()

add_subdirectory(poppler_interface)

if(Qt${QT_VERSION_MAJOR}_FOUND)
    message("Found Qt library: ${QT_VERSION_MAJOR}")
else()
    message(FATAL_ERROR "Qt Widgets not found")
endif()

set(MAIN_FILES
    # SRC
    src/main.cpp

    # DOCUMENT
    document/CMDoc.cpp              document/CMDoc.h
    document/CDocument.cpp          document/CDocument.h
    document/CEsquemaDoc.cpp        document/CEsquemaDoc.h
    document/CExportPathDoc.cpp     document/CExportPathDoc.h
    document/CExportCSV.cpp         document/CExportCSV.h
    document/CPdfDoc.cpp            document/CPdfDoc.h

    # UI
    ui/MainWindow.cpp               ui/MainWindow.h               ui/MainWindow.ui
    ui/QCItemDelegateFormulaTreeView.cpp

    ui/PDockPreview.cpp             ui/PDockPreview.h             ui/PDockPreview.ui
    ui/PEsquemaPage.cpp             ui/PEsquemaPage.h             ui/PEsquemaPage.ui
    ui/PFormExpToolBoxPage.cpp      ui/PFormExpToolBoxPage.h      ui/PFormExpToolBoxPage.ui
    ui/PMainEsquemaUI.cpp           ui/PMainEsquemaUI.h           ui/PMainEsquemaUI.ui
    ui/PTabTextPreview.cpp          ui/PTabTextPreview.h
    ui/WTextEdDynamicBlk.cpp        ui/WTextEdDynamicBlk.h

    ui/WBrowserTreeView.cpp         ui/WBrowserTreeView.h
    ui/WEsquemaListView.cpp         ui/WEsquemaListView.h
    ui/WFormulaTreeView.cpp         ui/WFormulaTreeView.h

    ui/WLoadedFilesTreeView.cpp     ui/WLoadedFilesTreeView.h
    ui/WToolBarPreview.cpp          ui/WToolBarPreview.h
    ui/WToolBarEsquema.cpp          ui/WToolBarEsquema.h
    ui/WExtendedListWidget.cpp      ui/WExtendedListWidget.h
    ui/WSpreadSheet.cpp             ui/WSpreadSheet.h

    ui/dialogs/NewEsquema_dlg.cpp
    ui/dialogs/NewEsquema_dlg.h
    ui/dialogs/NewEsquema_dlg.ui

    ui/dialogs/exportEsquema_dlg.cpp
    ui/dialogs/exportEsquema_dlg.h
    ui/dialogs/exportEsquema_dlg.ui

    ui/dialogs/ProgBar_dlg.cpp
    ui/dialogs/ProgBar_dlg.h
    ui/dialogs/ProgBar_dlg.ui

    ui/dialogs/InvalidFileName_dlg.cpp
    ui/dialogs/InvalidFileName_dlg.h
    ui/dialogs/InvalidFileName_dlg.ui

    ui/dialogs/DDisplayParsedInfo.cpp
    ui/dialogs/DDisplayParsedInfo.h
    ui/dialogs/DDisplayParsedInfo.ui

    ui/WExtViews/WTreeView.h
    ui/WExtViews/WTreeWidget.h
    ui/WExtViews/WListView.h
    ui/WExtViews/WListWidget.h
    ui/WExtViews/WTableView.h
    ui/WExtViews/WTableWidget.h
    ui/WExtViews/WColumnView.h

    # RESOURCE
    res/icons.qrc
)

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${MAIN_FILES}

        ui/WEsqView.h
        ui/WEsqView.cpp

        ui/ExtTabWidget.h
        ui/ExtTabWidget.cpp

        CParsedFile.h
        CParsedFile.cpp

        ui/CHorizontalProxyModel.h
        ui/CHorizontalProxyModel.cpp

        CParsedFileModel.h
        CParsedFileModel.cpp

        CParsedPdfModel.h
        CParsedPdfModel.cpp
    )
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED
            ${MAIN_FILES}
        )
    else()
        add_executable(${PROJECT_NAME}
            ${MAIN_FILES}
        )
    endif()
endif()

if(ENABLE_DBMANAGER)
    add_subdirectory(dbManager)
    target_link_libraries(${PROJECT_NAME} PRIVATE dbManager)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_DBMANAGER)
endif()

# Set the output directory for the executable
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Add current source dir as a include path
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/ui
        ${CMAKE_CURRENT_SOURCE_DIR}/ui/dialogs
        ${CMAKE_CURRENT_SOURCE_DIR}/ui/WExtViews
        ${CMAKE_CURRENT_SOURCE_DIR}/document
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

# LINK LIBRARIES
# QT
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        utils
        finder
        poppler_interface
)

if(QT_VERSION VERSION_LESS 6.1.0)
    set(
        BUNDLE_ID_OPTION
        MACOSX_BUNDLE_GUI_IDENTIFIER com.example.FindWhat
    )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING
        ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
            QT_NO_DEBUG_OUTPUT
    )
endif()

include(GNUInstallDirs)

install(
    TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
