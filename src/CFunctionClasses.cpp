#include "CFunctionClasses.h"
#include "src/CFormula.h"
#include <fstream>
#include "utils/GeneralFunctions.h"

CFunction::CFunction(FunctionType functionType, CFormula *parent) : m_functionType{ functionType }, m_parent{parent} {
    switch(m_functionType) {
    case FunctionType::Find:
        m_functionTypeName = "Find";
        break;
    case FunctionType::MoveIndex:
        m_functionTypeName = "Move Index";
        break;
    case FunctionType::MoveLine:
        m_functionTypeName = "Move Line";
        break;
    case FunctionType::AppendString:
        m_functionTypeName = "Append String";
        break;
    case FunctionType::ExtractData:
        m_functionTypeName = "Extract Data";
        break;
    default:
        m_functionTypeName = "";
    }
}

void CFunction::serialize(std::ofstream& out) const {
    /* - SERIALIZATION ORDER -
     *
     * FunctionType m_functionType
     * size of      m_functionTypeName
     * QString      m_functionTypeName
    */
    out.write(reinterpret_cast<const char*>(&m_functionType), sizeof(FunctionType)); // m_functionType
    SerializationUtils::writeQString(out, m_functionTypeName);                       // m_functionTypeName

    // still need to serialize parent pointer
}


void CFunction::deserialize(std::ifstream& in) {
    /* - DESERIALIZATION ORDER -
     *
     * FunctionType m_functionType
     * size of      m_functionTypeName
     * QString      m_functionTypeName
    */

    in.read(reinterpret_cast<char*>(&m_functionType), sizeof(m_functionType)); // m_functionType
    SerializationUtils::readQString(in, m_functionTypeName);                   // m_functionTypeName
}

CMathFunction::CMathFunction(FunctionType name) : CFunction(name) {}
CMathFunction::CMathFunction(const CMathFunction &other)
    : CFunction(other.m_functionType), m_val1{other.m_val1}, m_val2{other.m_val2} {}



CIndexingFunction::CIndexingFunction(FunctionType name) : CFunction(name) {}
CIndexingFunction::CIndexingFunction(FunctionType name, QString findText, int num, bool option) : CFunction(name), m_text{ findText }, m_num{ num }, m_option{ option } {};
CIndexingFunction::CIndexingFunction(const CIndexingFunction &other)
    : CFunction(other.m_functionType), m_text{other.m_text}, m_num{other.m_num}, m_option{other.m_option} {}



void CIndexingFunction::serialize(std::ofstream& out) const {
    CFunction::serialize(out);
    /* - SERIALIZATION ORDER -
     *
     * buffer   m_text
     * QString  m_text
     * int      m_num
     * bool     m_option
     * bool     m_startFromBeggining
    */

    SerializationUtils::writeQString(out, m_text);                                 // m_text
    out.write(reinterpret_cast<const char*>(&m_num), sizeof(int));                 // m_num
    out.write(reinterpret_cast<const char*>(&m_option), sizeof(bool));             // m_option
    out.write(reinterpret_cast<const char*>(&m_startFromBeggining), sizeof(bool)); // m_startFromBeggining
}

void CIndexingFunction::deserialize(std::ifstream& in) {
    CFunction::deserialize(in);
    /* - DESERIALIZATION ORDER -
     *
     * size of  m_text
     * QString  m_text
     * int      m_num
     * bool     m_option
     * bool     m_startFromBeggining
    */

    SerializationUtils::readQString(in, m_text);                           // m_text
    in.read(reinterpret_cast<char*>(&m_num), sizeof(int));                 // m_num
    in.read(reinterpret_cast<char*>(&m_option), sizeof(bool));             // m_option
    in.read(reinterpret_cast<char*>(&m_startFromBeggining), sizeof(bool)); // m_startFromBeggining
}

CExtractingFunction::CExtractingFunction(FunctionType name) : CFunction(name) {}
CExtractingFunction::CExtractingFunction(const CExtractingFunction &other)
    : CFunction(other.m_functionType), m_charsToGet{other.m_charsToGet}, m_charsToRead{other.m_charsToRead}, m_endingString{other.m_endingString},
    m_invertDirection{other.m_invertDirection}, m_charTypeToGet{other.m_charTypeToGet}, m_toAllow{other.m_toAllow}, m_toAvoid{other.m_toAvoid} {}

void CExtractingFunction::serialize(std::ofstream& out) const {
    CFunction::serialize(out);
    /* - SERIALIZATION ORDER -
     *
     * int           m_charsToGet
     * int           m_charsToRead
     * int           size of m_endingString
     * QString       m_endingString
     * bool          m_invertDirection
     * CharTypeToGet m_charTypeToGet
     * int           size of m_toAllow
     * QString       m_toAllow
     * int           size of m_toAvoid
     * QString       m_toAvoid
     */

    out.write(reinterpret_cast<const char*>(&m_charsToGet), sizeof(int));              // m_charsToGet
    out.write(reinterpret_cast<const char*>(&m_charsToRead), sizeof(int));             // m_charsToRead
    SerializationUtils::writeQString(out, m_endingString);                             // m_endingString
    out.write(reinterpret_cast<const char*>(&m_invertDirection), sizeof(bool));        // m_invertDirection
    out.write(reinterpret_cast<const char*>(&m_charTypeToGet), sizeof(CharTypeToGet)); // m_charTypeToGet
    SerializationUtils::writeQString(out, m_toAllow);                                  // m_toAllow
    SerializationUtils::writeQString(out, m_toAvoid);                                  // m_toAvoid
}

void CExtractingFunction::deserialize(std::ifstream& in) {
    CFunction::deserialize(in);
    /* - DESERIALIZATION ORDER -
     *
     * int           m_charsToGet
     * int           m_charsToRead
     * int           size of m_endingString
     * QString       m_endingString
     * bool          m_invertDirection
     * CharTypeToGet m_charTypeToGet
     * int           size of m_toAllow
     * QString       m_toAllow
     * int           size of m_toAvoid
     * QString       m_toAvoid
     */

    in.read(reinterpret_cast<char*>(&m_charsToGet), sizeof(int));              // m_charsToGet
    in.read(reinterpret_cast<char*>(&m_charsToRead), sizeof(int));             // m_charsToRead
    SerializationUtils::readQString(in, m_endingString);                       // m_endingString
    in.read(reinterpret_cast<char*>(&m_invertDirection), sizeof(bool));        // m_invertDirection
    in.read(reinterpret_cast<char*>(&m_charTypeToGet), sizeof(CharTypeToGet)); // m_charTypeToGet
    SerializationUtils::readQString(in, m_toAllow);                            // m_toAllow
    SerializationUtils::readQString(in, m_toAvoid);                            // m_toAvoid
}
