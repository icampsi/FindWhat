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
    // MEMBERS
    Action       m_action;
    QString      m_functionTypeName;
    CFormula*    m_parent = nullptr;

public:
    // CONSTRUCTORS&DESTRUCTORS
    explicit CFunction(Action functionType, CFormula* parent = nullptr);
    explicit CFunction(std::ifstream& in,   CFormula* parent = nullptr) : m_parent{parent} { CFunction::deserialize(in); } // Serialization constructor
    virtual ~CFunction() {} // Virtual destructor

    //SETTERS&GETTERS
    virtual Action getFunctionType() const { return m_action; }
    const QString& getFunctionTypeName() const { return m_functionTypeName;  }

    CFormula* getParent() const          {return m_parent;}
    void setParent(CFormula* formulaPtr) {m_parent = formulaPtr;}

    // SERIALIZATION
    void virtual serialize(std::ofstream& out) const;
    void virtual deserialize(std::ifstream& in);
};

class CIndexingFunction : public CFunction {
protected:
    // MEMBERS
    QString m_text{ "" };
    int     m_num{ 0 };
    bool    m_option{ true };
    bool    m_startFromBeggining{ false };

public:
    // CONSTRUCTORS&DESTRUCTORS
    explicit CIndexingFunction(Action name);
    CIndexingFunction(const CIndexingFunction &other);
    CIndexingFunction(std::ifstream& in, CFormula* parent = nullptr)
        : CFunction(in, parent) { CIndexingFunction::deserialize(in); } // Serialization constructor

    virtual ~CIndexingFunction() {}

    // GETTERS&SETTERS
    const QString& getText() const              { return m_text; }
    void           setText(const QString& text) { m_text = text; }

    int  getNum() const        { return m_num; }
    void setNum(const int num) { m_num = num;  }

    bool getOption() const      { return m_option;   }
    void setOption(bool option) { m_option = option; }

    bool getStartFromBeggining() const      { return m_startFromBeggining;   }
    void setStartFromBeggining(bool option) { m_startFromBeggining = option; }

    // SERIALIZATION
    void serialize(std::ofstream& out) const override;
    void deserialize(std::ifstream& in) override;
};

class CMathFunction : public CFunction {
public:
    enum class Operation { add, subtract, divide, multiply };

    QString   m_val1{ "" };
    QString   m_val2{ "" };
    Operation m_operation{ Operation::add };

    explicit CMathFunction(Action name);
    explicit CMathFunction(const CMathFunction &other);
    virtual ~CMathFunction() {} // Virtual destructor
};

class CExtractingFunction : public CFunction {
public:
    enum class CharTypeToGet { all, digit, letter };

protected:
    // MEMBERS
    int           m_charsToGet{ -1 };         // If negative, ignored. Stop when this amount of chars have been extracted.
    int           m_charsToRead{ -1 };        // If negative, ignored. Stop when this amount of chars have been read.
    bool          m_invertDirection{ false }; // Invert the direction in which the data is read.
    std::vector<QString> m_endingStr; // When this string is found, stop reading. Can be a single character, e.g., \n, space, ., etc.
    CharTypeToGet m_charTypeToGet = CharTypeToGet::all; // Specifies if only numbers, characters, or all should be extracted
    QString       m_toAllow{ "" };    // Specifies characters to allow even if they are blocked by m_charTypeToGet
    QString       m_toAvoid{ "" };    // Specifies characters to avoid even if they are allowed by m_charTypeToGet
    QString       m_toReplace{ "" };  // Define a substring to be replaced if found
    QString       m_replaceFor{ "" }; // Define for what will the substring be replaced

public:
    // CONSTRUCTORS&DESTRUCTORS
    explicit CExtractingFunction(Action name);
    explicit CExtractingFunction(const CExtractingFunction &other);
    explicit CExtractingFunction(std::ifstream& in, CFormula* parent = nullptr)
        : CFunction(in, parent) { CExtractingFunction::deserialize(in); } // Serialization constructor
    virtual ~CExtractingFunction() {}

    // GETTERS&SETTERS
    inline int  getCharsToGet() const         { return m_charsToGet; }
    inline void setCharsToGet(int charsToGet) { m_charsToGet = charsToGet; }

    inline int  getCharsToRead() const          { return m_charsToRead; }
    inline void setCharsToRead(int charsToRead) { m_charsToRead = charsToRead; }

    inline const std::vector<QString>& getEndingStringBlock() const { return m_endingStr; }
    inline void addEndingStringBlock(const QString& endingString) { m_endingStr.push_back(std::move(endingString)); }
    inline void deleteEndingStringBlockMember(size_t index) {
        if (index < m_endingStr.size()) {
            m_endingStr.erase(m_endingStr.begin() + index);
        }
    }
    inline void modifyEndingStringBlock(size_t i, const QString& str) { m_endingStr.at(i) = std::move(str); }

    inline bool isInverted() const                          { return m_invertDirection; }
    inline void setIsInverted(const bool invertedDirection) { m_invertDirection = invertedDirection; }

    inline CharTypeToGet getCharTypeToGet() const             { return m_charTypeToGet; }
    inline void setCharTypeToGet(CharTypeToGet charTypeToGet) { m_charTypeToGet = charTypeToGet; }

    inline const QString& getToAllow() const { return m_toAllow; }
    inline void setToAllow(QString toAllow)  { m_toAllow = toAllow; }

    inline const QString& getToAvoid() const       { return m_toAvoid; }
    inline void setToAvoid(const QString& toAvoid) { m_toAvoid = toAvoid; }

    inline const QString& getToReplace() const         { return m_toReplace; }
    inline void setToReplace(const QString& toReplace) { m_toReplace = toReplace; }

    inline const QString& getReplaceFor() const          { return m_replaceFor; }
    inline void setReplaceFor(const QString& replaceFor) { m_replaceFor = replaceFor; }

    // SERIALIZATION
    void serialize(std::ofstream& out)  const override;
    void deserialize(std::ifstream& in) override;
};

#endif // CFUNCTIONCLASSES_H
