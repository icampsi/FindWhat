#ifndef CESQUEMADOC_H
#define CESQUEMADOC_H

#include "CDocument.h"
#include "CExportCSV.h"
#include <vector>

#include <QFile>
#include <QTextStream>
#include <QMessageBox>

class CEsquemaDoc : public CDocument {
public:
    // Constructors and destructors
    CEsquemaDoc(CEsquema* esquema) : m_esquema{esquema} {}
    ~CEsquemaDoc() override {
        delete m_esquema;
        m_esquema = nullptr;
    }

    // Getters
    CEsquema* getEsquema() const { return m_esquema; }

protected:
    // Members
    CEsquema* m_esquema = nullptr; // The associated esquema to this doc.
};

class CPdfDoc : public CDocument {
public:
    //Constructors and destructors
    CPdfDoc(const QString& filePath) : m_filePath{filePath} {
        PDFToTextPoppler();
    }
    ~CPdfDoc() = default;

    //Getters (Don't need for setters, it should all be assigned on construction and never changed)
    QString getText()       { return m_text; }
    QString getFilePath()   { return m_filePath; }

private:
    // Private Functions
    void PDFToTextPoppler();

protected:
    // Members
    QString m_text;
    QString m_filePath;
};


// Document to hold all the information for the export path to follow
class CExportPathDoc {
public:
    //Constructors and destructors
    CExportPathDoc() : m_exportCSVs(){}
    ~CExportPathDoc();

private:
    std::vector<CExportCSV*> m_exportCSVs;

public:
    // INTERFACE FOR m_exportCSVs
    // Create a new CExportCSV object and return a pointer to it
    CExportCSV* createExportCSV();

    // Get the size of the vector
    size_t getExportCSVCount() const;

    // Get a pointer to a specific CExportCSV object by index
    CExportCSV* getExportCSVByIndex(size_t index);
    const std::vector<CExportCSV*>& getExportCSVs() { return m_exportCSVs; }
    void deleteExportCSV(int index) {
        // Check if the index is within valid range
        if (index < 0 || index >= m_exportCSVs.size()) {
            qDebug() << "Invalid index:" << index;
            return;
        }

        // Delete the item at the specified index
        delete m_exportCSVs[index];
        m_exportCSVs.erase(m_exportCSVs.begin() + index);
    }

    // Creates a .csv File from a xsvStringStructure
    void xsvm_stringStructureToFile(const QString& fileName, std::vector<std::vector<QString>>& rXSVStructure, QChar separator) {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(nullptr, QStringLiteral("ERROR"), QStringLiteral("Couldn't create file"));
            return;
        }

        QTextStream out(&file);
        for (const auto& row : rXSVStructure) {
            for (const auto &cell : row) {
                out << '"';
                for (const QChar& c : cell) {
                    if (c == '"')
                        out << "\"\"";
                    else
                        out << c;
                }
                out << '"' << separator;
            }
            out << '\n';
        }
        QMessageBox::information(nullptr, "Succes!", ".csv file created succesfully");
    }
};
#endif // CESQUEMADOC_H
