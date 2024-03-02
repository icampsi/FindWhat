#include "CEsquema.h"
#include "utils/GeneralFunctions.h"

#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>

CEsquema::CEsquema(const QString nameEsquema, std::vector<CFormula*> tExtractDataFormula, std::vector<CData*>& valorsEstatics, QString IDText)
    : m_nameEsquema(nameEsquema), m_IDText(IDText), t_extractDataFormula(tExtractDataFormula), m_staticData(valorsEstatics),
    m_csvFormatFormula(), m_XSVStructureResult(), m_dataForCheck1(), m_dataForCheck2(), m_fileNamePlaceholder(), m_outputDirectori() {}

CEsquema::CEsquema(const QString nameEsquema, std::vector<CFormula*> tExtractDataFormula, QString IDText)
    : m_nameEsquema(nameEsquema), m_IDText(IDText), t_extractDataFormula(tExtractDataFormula),
    m_csvFormatFormula(), m_XSVStructureResult(), m_dataForCheck1(), m_dataForCheck2(), m_fileNamePlaceholder(), m_outputDirectori() {}

CEsquema::~CEsquema() {
    // Delete dynamically allocated CData objects in m_valorsEstatics vector
    for (auto data : m_staticData) {
        delete data;
    }
    // Delete dynamically allocated CFormula objects in t_extractDataFormula vector
    for (auto formula : t_extractDataFormula) {
        delete formula;
    }

    // Delete dynamically allocated CData objects in m_XSVStructureResult vector of vectors
    for (auto& row : m_XSVStructureResult) {
        for (auto data : row) {
            delete data;
        }
    }
}

void inline CEsquema::formatDate(QString& data) {
    for (unsigned int i{ 0 }; i < data.size(); i++)
        if (data.at(i) == '-') data[i] = '/';

    int trackFormat{ 0 };
    QString dia;
    QString mes;
    QString any;
    for (unsigned int i{ 0 }; i < data.size(); i++) {
        if (data.at(i) == '/')	trackFormat++;
        else {
            switch (trackFormat) {
            case 0:
                dia.append(data.at(i));
                break;
            case 1:
                mes.append(data.at(i));
                break;
            case 2:
                any.append(data[i]);
                break;
            }
        }
    }
    data = any + '-' + mes + '-' + dia;
}

void CEsquema::createFileName(QString& newFileName) {
    QRegularExpression regex("<(.*?)>");
    QRegularExpressionMatchIterator matches = regex.globalMatch(m_fileNamePlaceholder);

    newFileName = m_fileNamePlaceholder;

    while (matches.hasNext()) {
        // Extract the captured string
        QRegularExpressionMatch match = matches.next();
        QString capturedString = match.captured(1);

        if (m_dataMap.contains(capturedString)) {
            // Replace the captured string with the corresponding value from the map
            newFileName.replace(match.capturedStart(0), match.capturedLength(0), m_dataMap.value(capturedString)->getDataString());
        } else {
            newFileName.replace(match.capturedStart(1), match.capturedLength(1), "ERROR_REPLACING_PLACEHOLDER");
        }
    }
}

void CEsquema::deleteFormula(int index) {
    if (index >= 0 && index < t_extractDataFormula.size()) {
        t_extractDataFormula.erase(t_extractDataFormula.begin() + index);
    } else {
        qDebug() << "index for formula is out of range of it's esquema t_extractorDataFormula vector";
    }
}

void CEsquema::addStaticData(CData* data ) {
    m_dataMap.insert(data->getDataName(), data); // Update m_dataMap
    m_staticData.push_back(data);
}

void CEsquema::deleteStaticData(int index) {
    if (index >= 0 && index < m_staticData.size()) {
        CData* data = m_staticData[index];
        m_dataMap.remove(data->getDataName()); // Update m_dataMap
        delete data;
        m_staticData.erase(m_staticData.begin() + index);
    }
    else qDebug() << "Index out of bounds for m_valorsEstatics vector";
}

void CEsquema::setCsvFormatFormula(const QString& rLine, char enclosureChar, char separator) {
    m_csvFormatFormula.clear();
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
            m_csvFormatFormula.push_back(cell);
            cell.clear();
        }
    }
}

void CEsquema::generateXSVStructure(QString &text) {
    m_XSVStructureResult.clear();
    std::vector<CData*> row;

    for(int i{0}; i < m_csvFormatFormula.size(); i++) {
        // Store the name of the data inside the formula we are checking
        QString dataName = m_csvFormatFormula.at(i);

        // Finds the data either from static data or t_extractDataFormula
        auto it = m_dataMap.find(dataName);
        if (it != m_dataMap.end()) {
            CData    *data    = it.value();
            CFormula *formula = data->getParentFormula();

            // If the data has a formula associated, it means that it is not a static data but a variable, so we extract the value first
            if(formula) { formula->applyFormula(text); }
            row.push_back(data);
        }
        else qDebug() << "data named " + dataName + " was not found. Be sure to have written it correctly and cas sensitive";
    }
    m_XSVStructureResult.push_back(row);
}

