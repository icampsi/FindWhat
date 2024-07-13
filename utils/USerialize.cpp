/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "USerialize.h"

namespace USerialize {
    void writeQString(std::ofstream& out, const QString& str) {
        // conversion so i can get size in bytes, not characters
        QByteArray utf8Data = str.toUtf8();
        size_t size = utf8Data.size();
        out.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
        out.write(utf8Data.constData(), size);
    }

    void readQString(std::ifstream& in, QString& str) {
        size_t size;
        in.read(reinterpret_cast<char*>(&size), sizeof(size_t));
        QByteArray utf8Data(size, Qt::Uninitialized);  // Reserve space for UTF-8 data
        in.read(utf8Data.data(), size);
        str = QString::fromUtf8(utf8Data);
    }
}
