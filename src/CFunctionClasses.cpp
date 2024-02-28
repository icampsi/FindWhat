#include "CFunctionClasses.h"
#include "src/CFormula.h"
#include <fstream>

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

    // m_functionType
    out.write(reinterpret_cast<const char*>(&m_functionType), sizeof(FunctionType));

    // m_functionTypeName
    int typeNameSize = m_functionTypeName.size();
    out.write(reinterpret_cast<const char*>(&typeNameSize), sizeof(int));
    out.write(m_functionTypeName.toStdString().c_str(), typeNameSize);

    // still need to serialize parent pointer
}


void CFunction::deserialize(std::ifstream& in) {
    /* - DESERIALIZATION ORDER -
     *
     * FunctionType m_functionType
     * size of      m_functionTypeName
     * QString      m_functionTypeName
    */

    // m_functionType
    in.read(reinterpret_cast<char*>(&m_functionType), sizeof(FunctionType));

    // m_functionTypeName
    int typeNameSize;
    in.read(reinterpret_cast<char*>(&typeNameSize), sizeof(int));
    char *typeNameBuffer = new char[typeNameSize];
    in.read(typeNameBuffer, typeNameSize);
    m_functionTypeName = QString::fromStdString(std::string(typeNameBuffer, typeNameSize));
    // still need to deserialize parent pointer

    delete[] typeNameBuffer;
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

    // m_text
    int textSize = m_text.size();
    out.write(reinterpret_cast<const char*>(&textSize), sizeof(int));
    out.write(m_text.toUtf8().constData(), textSize);

    // m_num
    out.write(reinterpret_cast<const char*>(&m_num), sizeof(int));

    // m_option
    out.write(reinterpret_cast<const char*>(&m_option), sizeof(bool));

    // m_startFromBeggining
    out.write(reinterpret_cast<const char*>(&m_startFromBeggining), sizeof(bool));
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

    // m_text
    int textSize;
    in.read(reinterpret_cast<char*>(&textSize), sizeof(int));
    char *textBuffer = new char[textSize];
    in.read(textBuffer, textSize);
    m_text = QString::fromUtf8(textBuffer, textSize);

    // m_num
    in.read(reinterpret_cast<char*>(&m_num), sizeof(int));

    // m_option
    in.read(reinterpret_cast<char*>(&m_option), sizeof(bool));

    // m_startFromBeggining
    in.read(reinterpret_cast<char*>(&m_startFromBeggining), sizeof(bool));

    delete[] textBuffer;
}


CExtractingFunction::CExtractingFunction(FunctionType name) : CFunction(name) {}
CExtractingFunction::CExtractingFunction(const CExtractingFunction &other)
    : CFunction(other.m_functionType), m_charsToGet{other.m_charsToGet}, m_charsToRead{other.m_charsToRead}, m_endingString{other.m_endingString},
    m_invertDirection{other.m_invertDirection}, m_charTypeToGet{other.m_charTypeToGet}, m_toAllow{other.m_toAllow}, m_toAvoid{other.m_toAvoid} {}


void CExtractingFunction::serialize(std::ofstream& out) const {
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

    // m_charsToGet
    out.write(reinterpret_cast<const char*>(&m_charsToGet), sizeof(int));

    // m_charsToRead
    out.write(reinterpret_cast<const char*>(&m_charsToRead), sizeof(int));

    // m_endingString
    int endingStringSize = m_endingString.size();
    out.write(reinterpret_cast<const char*>(&endingStringSize), sizeof(int));
    out.write(m_endingString.toUtf8().constData(), endingStringSize);

    // m_invertDirection
    out.write(reinterpret_cast<const char*>(&m_invertDirection), sizeof(bool));

    // m_charTypeToGet
    out.write(reinterpret_cast<const char*>(&m_charTypeToGet), sizeof(CharTypeToGet));

    // m_toAllow
    int toAllowSize = m_toAllow.size();
    out.write(reinterpret_cast<const char*>(&toAllowSize), sizeof(int));
    out.write(m_toAllow.toUtf8().constData(), toAllowSize);

    // m_toAvoid
    int toAvoidSize = m_toAvoid.size();
    out.write(reinterpret_cast<const char*>(&toAvoidSize), sizeof(int));
    out.write(m_toAvoid.toUtf8().constData(), toAvoidSize);
}

void CExtractingFunction::deserialize(std::ifstream& in) {
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

    // m_charsToGet
    in.read(reinterpret_cast<char*>(&m_charsToGet), sizeof(int));

    // m_charsToRead
    in.read(reinterpret_cast<char*>(&m_charsToRead), sizeof(int));

    // m_endingString
    int endingStringSize;
    in.read(reinterpret_cast<char*>(&endingStringSize), sizeof(int));
    char *endingStringBuffer = new char[endingStringSize];
    in.read(endingStringBuffer, endingStringSize);
    m_endingString = QString::fromUtf8(endingStringBuffer, endingStringSize);

    // m_invertDirection
    in.read(reinterpret_cast<char*>(&m_invertDirection), sizeof(bool));

    // m_charTypeToGet
    in.read(reinterpret_cast<char*>(&m_charTypeToGet), sizeof(CharTypeToGet));

    // m_toAllow
    int toAllowSize;
    in.read(reinterpret_cast<char*>(&toAllowSize), sizeof(int));
    char *toAllowBuffer = new char[toAllowSize];
    in.read(toAllowBuffer, toAllowSize);
    m_toAllow = QString::fromUtf8(toAllowBuffer, toAllowSize);

    // m_toAvoid
    int toAvoidSize;
    in.read(reinterpret_cast<char*>(&toAvoidSize), sizeof(int));
    char *toAvoidBuffer = new char[toAvoidSize];
    in.read(toAvoidBuffer, toAvoidSize);
    m_toAvoid = QString::fromUtf8(toAvoidBuffer, toAvoidSize);

    delete[] endingStringBuffer;
    delete[] toAllowBuffer;
    delete[] toAvoidBuffer;
}
