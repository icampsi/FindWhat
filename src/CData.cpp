#include "CData.h"
#include "CFormula.h"

CData::CData(const CData& other) :
    m_dataName(other.m_dataName), m_dataString(other.m_dataString) {
    // Perform a deep copy of the parent formula if it exists
    if (other.m_parentFormula) {
        m_parentFormula = new CFormula(*(other.m_parentFormula));
    }
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
