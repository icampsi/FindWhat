/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CFUNCTIONCLASSES_H
#define CFUNCTIONCLASSES_H

#include <QString>

class CFormula;

class CFunction {
public:
    enum class Action { Find, MoveIndex, MoveLine, AppendString, ExtractData };

protected:
    Action       m_action;
    QString      m_functionTypeName;
    CFormula*    m_parent = nullptr;

public:
    CFunction(Action functionType, CFormula* parent = nullptr);
    CFunction(std::ifstream& in, CFormula* parent = nullptr) : m_parent{parent} { CFunction::deserialize(in); } // Serialization constructor
    virtual ~CFunction() {} // Virtual destructor

    virtual Action getFunctionType() const { return m_action; }

    void setFunctionTypeName(const QString& name) { m_functionTypeName =  name; }
    QString getFunctionTypeName() const           { return m_functionTypeName;  }

    CFormula* getParent() const          {return m_parent;}
    void setParent(CFormula* formulaPtr) {m_parent = formulaPtr;}

    // Serialization
    void virtual serialize(std::ofstream& out) const;
    void virtual deserialize(std::ifstream& in);
};

class CIndexingFunction : public CFunction {
protected:
    QString m_text{ "" };
    int     m_num{ 0 };
    bool    m_option{ true };
    bool    m_startFromBeggining{ false };

public:
    CIndexingFunction(Action name);
    CIndexingFunction(Action name, QString findText, int num = 0, bool option = false);
    CIndexingFunction(const CIndexingFunction &other);
    CIndexingFunction(std::ifstream& in, CFormula* parent = nullptr)
        : CFunction(in, parent) { CIndexingFunction::deserialize(in); } // Serialization constructor

    virtual ~CIndexingFunction() {}

    // Getters and setters
    QString getText() const              { return m_text; }
    void    setText(const QString& text) { m_text = text; }

    int  getNum()        { return m_num; }
    void setNum(int num) { m_num = num; }

    bool getOption() const      { return m_option; }
    void setOption(bool option) { m_option = option; }

    bool getStartFromBeggining() const      { return m_startFromBeggining; }
    void setStartFromBeggining(bool option) { m_startFromBeggining = option; }

    // Serialization
    void serialize(std::ofstream& out) const override;
    void deserialize(std::ifstream& in) override;
};

class CMathFunction : public CFunction {
public:
    enum class Operation { add, subtract, divide, multiply };

    QString   m_val1{ "" };
    QString   m_val2{ "" };
    Operation m_operation{ Operation::add };

    CMathFunction(Action name);
    CMathFunction(const CMathFunction &other);
    virtual ~CMathFunction() {} // Virtual destructor
};

class CExtractingFunction : public CFunction {
public:
    enum class CharTypeToGet { all, digit, letter };

protected:
    int          m_charsToGet{ -1 };         // If negative, ignored. Stop when this amount of chars have been extracted.
    int          m_charsToRead{ -1 };        // If negative, ignored. Stop when this amount of chars have been read.
    QString      m_endingString = "\n";      // When this string is found, stop reading. Can be a single character, e.g., \n, space, ., etc.
    bool         m_invertDirection{ false }; // Invert the direction in which the data is read.
    CharTypeToGet m_charTypeToGet = CharTypeToGet::all; // Specifies if only numbers, characters, or all should be obtained
    QString       m_toAllow{ "" };
    QString       m_toAvoid{ "" };

public:
    CExtractingFunction(Action name);
    CExtractingFunction(const CExtractingFunction &other);
    CExtractingFunction(std::ifstream& in, CFormula* parent = nullptr)
        : CFunction(in, parent) { CExtractingFunction::deserialize(in); } // Serialization constructor
    virtual ~CExtractingFunction() {}

    // Getters and setters
    int  getCharsToGet() const         { return m_charsToGet; }
    void setCharsToGet(int charsToGet) { m_charsToGet = charsToGet; }

    int  getCharsToRead() const          { return m_charsToRead; }
    void setCharsToRead(int charsToRead) { m_charsToRead = charsToRead; }

    QString getEndingString() const            { return m_endingString; }
    void setEndingString(QString endingString) { m_endingString = endingString; }

    bool getInvertedDirection() const                 { return m_invertDirection; }
    void setInvertedDirection(bool invertedDirection) { m_invertDirection = invertedDirection; }

    CharTypeToGet getCharTypeToGet() const             { return m_charTypeToGet; }
    void setCharTypeToGet(CharTypeToGet charTypeToGet) { m_charTypeToGet = charTypeToGet; }

    QString getToAllow() const       { return m_toAllow; }
    void setToAllow(QString toAllow) { m_toAllow = toAllow; }

    QString getToAvoid() const       { return m_toAvoid; }
    void setToAvoid(QString toAvoid) { m_toAvoid = toAvoid; }

    // Serialization
    void serialize(std::ofstream& out)  const override;
    void deserialize(std::ifstream& in) override;
};

#endif // CFUNCTIONCLASSES_H
