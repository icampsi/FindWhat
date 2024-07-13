/* =================================================== *
 * ====         Copyright (c) 2024 icampsi        ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include <catch2/catch_all.hpp>

#include <QString>
#include "../USystem.h"

#include <QString>
#include <QWidget>
#include <QApplication>
#include <QRegularExpression>
#ifdef _WIN32
#include <windows.h>
#endif
#include <cstdlib>

namespace SystemUtils {
    extern bool G_isLittleEndian;

    QString getHomeDirectory();
    void setEndianness();
    bool hasInvalidFileNameChr(const QString& fileName);
    QWidget* getLastParent(QWidget* widget);
}

// Test case for getHomeDirectory
TEST_CASE("getHomeDirectory") {
    QString homeDir = SystemUtils::getHomeDirectory();

#ifdef _WIN32
    TCHAR userProfile[MAX_PATH];
    DWORD size = sizeof(userProfile);
    REQUIRE(GetEnvironmentVariable(TEXT("USERPROFILE"), userProfile, size) != 0);
    QString expectedHomeDir = QString::fromWCharArray(userProfile);
    REQUIRE(homeDir == expectedHomeDir);
#else
    char* home = getenv("HOME");
    REQUIRE(home != nullptr);
    QString expectedHomeDir = QString(home);
    REQUIRE(homeDir == expectedHomeDir);
#endif
}

// Test case for setEndianness
TEST_CASE("setEndianness") {
    SystemUtils::setEndianness();
    int num = 1;
    bool expectedIsLittleEndian = (*(reinterpret_cast<char*>(&num)) == 1);
    REQUIRE(SystemUtils::G_isLittleEndian == expectedIsLittleEndian);
}

// Test case for hasInvalidFileNameChr
TEST_CASE("hasInvalidFileNameChr") {
    REQUIRE(SystemUtils::hasInvalidFileNameChr("valid_filename.txt") == false);
    REQUIRE(SystemUtils::hasInvalidFileNameChr("invalid<filename>.txt") == true);
    REQUIRE(SystemUtils::hasInvalidFileNameChr("another:invalid|filename*?.txt") == true);
}

// Test case for getLastParent
TEST_CASE("getLastParent") {
    int argc = 1;
    char* argv[] = { (char*)"" };
    QApplication app(argc, argv);

    QWidget topLevelWidget;
    QWidget childWidget(&topLevelWidget);
    QWidget grandChildWidget(&childWidget);

    REQUIRE(SystemUtils::getLastParent(&grandChildWidget)   == &topLevelWidget);
    REQUIRE(SystemUtils::getLastParent(&childWidget)        == &topLevelWidget);
    REQUIRE(SystemUtils::getLastParent(&topLevelWidget)     == &topLevelWidget);
}
