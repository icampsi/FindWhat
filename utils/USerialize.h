/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef USERIALIZE_H
#define USERIALIZE_H

#include <QString>
#include <fstream>

class QAbstractItemModel;
namespace USerialize {
    void writeQString(std::ofstream& out, const QString& str);
    void readQString(std::ifstream& in, QString& str);

    template<typename Container>
    void writeCustomContainer(std::ofstream& out, const Container& container) {
        size_t size = container.size();
        out.write(reinterpret_cast<const char*>(&size), sizeof(size_t));

        for (const auto& item : container) {
            if constexpr (std::is_pointer_v<typename Container::value_type>) {
                item->serialize(out);
            } else {
                item.serialize(out);
            }
        }
    }

    template<typename Container>
    void readCustomContainer(std::ifstream& in, Container& container) {
        size_t size;
        in.read(reinterpret_cast<char*>(&size), sizeof(size_t));

        for (size_t i{ 0 }; i < size; ++i) {
            if constexpr (std::is_pointer<typename Container::value_type>::value) {
                typename Container::value_type newItem = new std::remove_pointer_t<typename Container::value_type>(in);
                container.push_back(newItem);
            } else {
                typename Container::value_type newItem(in);
                container.push_back(std::move(newItem));
            }
        }
    }

    template<typename Container>
    void writeCustomQStringContainer(std::ofstream& out, const Container& container) {
        size_t size = container.size();
        out.write(reinterpret_cast<const char*>(&size), sizeof(size_t));

        for (const auto& item : container) {
            USerialize::writeQString(out, item);
        }
    }

    template<typename Container>
    void readCustomQStringContainer(std::ifstream& in, Container& container) {
        size_t size;
        in.read(reinterpret_cast<char*>(&size), sizeof(size_t));
        for (size_t i{ 0 }; i < size; ++i) {
            QString tempStr;
            readQString(in, tempStr);
            container.push_back(std::move(tempStr));
        }
    }
    
    void writeModel(std::ofstream &out, const QAbstractItemModel* model);
    void readModel(std::ifstream &in, QAbstractItemModel* model);

    // Writes a QByteArray to a file using QFile. Filepath must include extension
    void fileFromBinary(const QString &filePath, QByteArray &fileData) ;
}

#endif // USERIALIZE_H
