/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *

 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CEsquema.h"
#include "utils/GeneralFunctions.h"

#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>

CEsquema::CEsquema(const QString nameEsquema, std::vector<CFormula*> tExtractDataFormula, std::vector<CData*>& valorsEstatics, QString IDText)
    : m_nameEsquema(nameEsquema), m_IDText(IDText), m_extractDataFormula(tExtractDataFormula), m_staticData(valorsEstatics),
    m_csvFormatFormulaStructure(), m_XSVStringStructureResult(), m_dataForCheck1(), m_dataForCheck2(), m_outputDirectori() {}

CEsquema::CEsquema(const QString nameEsquema, std::vector<CFormula*> tExtractDataFormula, QString IDText)
    : m_nameEsquema(nameEsquema), m_IDText(IDText), m_extractDataFormula(tExtractDataFormula),
    m_csvFormatFormulaStructure(), m_XSVStringStructureResult(), m_dataForCheck1(), m_dataForCheck2(), m_outputDirectori() {}

CEsquema::~CEsquema() {
    // Delete dynamically allocated CData objects in m_valorsEstatics vector
    for (auto data : m_staticData) {
        delete data;
    }
    // Delete dynamically allocated CFormula objects in t_extractDataFormula vector
    for (auto formula : m_extractDataFormula) {
        delete formula;
    }
}

bool CEsquema::createFileName(QString& newFileName, const QString &fileNamePlaceholder) const {
    QRegularExpression regex("<(.*?)>");
    QRegularExpressionMatchIterator matches = regex.globalMatch(fileNamePlaceholder);

    newFileName = fileNamePlaceholder;

    while (matches.hasNext()) {
        // Extract the captured string
        QRegularExpressionMatch match = matches.next();
        QString capturedString = match.captured(1);

        if (m_dataMap.contains(capturedString)) {
            // Replace the captured string with the corresponding value from the map
            newFileName.replace(match.capturedStart(0), match.capturedLength(0), m_dataMap.value(capturedString)->getDataString());
        } else {
            newFileName.replace(match.capturedStart(0), match.capturedLength(0), "ERROR_REPLACING_PLACEHOLDER");
        }
        matches = regex.globalMatch(newFileName); // Repeat the search so indexes get updated
    }
    if(SystemUtils::containsInvalidFileNameChars(newFileName)) {
        return false;
    }
    else return true;
}

void CEsquema::deleteFormula(size_t index) {
    if (index < m_extractDataFormula.size()) {
        m_extractDataFormula.erase(m_extractDataFormula.begin() + index);
    } else {
        qDebug() << "index for formula is out of range of it's esquema t_extractorDataFormula vector";
    }
}

void CEsquema::addStaticData(CData* data ) {
    m_dataMap.insert(data->getDataName(), data); // Update m_dataMap
    m_staticData.push_back(data);
}

void CEsquema::deleteStaticData(size_t index) {
    if (index < m_staticData.size()) {
        CData* data = m_staticData[index];
        m_dataMap.remove(data->getDataName()); // Update m_dataMap
        delete data;
        m_staticData.erase(m_staticData.begin() + index);
    }
    else qDebug() << "Index out of bounds for m_valorsEstatics vector";
}

void CEsquema::setStaticDataName(CData* data, QString name) {
    // Update m_dataMap
    QString oldName = data->getDataName();
    m_dataMap.insert(name, data);
    if (m_dataMap.contains(oldName)) {
        m_dataMap.take(oldName);        // Remove the old key-value pair and get the value
        m_dataMap.insert(name, data);   // Insert the value with the new key
    }
    data->setDataName(name);
}

