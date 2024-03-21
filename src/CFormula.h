/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CFORMULA_H
#define CFORMULA_H

#include "CFunctionClasses.h"
#include "CData.h"

class CPdfDoc;
// EXTRACTS A DATA FROM GIVEN PARAMETERS (each formula extracts only one value)
class CFormula {
public:
    struct IndexPosition {
        size_t initial = 0;
        size_t final   = 0;
    };
    struct Result {
        IndexPosition indexPosition;
        QString result{""};
    };
    enum class FunctionType : int { Indexing, Extracting, Math };  // Used only for serialization purposes

protected:
    //QString       m_result = "";
    CData    	  m_data;
    Result m_result;
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
    Result          getResult() const   { return m_result; }
    QString         getDataName() const { return m_data.getDataName();}
    CData          *getData()           { return &m_data; }

    void            setDataName(const QString& dataName) { m_data.setDataName(dataName); }

    // Result   getIndexPosition() const                       { return m_indexPosition; }
    void            setIndexPosition(size_t initial, size_t final) { m_result.indexPosition.initial = initial; m_result.indexPosition.final = final;}

    CFunction*      getFunction(int index) const { return m_formulaPath[index]; }
    size_t          getPathSize() const          { return m_formulaPath.size(); }

    // FORMULA FUNCTIONS
    Result applyFormula(CPdfDoc* pPdfDoc, size_t from = 0, int to = -1, Result* halfWayResult = nullptr);

    inline int  findText (CPdfDoc *pPdfDoc, CIndexingFunction* pFunctionToApply);
    inline void moveIndex(CPdfDoc *pPdfDoc, CIndexingFunction* pFunctionToApply);
    inline void moveLine (CPdfDoc *pPdfDoc, CIndexingFunction* pFunctionToApply);
    inline void BeginLine(CPdfDoc *pPdfDoc); // Sets index at the begining of current line
    inline void EndLine  (CPdfDoc *pPdfDoc); // Sets index at the begining of current line
    inline void appendString(CIndexingFunction* pFunctionToApply);
    inline void appendData(CIndexingFunction* pFunctionToApply, std::vector<CData>* thisContainer);
    inline bool MathData(CMathFunction* pMathFunctionToApply);

    void extractData(CPdfDoc* pPdfDoc, CExtractingFunction* pFunctionToApply);

    // m_formulaPath INTERFACE
    void addFunction(CFunction* function) {
        function->setParent(this);
        m_formulaPath.push_back(function);
    }
    void deleteFunction(const size_t index);
    void reorderFunctionPath(size_t objectToMoveIndex, size_t destinationIndex); // Moves object from one index to another
    void clearPath() { m_formulaPath.clear();}

    // SERIALIZATION
    void serialize(std::ofstream& out) const;
    void deserialize(std::ifstream& in);
};

#endif // CFORMULA_H
