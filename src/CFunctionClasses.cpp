/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CFunctionClasses.h"

#include <fstream>

#include "utils/GeneralFunctions.h"
#include "CFormula.h"

CFunction::CFunction(Action functionType, CFormula *parent) : m_action{ functionType }, m_parent{parent} {
    switch(m_action) {
    case Action::Find:
        m_functionTypeName = "Find";
        break;
    case Action::MoveIndex:
        m_functionTypeName = "Move Index";
        break;
    case Action::MoveLine:
        m_functionTypeName = "Move Line";
        break;
    case Action::AppendString:
        m_functionTypeName = "Append String";
        break;
    case Action::ExtractData:
        m_functionTypeName = "Extract Data";
        break;
    default:
        m_functionTypeName = "";
    }
}

void CFunction::serialize(std::ofstream& out) const {
    /* - SERIALIZATION ORDER -
     *
     * Action       m_action
     * size of      m_functionTypeName
     * QString      m_functionTypeName
    */

    out.write(reinterpret_cast<const char*>(&m_action), sizeof(Action)); // m_action
    SerializationUtils::writeQString(out, m_functionTypeName);           // m_functionTypeName
}


void CFunction::deserialize(std::ifstream& in) {
    /* - DESERIALIZATION ORDER -
     *
     * Action       m_action
     * size of      m_functionTypeName
     * QString      m_functionTypeName
    */

    in.read(reinterpret_cast<char*>(&m_action), sizeof(Action)); // m_action
    SerializationUtils::readQString(in, m_functionTypeName);     // m_functionTypeName
}

CMathFunction::CMathFunction(Action name) : CFunction(name) {}
CMathFunction::CMathFunction(const CMathFunction &other)
    : CFunction(other.m_action), m_val1{other.m_val1}, m_val2{other.m_val2} {}



CIndexingFunction::CIndexingFunction(Action name) : CFunction(name) {
    if (name == Action::Find) m_num = -1; // Set m_num to negative if it is a Find function so by default the full document is looked at instead of page 0;
}
CIndexingFunction::CIndexingFunction(const CIndexingFunction &other)
    : CFunction(other.m_action), m_text{other.m_text}, m_num{other.m_num}, m_option{other.m_option} {}

void CIndexingFunction::serialize(std::ofstream& out) const {
    CFunction::serialize(out);
    /* - SERIALIZATION ORDER -
     *
     * size_t   size of m_text
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
    /* - DESERIALIZATION ORDER -
     *
     * size_t   size of m_text
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

CExtractingFunction::CExtractingFunction(Action name) : CFunction(name), m_endingStr{ "\n" } {}

CExtractingFunction::CExtractingFunction(const CExtractingFunction &other)
    : CFunction(other.m_action),
    m_charsToGet{other.m_charsToGet},
    m_charsToRead{other.m_charsToRead},
    m_invertDirection{other.m_invertDirection},
    m_endingStr{other.m_endingStr},
    m_charTypeToGet{other.m_charTypeToGet},
    m_toAllow{other.m_toAllow},
    m_toAvoid{other.m_toAvoid} {
}

void CExtractingFunction::serialize(std::ofstream& out) const {
    CFunction::serialize(out);
    /* - SERIALIZATION ORDER -
     *
     * int           m_charsToGet
     * int           m_charsToRead
     * int           size of m_endingStr
     * int           size of m_endingStr stored string
     * QString       m_endingStr stored string
     * bool          m_invertDirection
     * CharTypeToGet m_charTypeToGet
     * int           size of m_toAllow
     * QString       m_toAllow
     * int           size of m_toAvoid
     * QString       m_toAvoid
     * int           size of m_toReplace
     * QString       m_toReplace
     * int           size of m_replaceFor
     * QString       m_replaceFor
     */

    out.write(reinterpret_cast<const char*>(&m_charsToGet), sizeof(int));              // m_charsToGet
    out.write(reinterpret_cast<const char*>(&m_charsToRead), sizeof(int));             // m_charsToRead
    SerializationUtils::writeCustomQStringContainer(out, m_endingStr);                 // m_endingStr - BOOKMARK - NEEDS FIXING
    out.write(reinterpret_cast<const char*>(&m_invertDirection), sizeof(bool));        // m_invertDirection
    out.write(reinterpret_cast<const char*>(&m_charTypeToGet), sizeof(CharTypeToGet)); // m_charTypeToGet
    SerializationUtils::writeQString(out, m_toAllow);                                  // m_toAllow
    SerializationUtils::writeQString(out, m_toAvoid);                                  // m_toAvoid
    SerializationUtils::writeQString(out, m_toReplace);                                // m_toReplace
    SerializationUtils::writeQString(out, m_replaceFor);                               // m_replaceFor
}

void CExtractingFunction::deserialize(std::ifstream& in) {
    /* - DESERIALIZATION ORDER -
     *
     * int           m_charsToGet
     * int           m_charsToRead
     * int           size of m_endingStr
     * int           size of m_endingStr stored string
     * QString       m_endingStr stored string
     * bool          m_invertDirection
     * CharTypeToGet m_charTypeToGet
     * int           size of m_toAllow
     * QString       m_toAllow
     * int           size of m_toAvoid
     * QString       m_toAvoid
     * int           size of m_toReplace
     * QString       m_toReplace
     * int           size of m_replaceFor
     * QString       m_replaceFor
     */

    in.read(reinterpret_cast<char*>(&m_charsToGet) , sizeof(int));             // m_charsToGet
    in.read(reinterpret_cast<char*>(&m_charsToRead), sizeof(int));             // m_charsToRead
    SerializationUtils::readCustomQStringContainer(in, m_endingStr);           // m_endingStr - BOOKMARK - NEEDS FIXING
    in.read(reinterpret_cast<char*>(&m_invertDirection), sizeof(bool));        // m_invertDirection
    in.read(reinterpret_cast<char*>(&m_charTypeToGet), sizeof(CharTypeToGet)); // m_charTypeToGet
    SerializationUtils::readQString(in, m_toAllow);                            // m_toAllow
    SerializationUtils::readQString(in, m_toAvoid);                            // m_toAvoid
    SerializationUtils::readQString(in, m_toReplace);                          // m_toReplace
    SerializationUtils::readQString(in, m_replaceFor);                         // m_replaceFor
}