void CEsquema::constructCsvFormatFormulaStructure(const QString& rLine, char enclosureChar, char separator) {
    m_csvFormatFormulaStructure.clear();
    QString cell{ "" };
    for (unsigned int i{ 0 }; i < rLine.length(); i++) {
        if (rLine[i] != separator && i != rLine.length() - 1) {
            if (rLine[i] == enclosureChar) {
                i++;
                if (rLine[i] == enclosureChar) {
                    cell.append(rLine[i]);
                    i++;
                }
                else {
                    while (rLine[i] != enclosureChar) {
                        cell.append(rLine[i]);
                        if (i == rLine.length()) break;
                        i++;
                    }
                }
            }
            if (rLine[i] != enclosureChar) { cell.append(rLine[i]); }
        }
        else {
            if (i == rLine.length() - 1 && rLine[i] != separator) {
                cell.append(rLine[i]);
            }
            m_csvFormatFormulaStructure.push_back(cell);
            cell.clear();
        }
    }
}

void CEsquema::xsv_stringStructureToString(QString* pFullFileString, char enclosureChar, char separator) {
    for (size_t i{ 0 }; i < m_XSVStringStructureResult.size(); i++) {
        for (size_t k{ 0 }; k < m_XSVStringStructureResult[i].size(); k++) {
            // Add enclosure char on both ends and start second loop for accessing individual chars from the string
            // of this cell. We copy each char and check if " is found, so we can add another one to keep it as a scape.
            const QString &cell = m_XSVStringStructureResult[i][k];
            if (!cell.isEmpty()) { // Si la cela ha de ser buida, no facis tot el procés de sota: Evita que es posen "" a les celes buides.
                pFullFileString->append("\"");
                for (unsigned int index{ 0 }; index < cell.length(); index++) {
                    if (cell[index] == '\"') {
                        pFullFileString->append("\"\"");
                    }
                    else {
                        pFullFileString->append(cell[index]);
                    }
                }
                pFullFileString->append(enclosureChar);
            }
            pFullFileString->append(separator);
        }
        pFullFileString->append('\n');
    }
}


void CEsquema::generateXSVStringStructure(const QString &text) {
    m_XSVStringStructureResult.clear();
    std::vector<QString> row;

    for(size_t i{0}; i < m_csvFormatFormulaStructure.size(); i++) {
        QString cell = m_csvFormatFormulaStructure.at(i);
        QRegularExpression regex("<(.*?)>");
        QRegularExpressionMatchIterator matches = regex.globalMatch(cell);

        while (matches.hasNext()) {
            // Extract the captured string
            QRegularExpressionMatch match = matches.next();
            QString capturedString = match.captured(1);

            // Finds the data either from static data or t_extractDataFormula
            auto it = m_dataMap.find(capturedString);
            if (it != m_dataMap.end()) {
                CData    *data    = it.value();
                CFormula *formula = data->getParentFormula();

                // If the data has a formula associated, it means that it is not a static data but a variable, so we extract the value first
                if(formula) { formula->applyFormula(text); }
                // Replace the captured string with the corresponding value from the map
                cell.replace(match.capturedStart(0), match.capturedLength(0), data->getDataString());
            }
            else {
                cell.replace(match.capturedStart(1), match.capturedLength(1), "ERROR_REPLACING_PLACEHOLDER");
            }
            // Repeat the loock up for matches so indexes of found match get updated and the substitutions are performed where they should
            matches = regex.globalMatch(cell);
        }
        row.push_back(std::move(cell));
    }
    m_XSVStringStructureResult.push_back(std::move(row));
}

void CEsquema::serialize(std::ofstream& out) const {
    /* - SERIALIZATION ORDER -
     * int                              size of m_nameEsquema
     * QString                          m_nameEsquema
     * int                              size of m_IDText
     * QString                          m_IDText
     * int                              size of t_extractDataFormula
     * std::vector<CFormula*>           t_extractDataFormula
     * int                              size of m_valorsEstatics
     * std::vector<CData*>              m_valorsEstatics
     * int                              size of m_dataForCheck1
     * std::vector<unsigned int>        m_dataForCheck1
     * int                              size of m_dataForCheck2
     * std::vector<unsigned int>        m_dataForCheck2
     * int                              size of m_outputDirectori
     * QString                          m_outputDirectori
     *
     * - NO NEED -
     * m_XSVStringStructureResult
     * m_csvFormatFormula       - TO BE RECONSTRUCTED ON DESERIALIZATION from m_fileNameFormula
     * m_dataMap                - TO BE RECONSTRUCTED ON DESERIALIZATION from m_valorsEstatics and t_extractDataFormula
     *
     */

    SerializationUtils::writeQString(out, m_nameEsquema);                // m_nameEsquema
    SerializationUtils::writeQString(out, m_IDText);                     // m_IDText
    SerializationUtils::writeCustomContainer(out, m_extractDataFormula); // t_extractDataFormula
    SerializationUtils::writeCustomContainer(out, m_staticData);         // m_valorsEstatics

    SerializationUtils::writePrimitiveContainer(out, m_dataForCheck1);   // m_dataForCheck1
    SerializationUtils::writePrimitiveContainer(out, m_dataForCheck2);   // m_dataForCheck2
    SerializationUtils::writeQString(out, m_outputDirectori);            // m_outputDirectori
}

