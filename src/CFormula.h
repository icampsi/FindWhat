#ifndef CFORMULA_H
#define CFORMULA_H

#include "CData.h"
#include "CFunctionClasses.h"
#include "qdebug.h"



// EXTREU UNA ÚNICA DADA A PARTIR DELS PARÀMETRES DONATS
class CFormula {
public:
    struct IndexPosition {
        int initial = 0;
        int final   = 0;
    };
protected:

    QString       m_result = "";
    CData    	  m_data;
    IndexPosition m_indexPosition;
    std::vector<CFunction*> m_formulaPath; // succeció de CFunction que defineixen el recorregut que farà la fòrmula per extreure la dada.

public:
    // CONSTRUCTORS AND DESTRUCTORS
    CFormula(QString dataName) : m_data(dataName, this) {}
    CFormula() = default;
    CFormula(const CFormula& other);
    ~CFormula();

    // OPERATORS
    CFormula& operator=(const CFormula& other); // Assignment operator to support assignment between CFormula instances

    // GETTERS AND SETTERS
    QString         getResult()   { return m_result; }
    QString         getDataName() { return m_data.getDataName();}
    CData          *getData()     { return &m_data; }

    void            setDataName(const QString& dataName) { m_data.setDataName(dataName); }
    IndexPosition   getIndexPosition() { return m_indexPosition; }
    CFunction*      getFunction(int index){ return m_formulaPath[index]; }
    size_t          getPathSize() { return m_formulaPath.size(); }
    void            setIndexPosition(int initial, int final) { m_indexPosition = {initial, final}; }

    // FORMULA FUNCTIONS
    QString applyFormula(QString& text, unsigned int from = 0, int to = -1);

    inline int  findText(QString& text , CIndexingFunction* pFunctionToApply);
    inline void moveIndex(QString& text, CIndexingFunction* pFunctionToApply) {
        m_indexPosition.initial += pFunctionToApply->getNum();
        m_indexPosition.final    = m_indexPosition.initial;
    }
    inline void moveLine(QString& text, CIndexingFunction* pFunctionToApply);
    inline void BeginLine(QString& text); // Col·loca l'index al principi de la línia actual
    inline void EndLine(QString& text); // Col·loca l'index al final de la línia actual

    inline void appendString(CIndexingFunction* pFunctionToApply) { //Appends or prepends data to m_result
        if(!pFunctionToApply->getOption()) m_result.append(pFunctionToApply->getText());
        else                              m_result.prepend(pFunctionToApply->getText());
    }
    inline void appendData(CIndexingFunction* pFunctionToApply, std::vector<CData>* thisContainer);

    inline bool MathData(CMathFunction* pMathFunctionToApply, std::vector<CData>* thisContainer);

    void extractData(QString& text, CExtractingFunction* pFunctionToApply);
    void extractDataInverted(QString& text, CExtractingFunction* pFunctionToApply);

    // PATH INTERFACE
    void addFunction(CFunction* function) {
        function->setParent(this);
        m_formulaPath.push_back(function);
    }
    void deleteFunction(int index) {
        delete m_formulaPath[index];
        m_formulaPath.erase(m_formulaPath.begin() + index);
    }

    // Moves object from one index to another
    void reorderFunctionPath(int objectToMoveIndex, int destinationIndex) {
        if (objectToMoveIndex < 0 || objectToMoveIndex >= m_formulaPath.size() ||
            destinationIndex < 0 || destinationIndex >= m_formulaPath.size()) {
            qDebug() << "Invalid object index or destination index";
            return;
        }

        if (objectToMoveIndex == destinationIndex) {
            // No need to move if the indices are the same
            return;
        }

        if (objectToMoveIndex < destinationIndex) {
            // Move forward: Move the element at objectToMoveIndex to destinationIndex,
            // shifting the elements between them to the left
            std::rotate(m_formulaPath.begin() + objectToMoveIndex,
                        m_formulaPath.begin() + objectToMoveIndex + 1,
                        m_formulaPath.begin() + destinationIndex + 1);
        } else {
            // Move backward: Move the element at objectToMoveIndex to destinationIndex,
            // shifting the elements between them to the right
            std::rotate(m_formulaPath.begin() + destinationIndex,
                        m_formulaPath.begin() + objectToMoveIndex,
                        m_formulaPath.begin() + objectToMoveIndex + 1);
        }
    }
    void clearPath() { m_formulaPath.clear();}
};

#endif // CFORMULA_H
