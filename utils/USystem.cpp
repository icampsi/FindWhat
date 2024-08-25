/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "USystem.h"

#ifdef _WIN32
#include <Windows.h> // For GetEnvironmentVariable on Windows
#else
#include <cstdlib>   // For getenv on Unix-like systems
#endif

#include <QWidget>
#include <QString>
#include <QRegularExpression>

namespace SystemUtils {
    QString getHomeDirectory() {
        QString homePath;

    #ifdef _WIN32
        TCHAR userProfile[MAX_PATH];
        DWORD size = sizeof(userProfile);
        if (GetEnvironmentVariable(TEXT("USERPROFILE"), userProfile, size) != 0) {
            homePath = QString::fromWCharArray(userProfile);
        }
    #else
        char* home = getenv("HOME");
        if (home != nullptr) {
            homePath = QString(home);
        }
    #endif

        return homePath;
    }

    // Definition of global variable
    bool G_isLittleEndian;

    // Implementation of function to set endianness
    void setEndianness() {
        int num = 1;
        if (*(reinterpret_cast<char*>(&num)) == 1)  G_isLittleEndian = true;
        else                                        G_isLittleEndian = false;
    }

    bool hasInvalidFileNameChr(const QString& fileName) {
        static const QRegularExpression regex("[<>:\"\\/\\\\|?*]");
        return regex.match(fileName).hasMatch();
    }

    // BOOKMARK - NOT REALLY A SYSTEM UTIL...
    QWidget* getLastParent(QWidget* widget) {
        QWidget* parent = widget->parentWidget();
        if (parent == nullptr) return widget; // base case
        else                   return getLastParent(parent); // Recursively call the function with the parent widget
    }
}
