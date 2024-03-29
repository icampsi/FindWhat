/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CDATA_H
#define CDATA_H

#include <QString>

class CFormula;

class CData {
protected:
    QString   m_dataName{ "" };   // Data Name
    QString   m_dataString{ "" }; // Value
    CFormula *m_parentFormula = nullptr;

public:
    // CONSTURCTORS AND DESTRUCTORS
    explicit CData(const QString& dataName, const QString& dataString = "", CFormula* parent = nullptr) : m_dataName{dataName}, m_dataString{dataString}, m_parentFormula{parent} {}
    explicit CData(const QString& dataName, CFormula* parent = nullptr) : m_dataName{dataName}, m_dataString{""}, m_parentFormula{parent} {}
    explicit CData(std::ifstream& in, CFormula* parent = nullptr) : m_parentFormula{parent} { CData::deserialize(in); } // Serialization constructor
    explicit CData(const CData& other, CFormula *parent = nullptr);
    ~CData() = default;

    CData& operator=(const CData& other);

    // GETTERS AND SETTERS
    const QString& getDataName() const        { return m_dataName;     }
    void setDataName(const QString& dataName) { m_dataName = dataName; }

     const QString& getDataString() const          { return m_dataString;       }
    void setDataString(const QString& dataString) { m_dataString = dataString; }

    CFormula *getParentFormula() const       { return m_parentFormula;   }
    void setParentFormula(CFormula* parent)  { m_parentFormula = parent; }

    void serialize(std::ofstream& out) const;
    void deserialize(std::ifstream& in);

    // enum class DataType { integer, floating, string, empty };
    // DataType m_dataType{ DataType::empty }; // Tipus de dada

    // qint64 m_dataInt{ 0 };              // Dada
    // float m_dataFloat{ 0.0f };           // Dada

    // QString m_assocBegin{ "" };          // Valors a afegir al inici de la string
    // QString m_assocEnd{ "" };            // Valors a afegir al final de la string

    // bool m_error{ false };               // True si s'ha detectat algun error en el procés d'extracció de la dada. Val la pena canviar-ho per un struct que indiqui el tipus d'error exactament?

    // void setData(QString data);
    // void comaToDot(QString& textWithComa, QString& emptyStr);
};

#endif // CDATA_H
