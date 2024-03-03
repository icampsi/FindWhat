#ifndef CFORMULA_H
#define CFORMULA_H

#include "CFunctionClasses.h"
#include "CData.h"

// EXTREU UNA ÚNICA DADA A PARTIR DELS PARÀMETRES DONATS
class CFormula {
public:
    struct IndexPosition {
        int initial = 0;
        int final   = 0;
    };
    enum class FunctionType : int { Indexing, Extracting, Math };  // Used only for serialization purposes

protected:
    QString       m_result = "";
    CData    	  m_data;
    IndexPosition m_indexPosition;
    std::vector<CFunction*> m_formulaPath; // succeció de CFunction que defineixen el recorregut que farà la fòrmula per extreure la dada.

public:
    // CONSTRUCTORS AND DESTRUCTORS
    CFormula(QString dataName) : m_data(dataName, this) {}
    CFormula() : m_data("", this) {}
    CFormula(std::ifstream& in) : m_data(in, this) { CFormula::deserialize(in); }
    CFormula(const CFormula& other);
    ~CFormula();

    // OPERATORS
    CFormula& operator=(const CFormula& other); // Assignment operator to support assignment between CFormula instances

    // GETTERS AND SETTERS
    QString         getResult() const   { return m_result; }
    QString         getDataName() const { return m_data.getDataName();}
    CData          *getData()           { return &m_data; }

    void            setDataName(const QString& dataName) { m_data.setDataName(dataName); }

    IndexPosition   getIndexPosition() const                 { return m_indexPosition; }
    void            setIndexPosition(int initial, int final) { m_indexPosition = {initial, final}; }

    CFunction*      getFunction(int index) const { return m_formulaPath[index]; }
    size_t          getPathSize() const          { return m_formulaPath.size(); }

    // FORMULA FUNCTIONS
    QString applyFormula(const QString &text, unsigned int from = 0, int to = -1);

    inline int  findText(const QString &text, CIndexingFunction* pFunctionToApply);
    inline void moveIndex(const QString& text, CIndexingFunction* pFunctionToApply);
    inline void moveLine(const QString &text, CIndexingFunction* pFunctionToApply);
    inline void BeginLine(const QString& text); // Sets index at the begining of current line
    inline void EndLine(const QString&   text); // Sets index at the begining of current line

    inline void appendString(CIndexingFunction* pFunctionToApply) { //Appends or prepends data to m_result
        if(!pFunctionToApply->getOption()) m_result.append(pFunctionToApply->getText());
        else                               m_result.prepend(pFunctionToApply->getText());
    }

    inline void appendData(CIndexingFunction* pFunctionToApply, std::vector<CData>* thisContainer);
    inline bool MathData(CMathFunction* pMathFunctionToApply, std::vector<CData>* thisContainer);

    void extractData(const QString &text, CExtractingFunction* pFunctionToApply);
    void extractDataInverted(const QString &text, CExtractingFunction* pFunctionToApply);

    // PATH INTERFACE
    void addFunction(CFunction* function) {
        function->setParent(this);
        m_formulaPath.push_back(function);
    }

    void deleteFunction(const int index);

    // Moves object from one index to another
    void reorderFunctionPath(int objectToMoveIndex, int destinationIndex);
    void clearPath() { m_formulaPath.clear();}

    void serialize(std::ofstream& out) const;
    void deserialize(std::ifstream& in);
};

#endif // CFORMULA_H
