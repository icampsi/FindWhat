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
    src/CData.cpp \
    src/CEsquema.cpp \
    src/CFormula.cpp \
    src/CFunctionClasses.cpp \
    src/main.cpp \
    ui/PDockPreview.cpp \
    ui/PEsquemaPage.cpp \
    ui/PFormExpToolBoxPage.cpp \
    ui/PMainEsquemaUI.cpp \
    ui/QCItemDelegateFormulaTreeView.cpp \
    ui/QCSpreadSheetTableModel.cpp \
    ui/WBrowserTreeView.cpp \
    ui/WEsquemaListView.cpp \
    ui/WEsquemaTreeView.cpp \
    ui/WLoadedEsquemes.cpp \
    ui/WLoadedFilesTreeView.cpp \
    ui/WSpreadSheetTable.cpp \
    ui/WToolBarEsquema.cpp \
    ui/WToolbarPreview.cpp \
    ui/dialogs/NewEsquema_dlg.cpp \
    ui/dialogs/ExportCSVProgressBar_dlg.cpp \
    ui/dialogs/exportEsquema_dlg.cpp \
    ui/mainwindow.cpp \
    utils/CTextExtractor.cpp \
    utils/GeneralFunctions.cpp

# Header files
HEADERS += \
    document/CDocument.h \
    document/CDocumentSubclasses.h \
    document/CExportCSV.h \
    document/CMDoc.h \
    src/CData.h \
    src/CEsquema.h \
    src/CFormula.h \
    src/CFunctionClasses.h \
    ui/PDockPreview.h \
    ui/PEsquemaPage.h \
    ui/PFormExpToolBoxPage.h \
    ui/PMainEsquemaUI.h \
    ui/QCItemDelegateFormulaTreeView.h \
    ui/QCSpreadSheetTableModel.h \
    ui/WBrowserTreeView.h \
    ui/WEsquemaListView.h \
    ui/WEsquemaTreeView.h \
    ui/WLoadedEsquemes.h \
    ui/WLoadedFilesTreeView.h \
    ui/WSpreadSheetTable.h \
    ui/WToolBarEsquema.h \
    ui/WToolbarPreview.h \
    ui/dialogs/ExportCSVProgressBar_dlg.h \
    ui/dialogs/NewEsquema_dlg.h \
    ui/dialogs/exportEsquema_dlg.h \
    ui/mainWindow.h \
    utils/CTextExtractor.h \
    utils/GeneralFunctions.h

# Include paths
INCLUDEPATH += \
    "poppler/include"

# Form files
FORMS += \
    ui/PDockPreview.ui \
    ui/PEsquemaPage.ui \
    ui/PFormExpToolBoxPage.ui \
    ui/PMainEsquemaUI.ui \
    ui/WLoadedEsquemes.ui \
    ui/dialogs/ExportCSVProgressBar_dlg.ui \
    ui/dialogs/NewEsquema_dlg.ui \
    ui/dialogs/exportEsquema_dlg.ui \
    ui/mainwindow.ui

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

RESOURCES += \
    res/icons.qrc \
    res/icons.qrc
