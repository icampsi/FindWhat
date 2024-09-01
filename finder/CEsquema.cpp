/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CEsquema.h"
#include "utils/UText.h"
#include "utils/USystem.h"
#include "utils/USerialize.h"

#include "CFormula.h"

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
    auto replacer = [this](const QString& capturedString) -> QString {
        if (m_dataMap.contains(capturedString)) {
            return m_dataMap.value(capturedString)->getDataString();
        } else {
            return "ERROR_REPLACING_PLACEHOLDER";
        }
    };

    newFileName = fileNamePlaceholder;
    UText::replacePlaceholders(newFileName, "<(.*?)>", replacer);
    
    return !SystemUtils::hasInvalidFileNameChr(newFileName);
}

void CEsquema::deleteFormula(const size_t index) {
    if (index < m_extractDataFormula.size()) {
        m_extractDataFormula.erase(m_extractDataFormula.begin() + index);
    } else {
        qDebug() << "index for formula is out of range of it's esquema m_extractorDataFormula vector";
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

void CEsquema::setStaticDataName(CData* data, const QString &name) {
    // Update m_dataMap
    QString oldName = data->getDataName();
    m_dataMap.insert(name, data);
    if (m_dataMap.contains(oldName)) {
        m_dataMap.take(oldName);        // Remove the old key-value pair and get the value
        m_dataMap.insert(name, data);   // Insert the value with the new key
    }
    data->setDataName(name);
}

void CEsquema::setFormulaName(CFormula* formula, const QString &name) {
    m_dataMap.insert(name, formula->getData());
    // Update m_dataMap
    QString oldName = formula->getDataName();
    if (m_dataMap.contains(oldName)) {
        m_dataMap.take(oldName);  // Remove the old key-value pair and get the value
        m_dataMap.insert(name, formula->getData()); // Insert the value with the new key
    }
    formula->setDataName(name);
}

void CEsquema::addExtractDataFormula(CFormula* formula) {
    m_dataMap.insert(formula->getDataName(), formula->getData());
    m_extractDataFormula.push_back(formula);
}

void CEsquema::serialize(std::ofstream& out) const {
    /* - SERIALIZATION ORDER -
     * QString                          m_nameEsquema
     * std::vector<CFormula*>           m_extractDataFormula
     * std::vector<CData*>              m_staticData
     *
     * - NO NEED -
     * m_dataMap                - TO BE RECONSTRUCTED ON DESERIALIZATION from m_staticData and m_extractDataFormula
     */

    USerialize::writeQString(out, m_nameEsquema);                // m_nameEsquema
    USerialize::writeCustomContainer(out, m_extractDataFormula); // m_extractDataFormula
    USerialize::writeCustomContainer(out, m_staticData);         // m_staticData
}

void CEsquema::deserliazile(std::ifstream& in) {
    /* - DESERIALIZATION ORDER -
     * QString                          m_nameEsquema
     * std::vector<CFormula*>           m_extractDataFormula
     * std::vector<CData*>              m_staticData
     *
     * - RECONSTRUCTION -
     * m_dataMap                - RECONSTRUCTED ON DESERIALIZATION from m_staticData and m_extractDataFormula
     */

    USerialize::readQString(in, m_nameEsquema); // m_nameEsquema

    size_t extractDataFormulaSize{ 0 };
    in.read(reinterpret_cast<char*>(&extractDataFormulaSize), sizeof(size_t));
    for(size_t i{0}; i < extractDataFormulaSize; i++ ) {
        CFormula* formula = new CFormula(in);
        addExtractDataFormula(formula);
    }

    size_t staticDataSize;
    in.read(reinterpret_cast<char*>(&staticDataSize), sizeof(size_t));
    for(size_t i{0}; i < staticDataSize; i++ ) {
        CData* data = new CData(in);
        addStaticData(data);
    }
}
