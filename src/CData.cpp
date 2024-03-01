#include "CData.h"
#include "CFormula.h"
#include "utils/GeneralFunctions.h"

#include <fstream>

CData::CData(const CData& other, CFormula *parent) :
    m_dataName(other.m_dataName), m_dataString(other.m_dataString) {
}

CData& CData::operator=(const CData& other) {
    if (this != &other) { // Check for self-assignment
        // Copy member variables
        m_dataName   = other.m_dataName;
        m_dataString = other.m_dataString;

        // Perform a deep copy of the parent formula if it exists
        if (other.m_parentFormula) {
            // Allocate new memory and copy the formula
            m_parentFormula = new CFormula(*(other.m_parentFormula));
        } else {
            m_parentFormula = nullptr; // No parent formula in 'other', so set to nullptr in this object
        }
    }
    return *this;
}

void CData::serialize(std::ofstream& out) const {
    /* - SERIALIZATION ORDER -
     * int       size of m_dataName
     * QString   m_dataName
     * int       size of m_dataString
     * QString   m_dataString
     * CFormula *m_parentFormula - NEED TO CHECK HOW TO DO THAT
    */

    SerializationUtils::writeQString(out, m_dataName);   // m_dataName
    SerializationUtils::writeQString(out, m_dataString); // m_dataString
}

void CData::deserialize(std::ifstream& in) {
    /* - DESERIALIZATION ORDER -
     * int       size of m_dataName
     * QString   m_dataName
     * int       size of m_dataString
     * QString   m_dataString
     * CFormula *m_parentFormula - NEED TO CHECK HOW TO DO THAT
    */

    SerializationUtils::readQString(in, m_dataName);   // m_dataName
    SerializationUtils::readQString(in, m_dataString); // m_dataString
}

// CData::CData(QString dataName, QString dataString, DataType dataType, QString assocBegin, QString assocEnd) {
//     m_dataName = dataName;
//     m_dataType = dataType;
//     m_assocBegin = assocBegin;
//     m_assocEnd = assocEnd;
//     setData(dataString);
// }

// CData::CData(QString dataName, QString dataString, DataType dataType) {
//     m_dataName = dataName;
//     m_dataType = dataType;
//     m_assocBegin = "";
//     m_assocEnd = "";
//     setData(dataString);
// }

// CData::CData(QString dataName, QString dataString) {
//     m_dataName = dataName;
//     m_dataType = DataType::string;
//     m_assocBegin = "";
//     m_assocEnd = "";
//     setData(dataString);
// }

// void CData::comaToDot(QString& textWithComa, QString& emptyStr) {
//     for (int i = 0; i < textWithComa.size(); ++i) {
//         if (textWithComa[i] == ',') {
//             emptyStr += '.';
//         } else {
//             emptyStr += textWithComa[i];
//         }
//     }
// }

// void CData::setData(QString data) {
//     m_dataString = data;

//     switch (m_dataType) {
//     case DataType::integer:
//         m_dataInt = data.toLongLong();
//         break;

//     case DataType::floating:
//         QString dottedNumber;
//         comaToDot(data, dottedNumber);
//         m_dataFloat = dottedNumber.toFloat();
//         break;
//     }
// }
