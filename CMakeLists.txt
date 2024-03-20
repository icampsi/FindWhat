cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(FindWhat VERSION 0.1.0 LANGUAGES CXX)

# Enable all warnings and treat them as errors
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-nonportable-include-path -Wno-microsoft-include)
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
if(Qt${QT_VERSION_MAJOR}_FOUND)
    message("Found Poppler library: ${POPPLER_CPP_LIB}")
else()
    message(FATAL_ERROR "Qt Widgets not found")
endif()

# FILES TO COMPILE (Organized following the folder structure of the project)
set(SRC_FILES
    src/main.cpp

    src/CData.cpp               src/CData.h
    src/CEsquema.cpp            src/CEsquema.h
    src/CFormula.cpp            src/CFormula.h
    src/CFunctionClasses.cpp    src/CFunctionClasses.h
)

set(DOCUMENT_FILES
    document/CMDoc.cpp                  document/CMDoc.h
    document/CDocument.cpp              document/CDocument.h
    document/CEsquemaDoc.cpp            document/CEsquemaDoc.h
    document/CExportPathDoc.cpp         document/CExportPathDoc.h
    document/CExportCSV.cpp             document/CExportCSV.h
    document/CPdfDoc.cpp                document/CPdfDoc.h
)

set(UTILS_FILES
    utils/GeneralFunctions.cpp  utils/GeneralFunctions.h
)

set(UI_FILES
    ui/MainWindow.cpp         ui/MainWindow.h           ui/MainWindow.ui
    ui/QCItemDelegateFormulaTreeView.cpp

    ui/PDockPreview.cpp        ui/PDockPreview.h        ui/PDockPreview.ui
    ui/PEsquemaPage.cpp        ui/PEsquemaPage.h        ui/PEsquemaPage.ui
    ui/PFormExpToolBoxPage.cpp ui/PFormExpToolBoxPage.h ui/PFormExpToolBoxPage.ui
    ui/PMainEsquemaUI.cpp      ui/PMainEsquemaUI.h      ui/PMainEsquemaUI.ui
    ui/PTabTextPreview.cpp     ui/PTabTextPreview.h     ui/PTabTextPreview.ui


    ui/WBrowserTreeView.cpp     ui/WBrowserTreeView.h
    ui/WEsquemaListView.cpp     ui/WEsquemaListView.h
    ui/WEsquemaTreeView.cpp     ui/WEsquemaTreeView.h
    ui/WLoadedEsquemes.cpp      ui/WLoadedEsquemes.h
    ui/WLoadedFilesTreeView.cpp ui/WLoadedFilesTreeView.h
    ui/WToolBarPreview.cpp      ui/WToolBarPreview.h
    ui/WToolBarEsquema.cpp      ui/WToolBarEsquema.h

    ui/dialogs/NewEsquema_dlg.cpp       ui/dialogs/NewEsquema_dlg.h      ui/dialogs/NewEsquema_dlg.ui
    ui/dialogs/exportEsquema_dlg.cpp    ui/dialogs/exportEsquema_dlg.h   ui/dialogs/exportEsquema_dlg.ui
    ui/dialogs/ProgBarExport_dlg.cpp    ui/dialogs/ProgBarExport_dlg.h   ui/dialogs/ProgBarExport_dlg.ui
    ui/dialogs/InvalidFileName_dlg.cpp  ui/dialogs/InvalidFileName_dlg.h ui/dialogs/InvalidFileName_dlg.ui
)

set(RESOURCE_FILES
    res/icons.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(FindWhat
        MANUAL_FINALIZATION
        ${SRC_FILES}
        ${DOCUMENT_FILES}
        ${UTILS_FILES}
        ${UI_FILES}
        ${RESOURCE_FILES}
        ui/PEndingStringBlock.h ui/PEndingStringBlock.cpp
    )

else()
    if(ANDROID)
        add_library(FindWhat SHARED
            ${PROJECT_SOURCES}
            ${SRC_FILES}
            ${DOCUMENT_FILES}
            ${UTILS_FILES}
            ${UI_FILES}
            ${RESOURCE_FILES}
        )
    else()
        add_executable(FindWhat
            ${PROJECT_SOURCES}
            ${SRC_FILES}
            ${DOCUMENT_FILES}
            ${UTILS_FILES}
            ${UI_FILES}
            ${RESOURCE_FILES}
        )
    endif()
endif()

# Set the output directory for the executable
set_target_properties(FindWhat PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
# Set the output directory for the executable
set_target_properties(FindWhat PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" )
# Add current source dir as a include path
target_include_directories(FindWhat PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


# LINK LIBRARIES
#QT
target_link_libraries(FindWhat PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.FindWhat)
endif()

# POPPLER
find_library(POPPLER_CPP_LIB NAMES poppler-cpp)
if (POPPLER_CPP_LIB)
    message("Found Poppler library: ${POPPLER_CPP_LIB}")
    target_link_libraries(FindWhat PRIVATE
        ${POPPLER_CPP_LIB}
    )
    # Poppler includes
    find_path(POPPER_CPP_INCLUDE_DIR poppler/cpp)
    if (POPPER_CPP_INCLUDE_DIR)
        message("Found Poppler include directory: ${POPPER_CPP_INCLUDE_DIR}")
        target_include_directories(FindWhat PRIVATE ${POPPER_CPP_INCLUDE_DIR})
    else ()
        message(FATAL_ERROR "Poppler include directory not found")
    endif ()
else ()
    message(FATAL_ERROR "Poppler library not found. You can download and install it from https://gitlab.freedesktop.org/poppler/poppler")
endif ()

set_target_properties(FindWhat PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS FindWhat
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FindWhat)
endif()
