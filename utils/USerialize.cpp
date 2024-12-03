/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "USerialize.h"
#include "QAbstractItemModel"
#include <QFile>

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

    void writeModel(std::ofstream &out, const QAbstractItemModel* model) {
        int rowCount = model->rowCount();
        int columnCount = model->columnCount();

        // Write row and column counts
        out.write(reinterpret_cast<const char*>(&rowCount), sizeof(int));
        out.write(reinterpret_cast<const char*>(&columnCount), sizeof(int));

        // Write header labels
        for (int column = 0; column < columnCount; ++column) {
            QString header = model->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();
            USerialize::writeQString(out, header);
        }

        // Write model data
        for (int row = 0; row < rowCount; ++row) {
            for (int column = 0; column < columnCount; ++column) {
                QModelIndex index = model->index(row, column);
                QString text = model->data(index, Qt::DisplayRole).toString();
                USerialize::writeQString(out, text);
            }
        }
    }


    void readModel(std::ifstream &in, QAbstractItemModel* model) {
        int rowCount;
        int columnCount;

        in.read(reinterpret_cast<char*>(&rowCount), sizeof(int));
        in.read(reinterpret_cast<char*>(&columnCount), sizeof(int));

        // Insert rows and columns in the model
        model->insertRows(0, rowCount);
        model->insertColumns(0, columnCount);

        // Read header labels
        for (int column = 0; column < columnCount; ++column) {
            QString header;
            USerialize::readQString(in, header);
            model->setHeaderData(column, Qt::Horizontal, header);
        }

        // Read model data
        for (int row = 0; row < rowCount; ++row) {
            for (int column = 0; column < columnCount; ++column) {
                QString text;
                USerialize::readQString(in, text);
                QModelIndex index = model->index(row, column);
                model->setData(index, text);
            }
        }
    }


    void fileFromBinary(const QString &filePath, QByteArray &fileData) {
        QFile pdfFile(filePath);
        if (pdfFile.open(QIODevice::WriteOnly)) {
            pdfFile.write(fileData);
            pdfFile.close();
            qDebug() << "PDF file successfully reconstructed at: " << filePath;
        } else {
            qWarning() << "Failed to open file for writing: " << pdfFile.errorString();
        }
    }
}