void CEsquema::xsvm_structureToString(QString* pFullFileString, char enclosureChar, char separator) {
    for (int i{ 0 }; i < m_XSVStructureResult.size(); i++) {
        for (int k{ 0 }; k < m_XSVStructureResult[i].size(); k++) {
            // Posem cometes al principi i al final de la cela, i fem un segon loop per accedir als caracters individuals de
            // l'string d'aquesta cela. Copiem els caracters un per un perquè així, si trobem ", hi insertem una altra ".
            if (!m_XSVStructureResult[i][k]->getDataString().isEmpty()) { // Si la cela ha de ser buida, no facis tot el procés de sota: Evita que es posen "" a les celes buides.
                pFullFileString->append("\"");
                for (unsigned int index{ 0 }; index < m_XSVStructureResult[i][k]->getDataString().length(); index++) {
                    if (m_XSVStructureResult[i][k]->getDataString()[index] == '\"') {
                        pFullFileString->append("\"\"");
                    }
                    else {
                        pFullFileString->append(m_XSVStructureResult[i][k]->getDataString()[index]);
                    }
                }
                pFullFileString->append(enclosureChar);
            }
            pFullFileString->append(separator);
        }
        pFullFileString->append('\n');
    }
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
     * int                              size of m_fileNameFormula
     * QString                          m_fileNameFormula
     * int                              size of m_outputDirectori
     * QString                          m_outputDirectori
     *
     * - NO NEED -
     * m_XSVStructureResult
     * m_csvFormatFormula       - TO BE RECONSTRUCTED ON DESERIALIZATION from m_fileNameFormula
     * m_dataMap                - TO BE RECONSTRUCTED ON DESERIALIZATION from m_valorsEstatics and t_extractDataFormula
     *
     */

    SerializationUtils::writeQString(out, m_nameEsquema);                // m_nameEsquema
    SerializationUtils::writeQString(out, m_IDText);                     // m_IDText
    SerializationUtils::writeCustomContainer(out, t_extractDataFormula); // t_extractDataFormula
    SerializationUtils::writeCustomContainer(out, m_staticData);     // m_valorsEstatics

    SerializationUtils::writePrimitiveContainer(out, m_dataForCheck1);   // m_dataForCheck1
    SerializationUtils::writePrimitiveContainer(out, m_dataForCheck2);   // m_dataForCheck2
    SerializationUtils::writeQString(out, m_fileNamePlaceholder);            // m_fileNameFormula
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
     * int                              size of m_fileNameFormula
     * QString                          m_fileNameFormula
     * int                              size of m_outputDirectori
     * QString                          m_outputDirectori
     *
     * - RECONSTRUCTION -
     * m_csvFormatFormula       - RECONSTRUCTED ON DESERIALIZATION from m_fileNameFormula
     * m_dataMap                - RECONSTRUCTED ON DESERIALIZATION from m_valorsEstatics and t_extractDataFormula
     *
     * - NO NEED -
     * m_XSVStructureResult
     */

    SerializationUtils::readQString(in, m_nameEsquema);                // m_nameEsquema
    SerializationUtils::readQString(in, m_IDText);                     // m_nameEsquema
    int extractDataFormulaSize;
    in.read(reinterpret_cast<char*>(&extractDataFormulaSize), sizeof(int));
    for(int i{0}; i < extractDataFormulaSize; i++ ) {
        CFormula* formula = new CFormula(in);
        t_extractDataFormula.push_back(formula);
    }
    // SerializationUtils::readCustomContainer(in, t_extractDataFormula, this); // t_extractDataFormula
    int valorsEstaticsSize;
    in.read(reinterpret_cast<char*>(&valorsEstaticsSize), sizeof(int));
    for(int i{0}; i < valorsEstaticsSize; i++ ) {
        CData* data = new CData(in);
        m_staticData.push_back(data);
    }
    // SerializationUtils::readCustomContainer(in, m_valorsEstatics, this);     // m_valorsEstatics
    SerializationUtils::readPrimitiveContainer(in, m_dataForCheck1);   // m_dataForCheck1
    SerializationUtils::readPrimitiveContainer(in, m_dataForCheck2);   // m_dataForCheck2
    SerializationUtils::readQString(in, m_fileNamePlaceholder);            // m_fileNameFormula
    SerializationUtils::readQString(in, m_outputDirectori);            // m_outputDirectori
}
