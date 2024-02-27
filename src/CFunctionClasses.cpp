#include "CFunctionClasses.h"

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
CFunction::FunctionType CFunction::getFunctionType() { return m_functionType; }
QString CFunction::getFunctionTypeName() { return m_functionTypeName; }
CFormula* CFunction::getParent() {return m_parent;}
void CFunction::setParent(CFormula* formulaPtr) {m_parent = formulaPtr;}


CMathFunction::CMathFunction(FunctionType name) : CFunction(name) {}
CMathFunction::CMathFunction(const CMathFunction &other)
    : CFunction(other.m_functionType), m_val1{other.m_val1}, m_val2{other.m_val2} {}



CIndexingFunction::CIndexingFunction(FunctionType name) : CFunction(name) {}
CIndexingFunction::CIndexingFunction(FunctionType name, QString findText, int num, bool option) : CFunction(name), m_text{ findText }, m_num{ num }, m_option{ option } {};
CIndexingFunction::CIndexingFunction(const CIndexingFunction &other)
    : CFunction(other.m_functionType), m_text{other.m_text}, m_num{other.m_num}, m_option{other.m_option} {}



CExtractingFunction::CExtractingFunction(FunctionType name) : CFunction(name) {}
CExtractingFunction::CExtractingFunction(const CExtractingFunction &other)
    : CFunction(other.m_functionType), m_charsToGet{other.m_charsToGet}, m_charsToRead{other.m_charsToRead}, m_endingString{other.m_endingString},
    m_invertDirection{other.m_invertDirection}, m_charTypeToGet{other.m_charTypeToGet}, m_toAllow{other.m_toAllow}, m_toAvoid{other.m_toAvoid} {}
