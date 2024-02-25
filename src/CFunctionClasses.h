#ifndef CFUNCTIONCLASSES_H
#define CFUNCTIONCLASSES_H

#include <QString>

class CFormula;

class CFunction {
public:
    enum class FunctionType { Find, MoveIndex, MoveLine, AppendString, ExtractData };

protected:
    FunctionType m_functionType;
    QString      m_functionTypeName;
    CFormula*    m_parent = nullptr;

public:
    CFunction(FunctionType functionType, CFormula* parent = nullptr);
    virtual ~CFunction() {} // Virtual destructor

    virtual FunctionType getFunctionType();
    QString getFunctionTypeName();
    CFormula* getParent();
    void setParent(CFormula* formulaPtr);
};

class CMathFunction : public CFunction {
public:
    enum class Operation { add, subtract, divide, multiply };

    QString   m_val1{ "" };
    QString   m_val2{ "" };
    Operation m_operation{ Operation::add };

    CMathFunction(FunctionType name);
    CMathFunction(const CMathFunction &other);
    virtual ~CMathFunction() {} // Virtual destructor
};

class CIndexingFunction : public CFunction {
protected:
    QString m_text{ "" };
    int     m_num{ 0 };
    bool    m_option{ true };
    bool    m_startFromBeggining{ false };

public:
    QString getText() const              { return m_text; }
    void    setText(const QString& text) { m_text = text; }

    int  getNum()        { return m_num; }
    void setNum(int num) { m_num = num; }

    bool getOption()            { return m_option; }
    void setOption(bool option) { m_option = option; }

    bool getStartFromBeggining()            { return m_startFromBeggining; }
    void setStartFromBeggining(bool option) { m_startFromBeggining = option; }

    CIndexingFunction(FunctionType name);
    CIndexingFunction(FunctionType name, QString findText, int num = 0, bool option = false);
    CIndexingFunction(const CIndexingFunction &other);
    virtual ~CIndexingFunction() {} // Virtual destructor
};

class CExtractingFunction : public CFunction {
public:
    enum class CharTypeToGet { all, digit, letter };

protected:
    int          m_charsToGet{ -1 };       // If negative, ignored. Stop when this amount of chars have been extracted.
    int          m_charsToRead{ -1 };      // If negative, ignored. Stop when this amount of chars have been read.
    QString      m_endingString = "\n";     // When this string is found, stop reading. Can be a single character, e.g., \n, space, ., etc.
    bool         m_invertDirection{ false }; // Invert the direction in which the data is read.
    CharTypeToGet m_charTypeToGet = CharTypeToGet::all; // Specifies if only numbers, characters, or all should be obtained
    QString       m_toAllow{ "" };
    QString       m_toAvoid{ "" };

public:
    int  getCharsToGet() { return m_charsToGet; }
    void setCharsToGet(int charsToGet) { m_charsToGet = charsToGet; }

    int  getCharsToRead() { return m_charsToRead; }
    void setCharsToRead(int charsToRead) { m_charsToRead = charsToRead; }

    QString getEndingString() { return m_endingString; }
    void setEndingString(QString endingString) { m_endingString = endingString; }

    bool getInvertedDirection() { return m_invertDirection; }
    void setInvertedDirection(bool invertedDirection) { m_invertDirection = invertedDirection; }

    CharTypeToGet getCharTypeToGet() { return m_charTypeToGet; }
    void setCharTypeToGet(CharTypeToGet charTypeToGet) { m_charTypeToGet = charTypeToGet; }

    QString getToAllow() { return m_toAllow; }
    void setToAllow(QString toAllow) { m_toAllow = toAllow; }

    QString getToAvoid() { return m_toAvoid; }
    void setToAvoid(QString toAvoid) { m_toAvoid = toAvoid; }

    CExtractingFunction(FunctionType name);
    CExtractingFunction(const CExtractingFunction &other);
    virtual ~CExtractingFunction() {} // Virtual destructor
};

#endif // CFUNCTIONCLASSES_H
