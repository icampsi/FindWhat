#include "generalfunctions.h"

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

