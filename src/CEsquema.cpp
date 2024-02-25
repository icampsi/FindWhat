#include "CEsquema.h"

CEsquema::CEsquema(const QString nameEsquema, std::vector<CFormula*> tExtractDataFormula, std::vector<CData*>& valorsEstatics, QString IDText)
    : m_valorsEstatics(valorsEstatics), m_nameEsquema(nameEsquema), m_IDText(IDText), t_extractDataFormula(tExtractDataFormula),
    m_dataForCheck1(), m_dataForCheck2(), m_csvFormatFormula(), m_outputDirectori(), m_fileNameFormula(), m_XSVStructureResult() {}

CEsquema::CEsquema(const QString nameEsquema, std::vector<CFormula*> tExtractDataFormula, QString IDText)
    : m_nameEsquema(nameEsquema), m_IDText(IDText), t_extractDataFormula(tExtractDataFormula),
    m_dataForCheck1(), m_dataForCheck2(), m_csvFormatFormula(), m_outputDirectori(), m_fileNameFormula(), m_XSVStructureResult() {}

CEsquema::~CEsquema() {
    // Delete dynamically allocated CData objects in m_valorsEstatics vector
    for (auto data : m_valorsEstatics) {
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

void CEsquema::renameFile(const char* oldName, const char* newName) {
    std::rename(oldName, newName);
}

// void CEsquema::createFileName(CFileData& file) {

//     QString tempStr{ "" };
//     int dataIndex;

//     file.m_newFileName.append(m_outputDirectori);
//     if (file.m_error) { file.m_newFileName += "ERROR - "; }

//     for (unsigned int i{ 0 }; i < m_fileNameFormula.length(); i++) {
//         tempStr = "";
//         if (m_fileNameFormula[i] == '$') {
//             i++;
//             while (m_fileNameFormula[i] != '$') {
//                 tempStr.append(m_fileNameFormula.at(i));
//                 i++;
//             }
//             dataIndex = tempStr.toInt();
//             if (file.m_extractedFileData.size() > dataIndex) {
//                 file.m_newFileName += file.m_extractedFileData[dataIndex].getDataString();
//             }
//             else {QMessageBox::critical(nullptr, "ERROR", "Error en escriure el nom del document. Revisa els paràmetres.", QMessageBox::Ok);}
//         }
//         if (m_fileNameFormula[i] != '$') { file.m_newFileName.append(m_fileNameFormula.at(i)); }
//     }
// }

// void CEsquema::createFileData(QString& text, CFileData& file) {
//     // VALORS VARIABLES -----------------------------------------------------------------

//     /*
//     for (int i{0}; i < m_extractDataFormula.size(); i++) {
//         m_extractDataFormula[i].applyFormula(text, &file.m_extractedFileData);
//         CData t_data(m_extractDataFormula[i].m_dataName, m_extractDataFormula[i].m_result);
//         m_extractDataFormula[i].m_result = "";
//         file.m_extractedFileData.push_back(t_data);
//     }
//     */

//     //for (int i{ 0 }; i < m_extractDataFormula.size(); i++) {
//     //	CData data = extractData(text, m_extractDataFormula[i]);
//     //	file.m_extractedFileData.push_back(data);
//     //	if (data.m_error == true) { file.m_error = true; }
//     //}

//     // CHECK IF "IVA + BI = TOTAL" I CANVIEM NOM DEL PDF ORIGINAL ------------------------
//     //if (checkData(file)) { file.m_error = true; } // ================================== NO FUNCIONA BÉ!!!!!!!!!!!!!!
// }

// ============================================================================================================================
// ============================================================================================================================
// ============================================================================================================================

// bool CEsquema::checkData(CFileData& file) {
//      bool error = false;
//     // {
//     //     float value1{ file.m_extractedFileData[m_dataForCheck1[0]].m_dataFloat };
//     //     float value2{ file.m_extractedFileData[m_dataForCheck1[1]].m_dataFloat };
//     //     float value3{ file.m_extractedFileData[m_dataForCheck1[2]].m_dataFloat };

//     //     float prova = value1 + value2;
//     //     if (!(value3 < prova + 0.0001 && value3 > prova - 0.0001)) { error = true; }
//     // }

//     // for (int i{ 0 }; i < file.m_extractedFileData.size(); i++) {
//     //     if (file.m_extractedFileData[i].m_error) { error = true; }
//     //     if (file.m_extractedFileData[i].m_dataString.isEmpty()) { error = true; }
//     // }

//      return error;
// }

//CData CEsquema::extractData(QString& text, extractDataFormula currentFormula) {   // Extreu els valors variables a partir de les formules
//	QString extractedString{ "" };
//
//	size_t index = text.find(currentFormula.referenceText);
//	if (index == QString::npos) { std::cout << "No s'ha trobat el text: " + currentFormula.referenceText << std::endl; }
//	else {
//		if (currentFormula.beginLine) {
//			while (index != 0) { // Garanteix que l'index sempre comenci al principi de la línia
//				index--;
//				if (text[index] == '\n') {
//					index++;
//					break;
//				}
//			}
//		}
//		else {
//			if (currentFormula.lineCount == 0) { index += currentFormula.referenceText.length(); }
//			else { moveLine(currentFormula.lineCount, index, text); }
//		}
//
//		for (index; text[index] != currentFormula.endingChar; index++) {
//			extractedString += text[index];
//		}
//	}
//
//	if (currentFormula.DataName == "data") {
//		formatDate(extractedString);
//	}
//	CData data(currentFormula.DataName, extractedString, currentFormula.dataType, "", "");
//	return data;
//}

void CEsquema::deleteFormula(int index) {
    if (index >= 0 && index < t_extractDataFormula.size()) {
        t_extractDataFormula.erase(t_extractDataFormula.begin() + index);
    } else {
        qDebug() << "index for formula is out of range of it's esquema t_extractorDataFormula vector";
    }
}

void CEsquema::addStaticData(CData* data ) {
    m_dataMap.insert(data->getDataName(), data); // Update m_dataMap
    m_valorsEstatics.push_back(data);
}

void CEsquema::deleteStaticData(int index) {
    if (index >= 0 && index < m_valorsEstatics.size()) {
        CData* data = m_valorsEstatics[index];
        m_dataMap.remove(data->getDataName()); // Update m_dataMap
        delete data;
        m_valorsEstatics.erase(m_valorsEstatics.begin() + index);
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
        QString dataName = m_csvFormatFormula.at(i);

        auto it = m_dataMap.find(dataName);
        if (it != m_dataMap.end()) {
            CData    *data    = it.value();
            CFormula *formula = data->getParentFormula();

            if(formula) { formula->applyFormula(text); }             // If the data has a formula associated, it means that it is not a static data but a variable, so we extract the value first
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


