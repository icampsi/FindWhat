/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef GENERALFUNCTIONS_H
#define GENERALFUNCTIONS_H
#include <QString>
#include <QWidget>
#include <fstream>

#include <type_traits> // for std::is_same
#include <QString>     // for QString


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

    /* Function template for serialization containers of custom classes
     * (only classes that include serialize(std::ofstream& out) and QString */
    template<typename Container>
    void writeCustomContainer(std::ofstream& out, const Container& container) {
        size_t size = container.size();
        out.write(reinterpret_cast<const char*>(&size), sizeof(size_t));

        for (const auto& item : container) {
            // Check if the type of 'item' is QString
            // if constexpr (std::is_same_v<typename q20::remove_cvref_t<decltype(item)>, QString>) {
            //     writeQString(out, item);
            // } else {
                item->serialize(out); // If 'item' is not QString, use the custom serialization methot all my source classes have
            // }
        }
    }

    template<typename Container, typename item>
    void readCustomContainer(std::ifstream& in, const Container& container, item* parent) {
        size_t size;
        in.read(reinterpret_cast<char*>(&size), sizeof(size_t));
        for (size_t i{ 0 }; i < size; ++i) {
            typename item::value_type newItem(in, parent); // Use custom serialization constructor all my source classes have to create a new one
            container.push_back(std::move(newItem));
        }
    }


    template<typename Container>
    void writeCustomQStringContainer(std::ofstream& out, const Container& container) {
        size_t size = container.size();
        out.write(reinterpret_cast<const char*>(&size), sizeof(size_t));

        for (const auto& item : container) {
            SerializationUtils::writeQString(out, item);

            // // Check if the type of 'item' is QString
            // if constexpr (std::is_same_v<typename q20::remove_cvref_t<decltype(item)>, QString>) {
            //     writeQString(out, item);
            // } else {
            //     item->serialize(out); // If 'item' is not QString, use the custom serialization methot all my source classes have
            // }
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
        size_t size;
        in.read(reinterpret_cast<char*>(&size), sizeof(size_t));
        for (int i = 0; i < size; ++i) {
            typename primContainer::value_type item;
            in.read(reinterpret_cast<char*>(&item), sizeof(typename primContainer::value_type));
            container.push_back(item);
        }
    }
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
