// =================================================== \\
// ====     Copyright (c) 2024 Ignasi Camps       ==== \\
// ==== SPDX-License-Identifier: GPL-3.0-or-later ==== \\
// =================================================== \\

#include "generalfunctions.h"

#include <QRegularExpression>

#include <cstdlib>   // For getenv on Unix-like systems
#include <Windows.h> // For GetEnvironmentVariable on Windows


QString getUserHomeDirectory() {
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

QString parseFromText(const QString& text) {
    QString parsedText;
    for(int i{0}; i < text.length(); i++) {
        if(text.at(i) == '\\' && i < text.length() - 1) {
            if(text.at(i+1) == 'n') {
                parsedText.append('\n');
                i++;
            }
            else parsedText.append(text.at(i));
        }
        else parsedText.append(text.at(i));
    }

    return parsedText;
}

QString parseToText(const QString& text) {
    QString parsedText;
    for(int i{0}; i < text.length(); i++) {
        if(text == '\n') {
            parsedText.append("\\n");
        }
        else parsedText.append(text.at(i));
    }
    return parsedText;
}

QWidget* getLastParent(QWidget* widget) {
    QWidget* parent = widget->parentWidget();
    if (parent == nullptr) return widget; // base case
    else                   return getLastParent(parent); // Recursively call the function with the parent widget
}

namespace SerializationUtils {
    void writeQString(std::ofstream& out, const QString& str) {
        // conversion so i can get size in bytes, not characters
        QByteArray utf8Data = str.toUtf8();
        int size = utf8Data.size();
        out.write(reinterpret_cast<const char*>(&size), sizeof(int));
        out.write(utf8Data.constData(), size);
    }

    void readQString(std::ifstream& in, QString& str) {
        int size;
        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        QByteArray utf8Data(size, Qt::Uninitialized);  // Reserve space for UTF-8 data
        in.read(utf8Data.data(), size);
        str = QString::fromUtf8(utf8Data);
    }
}

namespace SystemUtils {
    // Definition of global variable
    bool G_isLittleEndian;

    // Implementation of function to set endianness
    void setEndianness() {
        int num = 1;
        if (*(reinterpret_cast<char*>(&num)) == 1)  G_isLittleEndian = true;
        else                                        G_isLittleEndian = false;
    }

    bool containsInvalidFileNameChars(const QString& fileName) {
        QRegularExpression regex("[<>:\"\\/\\\\|?*]");
        return regex.match(fileName).hasMatch();
    }
}


