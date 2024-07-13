/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef USYSTEM_H
#define USYSTEM_H

class QString;
class QWidget;

namespace SystemUtils {
    QString getHomeDirectory();

    // Declaration of global variable
    extern bool G_isLittleEndian;

    // Function to set endianness
    void setEndianness();

    // Check if a stirng contains invalid characters for file name: < > : " / \ | ? *
    bool hasInvalidFileNameChr(const QString& fileName);

    // Helps retrieving the last parent of a widget
    // In theory it should allways be QMainWindow in this app, but i am returning QWidget for  flexibility
    QWidget* getLastParent(QWidget* widget);
}

#endif // USYSTEM_H
