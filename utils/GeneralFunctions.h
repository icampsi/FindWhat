// =================================================== \\
// ====     Copyright (c) 2024 Ignasi Camps       ==== \\
// ==== SPDX-License-Identifier: GPL-3.0-or-later ==== \\
// =================================================== \\

#ifndef GENERALFUNCTIONS_H
#define GENERALFUNCTIONS_H
#include <QString>
#include <QWidget>
#include "fstream"


QString getUserHomeDirectory();

// Allows for scape characters on the text edit widgets (right now only /n is supported since there shouldn't be a need for any other)
QString parseFromText(const QString& text);
QString parseToText(const QString& text);

// Helps retrieving the last parent of a widget
// In theory it should allways be QMainWindow in this app, but i am returning QWidget for  flexibility
QWidget* getLastParent(QWidget* widget);

namespace SerializationUtils {
    // Serialization helpers
    void writeQString(std::ofstream& out, const QString& str);
    void readQString(std::ifstream& in, QString& str);

#include <fstream>

    template<typename Container>
    void writeCustomContainer(std::ofstream& out, const Container& container) {
        size_t size = container.size();
        out.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
        for (auto&& item : container) {
            item->serialize(out);
        }
    }

    template<typename Container, typename item>
    void readCustomContainer(std::ifstream& in, Container& container, item* parent) {
        size_t size;
        in.read(reinterpret_cast<char*>(&size), sizeof(size_t));
        for (size_t i = 0; i < size; ++i) {
            typename item::value_type newItem(in, parent); // Create an instance of the item type by calling its constructor
            container.push_back(newItem);
        }
    }


    template<typename primContainer>
    void writePrimitiveContainer(std::ofstream& out, const primContainer& container) {
        size_t size = container.size();
        out.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
        for (const auto& item : container) {
            out.write(reinterpret_cast<const char*>(&item), sizeof(typename primContainer::value_type));
        }
    }

    template<typename primContainer>
    void readPrimitiveContainer(std::ifstream& in, primContainer& container) {
        int size;
        in.read(reinterpret_cast<char*>(&size), sizeof(int));
        for (int i = 0; i < size; ++i) {
            typename primContainer::value_type item; // Create an instance of the item type
            in.read(reinterpret_cast<char*>(&item), sizeof(typename primContainer::value_type));
            container.push_back(item);
        }
    }


    /*
        template<typename InputIt>
        void serializeContainer(std::ofstream& out, InputIt first, InputIt last) {
            int size = std::distance(first, last);
            out.write(reinterpret_cast<const char*>(&size), sizeof(int));
            for (auto it = first; it != last; ++it) {
                (*it)->serialize(out);
            }
        }

     * serializeContainer(out, t_extractDataFormula.begin(), t_extractDataFormula.end());
     * serializeContainer(out, m_valorsEstatics.begin(), m_valorsEstatics.end());
    */

}


namespace SystemUtils {
// Declaration of global variable
extern bool G_isLittleEndian;

// Function to set endianness
void setEndianness();

// Check if a stirng contains invalid characters for file name: < > : " / \ | ? *
bool containsInvalidFileNameChars(const QString& fileName);

}

#endif // GENERALFUNCTIONS_H