void CEsquema::deserliazile(std::ifstream& in) {
    /* - DESERIALIZATION ORDER -
     * int                              size of m_nameEsquema
     * QString                          m_nameEsquema
     * int                              size of m_IDText
     * QString                          m_IDText
     * int                              size of t_extractDataFormula
     * std::vector<CFormula*>           t_extractDataFormula
     * int                              size of m_valorsEstatics
     * std::vector<CData*>              m_valorsEstatics
     * int                              size of m_dataForCheck1
     * std::vector<unsigned int>        m_dataForCheck1
     * int                              size of m_dataForCheck2
     * std::vector<unsigned int>        m_dataForCheck2
     * int                              size of m_outputDirectori
     * QString                          m_outputDirectori
     *
     * - RECONSTRUCTION -
     * m_csvFormatFormula       - RECONSTRUCTED ON DESERIALIZATION from m_fileNameFormula
     * m_dataMap                - RECONSTRUCTED ON DESERIALIZATION from m_valorsEstatics and t_extractDataFormula
     *
     * - NO NEED -
     * m_XSVStringStructureResult
     */

    SerializationUtils::readQString(in, m_nameEsquema);                // m_nameEsquema
    SerializationUtils::readQString(in, m_IDText);                     // m_nameEsquema
    int extractDataFormulaSize;
    in.read(reinterpret_cast<char*>(&extractDataFormulaSize), sizeof(int));
    for(int i{0}; i < extractDataFormulaSize; i++ ) {
        CFormula* formula = new CFormula(in);
        addExtractDataFormula(formula);
    }
    // SerializationUtils::readCustomContainer(in, t_extractDataFormula, this); // t_extractDataFormula
    int valorsEstaticsSize;
    in.read(reinterpret_cast<char*>(&valorsEstaticsSize), sizeof(int));
    for(int i{0}; i < valorsEstaticsSize; i++ ) {
        CData* data = new CData(in);
        addStaticData(data);
    }
    // SerializationUtils::readCustomContainer(in, m_valorsEstatics, this);     // m_valorsEstatics
    SerializationUtils::readPrimitiveContainer(in, m_dataForCheck1);   // m_dataForCheck1
    SerializationUtils::readPrimitiveContainer(in, m_dataForCheck2);   // m_dataForCheck2
    SerializationUtils::readQString(in, m_outputDirectori);            // m_outputDirectori
}


// FUTURE UPDATES

// void inline CEsquema::formatDate(QString& data) {
//     for (unsigned int i{ 0 }; i < data.size(); i++)
//         if (data.at(i) == '-') data[i] = '/';

//     int trackFormat{ 0 };
//     QString dia;
//     QString mes;
//     QString any;
//     for (unsigned int i{ 0 }; i < data.size(); i++) {
//         if (data.at(i) == '/')	trackFormat++;
//         else {
//             switch (trackFormat) {
//             case 0:
//                 dia.append(data.at(i));
//                 break;
//             case 1:
//                 mes.append(data.at(i));
//                 break;
//             case 2:
//                 any.append(data[i]);
//                 break;
//             }
//         }
//     }
//     data = any + '-' + mes + '-' + dia;
// }
