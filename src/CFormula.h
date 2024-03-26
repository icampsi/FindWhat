/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CFORMULA_H
#define CFORMULA_H

#include "CData.h"
#include "src/CFunctionClasses.h"

class CPdfDoc;

// EXTRACTS A DATA FROM GIVEN PARAMETERS (each formula extracts only one value)
class CFormula {
public:
    struct IndexPosition { // Marks the first and last current position of the searched text index
        size_t initial = 0;
        size_t final   = 0;
    };
    struct Result { // Stores the current index and extracted text
        IndexPosition indexPosition;
        QString result{""};
    };
    enum class FunctionType : int { Indexing, Extracting, Math };  // Used only for serialization purposes

protected:
    Result m_result;
    CData  m_data;
    std::vector<CFunction*> m_formulaPath; // Marc the path to find the variable data

public:
    // CONSTRUCTORS AND DESTRUCTORS
    CFormula(const QString& dataName) : m_data(dataName, this) {}
    CFormula() : m_data("", this) {}
    CFormula(std::ifstream& in) : m_data(in, this) { CFormula::deserialize(in); }
    CFormula(const CFormula& other);
    ~CFormula();

    // OPERATORS
    CFormula& operator=(const CFormula& other); // Assignment operator to support assignment between CFormula instances

    // GETTERS AND SETTERS
    const Result&  getResult() const { return m_result; }
    CData         *getData()         { return &m_data; }

    const QString& getDataName() const        { return m_data.getDataName();}
    void setDataName(const QString& dataName) { m_data.setDataName(dataName); }

    CFunction* getFunction(int index) const { return m_formulaPath[index]; }
    size_t     getPathSize() const          { return m_formulaPath.size(); }

    // FORMULA FUNCTIONS
    const Result& applyFormula(CPdfDoc* pPdfDoc, size_t from = 0, int to = -1, Result* halfWayResult = nullptr);

    int  findText (CPdfDoc *pPdfDoc, CIndexingFunction* pFunctionToApply);
    void moveIndex(CPdfDoc *pPdfDoc, CIndexingFunction* pFunctionToApply);
    void moveLine (CPdfDoc *pPdfDoc, CIndexingFunction* pFunctionToApply);
    void BeginLine(CPdfDoc *pPdfDoc); // Sets index at the begining of current line
    void EndLine  (CPdfDoc *pPdfDoc); // Sets index at the begining of current line
    void appendString(CIndexingFunction* pFunctionToApply) { //Appends or prepends string to m_result
        if(!pFunctionToApply->getOption()) m_result.result.append(pFunctionToApply->getText());
        else                               m_result.result.prepend(pFunctionToApply->getText());
    }
    void appendData(CIndexingFunction* pFunctionToApply, std::vector<CData>* thisContainer);
    bool MathData(CMathFunction* pMathFunctionToApply);

    void extractData(CPdfDoc* pPdfDoc, CExtractingFunction* pFunctionToApply);

    // m_formulaPath INTERFACE
    void addFunction(CFunction* function);
    void deleteFunction(const size_t index);
    void reorderFunctionPath(const size_t objectToMoveIndex, const size_t destinationIndex); // Moves object from one index to another
    void clearPath() { m_formulaPath.clear(); }

    // SERIALIZATION
    void serialize(std::ofstream& out) const;
    void deserialize(std::ifstream& in);
};

#endif // CFORMULA_H
