/* =================================================== *
 * ====         Copyright (c) 2024 icampsi        ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include <catch2/catch_all.hpp>
#include <QString>
#include <fstream>
#include <QDebug>
#include "USerialize.h"
#include "finder/CData.h"

TEST_CASE("r/w stringContainer") {

    auto testWriteReadStr = [&](QString input) {
        std::string filename = "test_file.txt"; // Temporary file name
        QString result;

        // Write QString to file
        {
            std::ofstream out(filename, std::ios::binary);
            USerialize::writeQString(out, input);
        }

        // Read QString back from file
        {
            std::ifstream in(filename, std::ios::binary);
            REQUIRE(in.is_open()); // Ensure file opened successfully
            USerialize::readQString(in, result);
        }

        // Check if the read result matches the original QString
        REQUIRE(result == input);

        // Clean up temporary file
        std::remove(filename.c_str());
    };

    SECTION("Short string") {
        testWriteReadStr("s");
        testWriteReadStr("Hello, world!");
    }

    SECTION("Special string") {
        testWriteReadStr("");
        testWriteReadStr("a A 10 ? ' ¡ áéíóú € & % @ \n \t \\ \"");
    }

    SECTION("Long string") {
        QString input = "";
        for(int i = 0; i < 50000; i++) {
            input.append("Hello, World! \n");
        }
        testWriteReadStr(input);
    }

}

TEST_CASE("r/w customContainer") { // using finder classes
    SECTION("type_value noptr") {
        std::vector<CData> datav;
        std::vector<CData> datarc;
        std::string filename = "test_file.txt"; // Temporary file name

        // Create and store CData objects
        for(int i = 0; i < 10; i++) {
            datav.push_back(CData("Data Name", "Data String"));
        }

        // Write customContainer to file
        {
            std::ofstream out(filename, std::ios::binary);
            USerialize::writeCustomContainer(out, datav);
        }

        // Read customContainer back from file
        {
            std::ifstream in(filename, std::ios::binary);
            REQUIRE(in.is_open()); // Ensure file opened successfully
            USerialize::readCustomContainer(in, datarc);
            REQUIRE(datarc == datav);
        }

        // Clean up temporary file
        std::remove(filename.c_str());
    }

    SECTION("type_value ptr") { // using finder classes
        std::vector<CData*> datav;
        std::vector<CData*> datar;
        std::string filename = "test_file.txt"; // Temporary file name

        // Create and store CData objects
        for(int i = 0; i < 10; i++) {
            datav.push_back(new CData("Data Name", "Data String"));
        }

        // Write customContainer to file
        {
            std::ofstream out(filename, std::ios::binary);
            USerialize::writeCustomContainer(out, datav);
        }

        // Read customContainer back from file
        {
            std::ifstream in(filename, std::ios::binary);
            REQUIRE(in.is_open()); // Ensure file opened successfully
            USerialize::readCustomContainer(in, datar);

            // Compare containers
            REQUIRE(datar.size() == datav.size());
            for(size_t i{0}; i < datar.size(); i++) {
                REQUIRE(*datar.at(i) == *datav.at(i));
                delete datar.at(i);
                delete datav.at(i);
            }
        }
        datav.clear();
        datar.clear();

        // Clean up temporary file
        std::remove(filename.c_str());
    }
}
