QT += core gui \
       uitools

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following makes the code fail to compile
# if it uses any deprecated Qt APIs.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Source files
SOURCES += \
    document/CDocument.cpp \
    document/CDocumentSubclasses.cpp \
    document/CExportCSV.cpp \
    document/CMDoc.cpp \
    exportCSVProgressBar_dlg.cpp \
    src/CData.cpp \
    src/CEsquema.cpp \
    src/CFormula.cpp \
    src/CFunctionClasses.cpp \
    src/main.cpp \
    ui/PEsquemaPage.cpp \
    ui/QCItemDelegateFormulaTreeView.cpp \
    ui/WBrowserTreeView.cpp \
    ui/WDockPreview.cpp \
    ui/WEsquemaTreeView.cpp \
    ui/WFormExpToolBoxPage.cpp \
    ui/WLoadedEsquemes.cpp \
    ui/WLoadedFilesTreeView.cpp \
    ui/WMainEsquemaUI.cpp \
    ui/WSpreadSheetTable.cpp \
    ui/WSpreadSheetTableModel.cpp \
    ui/WToolBarEsquema.cpp \
    ui/WToolbarPreview.cpp \
    ui/dialogs/NewEsquema_dlg.cpp \
    ui/dockspreadsheet.cpp \
    ui/mainwindow.cpp \
    utils/CTextExtractor.cpp \
    utils/GeneralFunctions.cpp

# Header files
HEADERS += \
    CFileData.h \
    document/CDocument.h \
    document/CDocumentSubclasses.h \
    document/CExportCSV.h \
    document/CMDoc.h \
    exportCSVProgressBar_dlg.h \
    src/CData.h \
    src/CEsquema.h \
    src/CFormula.h \
    src/CFunctionClasses.h \
    ui/PEsquemaPage.h \
    ui/QCItemDelegateFormulaTreeView.h \
    ui/WBrowserTreeView.h \
    ui/WDockPreview.h \
    ui/WFormExpToolBoxPage.h \
    ui/WLoadedEsquemes.h \
    ui/WLoadedFilesTreeView.h \
    ui/WMainEsquemaUI.h \
    ui/WSpreadSheetTable.h \
    ui/WSpreadSheetTableModel.h \
    ui/WToolBarEsquema.h \
    ui/WToolbarPreview.h \
    ui/dialogs/NewEsquema_dlg.h \
    ui/dockspreadsheet.h \
    ui/mainWindow.h \
    ui/wesquematreeview.h \
    utils/CTextExtractor.h \
    utils/GeneralFunctions.h

# Include paths
INCLUDEPATH += \
    "poppler/include"

# Form files
FORMS += \
    exportcsvprogressbar_dlg.ui \
    ui/PEsquemaPage.ui \
    ui/WDockPreview.ui \
    ui/WFormExpToolBoxPage.ui \
    ui/WLoadedEsquemes.ui \
    ui/WMainEsquemaUI.ui \
    ui/dialogs/NewEsquema_dlg.ui \
    ui/dockspreadsheet.ui \
    ui/mainwindow.ui \
    ui/prova.ui

# Debug and Release library linking
debug {
    LIBS += -L"./poppler/debug/lib" \
            -lpoppler \
            -lpoppler-cpp
} else {
    LIBS += -L"./poppler/lib" \
            -lpoppler \
            -lpoppler-cpp
}

# Default rules for deployment.
TARGET = FindWhat
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
