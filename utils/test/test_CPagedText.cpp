/* =================================================== *
 * ====         Copyright (c) 2024 icampsi        ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include <catch2/catch_all.hpp>
#include "CPagedText.h"
#include "qdebug.h"

struct TestData {
    std::vector<QString> pages;
    QString pagesFT;
    std::vector<QString> unnorm_pages;
    QString unnorm_pagesFT;
};

TestData initializeTestData(QString pageText = "page ", size_t pageCount = 100) {
    TestData data;
    data.pages.reserve(pageCount);
    for(size_t i = 0; i < pageCount; i++) {
        QString page = pageText;
        page.append(QString::number(i));
        data.pagesFT.append(page);
        data.pages.push_back(page);

        page.prepend("\n\n\n\n\n       \t\t\t\t       \r\r\r\r\r       \n\n");
        page.append("\n\n\n\n\n       \t\t\t\t       \r\r\r\r\r       \n\n");
        data.unnorm_pagesFT.append(page);
        data.unnorm_pages.push_back(page);
    }
    return data;
}

void runTests(CPagedText& pt, const TestData& data, const std::string& instanceName) {
    SECTION(instanceName + " - Basic") {
        // pageCount()
        REQUIRE(pt.pageCount() == data.pages.size());
        // getFullText() with multiple calls
        for(int i = 0; i < 20; i++) {
            REQUIRE(pt.getFullText() == data.pagesFT);
        }
    }

    SECTION(instanceName + " - Individual pages") {
        {
            // Test each page content through getPagedDoc() and getPage()
            const std::vector<CPagedText::Page> pages = pt.getPagedDoc();
            for(size_t i = 0; i < pt.pageCount(); i++) {
                REQUIRE(data.pages.at(i) == pages.at(i).pageText);
                REQUIRE(pt.getPage(i).pageText == pages.at(i).pageText);
            }
        }

        {
            size_t pageNum = pt.pageCount() - 1; // last page as example
            size_t textIndex = pt.getPage(pageNum).pageText.length() - 1; // last character as example

            // Test calculateComprehensiveIndex();
            size_t compIndex = pt.calculateComprehensiveIndex(textIndex, pageNum); // comprehensive idex
            REQUIRE(pt.getPage(pageNum).pageText.at(textIndex) == pt.getFullText().at(compIndex));

            // Test getPageRelativeIndex()
            CPagedText::RelativeIndex relInd = pt.getPageRelativeIndex(compIndex);
            REQUIRE(relInd.pageIndex == pageNum);
            REQUIRE(relInd.charIndex == textIndex);
        }
    }
}

TEST_CASE("CPagedText page constructor Tests") {
    TestData data = initializeTestData();

    CPagedText pt(data.pages);
    CPagedText ptNorm(data.unnorm_pages, true);
    runTests(pt, data, "page constructor");
    runTests(ptNorm, data, "page constructor norm.");
}

TEST_CASE("CPagedText push_backing pages Tests") {
    TestData data = initializeTestData();

    CPagedText pt;
    CPagedText ptNorm;

    for(const QString& page : data.pages) {
        pt.push_back(page);
    }
    for(const QString& unnorm_page : data.unnorm_pages) {
        ptNorm.push_back(unnorm_page, true);
    }
    runTests(pt, data, " push_backing pages");
    runTests(ptNorm, data, " push_backing pages norm.");
}

TEST_CASE("CPagedText - one page constructor - Tests") {
    TestData data = initializeTestData("page ", 1);

    CPagedText pt(data.pagesFT);
    CPagedText ptNorm(data.unnorm_pagesFT, true);
    runTests(pt, data, " push_backing pages");
    runTests(ptNorm, data, " push_backing pages norm.");
}
