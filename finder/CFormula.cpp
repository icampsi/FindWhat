/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#include "CFormula.h"

#include <QDebug>
#include <fstream>

#include "document/CPdfDoc.h"

// Copy constructor
CFormula::CFormula(const CFormula& other) : m_data(other.m_data) {
    // Copy simple members
    m_result        = other.m_result;

    // Deep copy of the formula path vector
    for (CFunction *function : other.m_formulaPath) {
        CFunction *castedFunction = dynamic_cast<CIndexingFunction*>(function);

        // BOOKMARK - Probably need to rethink this bit
        if(castedFunction) {
            CIndexingFunction *indexFunction = new CIndexingFunction(*dynamic_cast<CIndexingFunction*>(function));
            indexFunction->setParent(this);
            m_formulaPath.push_back(indexFunction);
        }
        if(!castedFunction) {
            castedFunction = dynamic_cast<CExtractingFunction*>(function);
            if(castedFunction) {
                CExtractingFunction *extractFunction = new CExtractingFunction(*dynamic_cast<CExtractingFunction*>(function));
                extractFunction->setParent(this);
                m_formulaPath.push_back(extractFunction);
            }
        }
        if(!castedFunction){
            castedFunction = dynamic_cast<CMathFunction*>(function);
            if(castedFunction) {
                CMathFunction *mathFunction = new CMathFunction(*dynamic_cast<CMathFunction*>(function));
                mathFunction->setParent(this);
                m_formulaPath.push_back(mathFunction);
            }
        }
        if(!castedFunction) throw "couldn't cast";
    }
}

CFormula& CFormula::operator=(const CFormula& other) {
    if (this != &other) { // Check for self-assignment
        // Copy simple members
        m_data          = other.m_data;
        m_result        = other.m_result;

        // Deep copy of the formula path vector
        // Clear the existing formula path
        for (CFunction* function : m_formulaPath) {
            delete function;
        }
        m_formulaPath.clear();

        // Perform deep copy of the formula path from 'other'
        for (CFunction* function : other.m_formulaPath) {
            // Perform deep copy based on function type
            CFunction* newFunction = nullptr;
            if (auto indexFunction = dynamic_cast<CIndexingFunction*>(function)) {
                newFunction = new CIndexingFunction(*indexFunction);
            } else if (auto extractFunction = dynamic_cast<CExtractingFunction*>(function)) {
                newFunction = new CExtractingFunction(*extractFunction);
            } else if (auto mathFunction = dynamic_cast<CMathFunction*>(function)) {
                newFunction = new CMathFunction(*mathFunction);
            }
            if (newFunction) {
                newFunction->setParent(this);
                m_formulaPath.push_back(newFunction);
            }
        }
    }
    return *this;
}

CFormula::~CFormula() {
    for (CFunction* function : m_formulaPath) {
        if (function) delete function;
    }
}

const CFormula::Result& CFormula::applyFormula(const CPagedText* pPagedText, size_t from, int to, CFormula::Result *halfWayResult) {
    const QString text = pPagedText->getFullText(); // NEW BOOKMARK

    m_result.result.clear(); // Reset result
    if(m_formulaPath.size() == 0) return m_result; // If there are no functions loaded, we have reseted the result value and stop here.

    // Check if 'from' is within the range of formula path
    if (from > m_formulaPath.size() - 1) {
        // Handle out-of-range error
        qWarning() << "applyFormula: 'from' index is out of range.";
        m_result.result = "ERROR EXTRACTING VALUES";
        return m_result;
    }

    if (to < 0) {
        to = static_cast<int>(m_formulaPath.size()) - 1; // Allow "go through the full path" if to is negative
    }

    // Check if 'to' index is out of range
    if (to >= static_cast<int>(m_formulaPath.size())) {
        qWarning() << "applyFormula: 'to' index is out of range.";
        m_result.result = "ERROR EXTRACTING VALUES";
        return m_result;
    }
    size_t formulaPathSz = m_formulaPath.size();
    bool shouldBreak = false;
    for (size_t i = from ; i < formulaPathSz; i++) {
        if(i == from) {
            m_result.indexPosition = {0, 0};
        }

        CIndexingFunction* pIndexFun = dynamic_cast<CIndexingFunction*>(m_formulaPath[i]);
        CModFunction *pModFun = dynamic_cast<CModFunction*>(m_formulaPath[i]);
        CConditionFunction *pCondFun = dynamic_cast<CConditionFunction*>(m_formulaPath[i]);
        switch (m_formulaPath[i]->getFunctionType()) {
        case CFunction::Function::Find:
            if (pIndexFun) {
                if (findText(pPagedText, pIndexFun) == -1) return m_result;
            }
            break;

        case CFunction::Function::MoveIndex:
            if (pIndexFun) { moveIndex(text, pIndexFun); }
            break;

        case CFunction::Function::MoveLine:
            if (pIndexFun) { moveLine(text, pIndexFun); }
            break;

        case CFunction::Function::AppendString:
            appendString(pIndexFun); // I DONT LIKE IT BEING INDEXING FUNCTION. CREATE A SUBCLASS FOR APPENDING/SUBSTRACTING STRINGS?
            break;

        case CFunction::Function::ModifyResult:
            if(pModFun) replaceString(pModFun);
            break;
            
        case CFunction::Function::Condition:
            if(pCondFun) {
                switch (doCondition(text, pCondFun)) {
                    using Act = CConditionFunction::Action;
                case Act::DoNothing:
                    break;
                case Act::Jump:
                    if(i + pCondFun->getNJump() < formulaPathSz && pCondFun->getNJump() > 1) {
                        i += pCondFun->getNJump() - 1;
                    }
                    else if (i != formulaPathSz - 1) {
                        i = formulaPathSz - 2; // If we want to jump farther away than possible, we actually just jump to the last one.
                    }
                    break;
                case Act::Stop:
                    shouldBreak = true;
                    break;
                }
            }
            break;

        case CFunction::Function::ExtractData:
            CExtractingFunction* pExctracFun = static_cast<CExtractingFunction*>(m_formulaPath[i]);
            extractData(text, pExctracFun);
            break;

        }

        // Check if we reached "to" and update halfwayResult if so.
        if(halfWayResult) {
            if(static_cast<int>(i) == to) *halfWayResult =  m_result;
        }
        if(shouldBreak) break;
    }
    //thisContainer	= nullptr;
    m_data.setDataString(m_result.result);
    return m_result;
}

int CFormula::findText(const CPagedText *pagedText, CIndexingFunction* pFunctionToApply) {
    size_t indexPos_final   = m_result.indexPosition.final;
    size_t indexPos_initial = m_result.indexPosition.initial;
    // Initial check for indexPos. If final > initial last function was extracting, so indexes need to be brought together.
    if (indexPos_final > indexPos_initial) indexPos_initial = indexPos_final;

    QString text("");
    size_t relativeIndexInitial = indexPos_initial;
    size_t relativeIndexFinal   = indexPos_final;

    if (pFunctionToApply->getStartFromBeggining()) {
        relativeIndexInitial = 0;
        relativeIndexFinal   = 0;
    }

    // Set up what pages to look for the text
    int pageToLook = pFunctionToApply->getNum() - 1;
    if(pageToLook >= static_cast<int>(pagedText->pageCount())) pageToLook = -1; // If the page number is bigger than the total pages, look full document.

    if(pageToLook < 0) { // if negative looks the entire document
       text = pagedText->getFullText();
    }
    else {
        text = pagedText->getPage(pageToLook).pageText;
        pagedText->calculateComprehensiveIndex(relativeIndexInitial, pageToLook);
        pagedText->calculateComprehensiveIndex(relativeIndexFinal, pageToLook);
    }

    // TRUE = end text. FALSE = begin text
    const std::vector<QString>& textBlock = pFunctionToApply->getTextBlock();
    // Find text. It keeps iterating through the vector until one of the texts are found,
    // thus, text strings will only be looked for if previous ones aren't there
    for (auto iter = textBlock.begin(); iter != textBlock.end(); ++iter) {
        const QString& textToFind = *iter;
        if (textToFind.isEmpty()) continue;

        qsizetype textIndex{0};
        if(!pFunctionToApply->getGoBackwards()) {
            textIndex = text.indexOf(textToFind, indexPos_initial);     // Look forward
        } else {
            size_t i = text.lastIndexOf(textToFind, indexPos_initial);
            textIndex = i == 0 ? i : i - 1; // Look backwards. We start 1 position before, except if we are at 0.
        }
        if (textIndex != -1) { // if found
            relativeIndexInitial = textIndex;

            if (pFunctionToApply->getOption()) {
                relativeIndexInitial += textToFind.length();
            }

            relativeIndexFinal = relativeIndexInitial;
            break;
        }
        else {
            qDebug() << "Couldn't find string '" << textToFind << "' for: '" << m_data.getDataName() << "'";
            // return negative if no strings where found in the text
            if (iter == textBlock.end() - 1) {
                qDebug() << "None of the supplyed strings where found for: '" << m_data.getDataName() << "'";
                return -1;
            }
        }
    }

    if(pageToLook >= 0) {
        relativeIndexInitial += pagedText->getPage(pageToLook).pageCharRange.from;
        relativeIndexFinal   += pagedText->getPage(pageToLook).pageCharRange.from;
    }
    m_result.indexPosition.initial = relativeIndexInitial;
    m_result.indexPosition.final   = relativeIndexFinal;

    return 0;
}

void CFormula::moveIndex(const QString& text, CIndexingFunction* pFunctionToApply) {
    // Initial check for indexPos. If final > initial last function was extracting, so indexes need to be brought together.
    if (m_result.indexPosition.final > m_result.indexPosition.initial) m_result.indexPosition.initial = m_result.indexPosition.final;

    // Perform an initial check for index position. If final > initial means last function was extracting function and indexes need to be brought together now.
    size_t newIndex = m_result.indexPosition.initial + pFunctionToApply->getNum();
    // Check if the new index is within bounds
    if (newIndex >= 0 && newIndex < static_cast<size_t>(text.size())) {
        m_result.indexPosition.initial = newIndex;
        m_result.indexPosition.final   = m_result.indexPosition.initial;
    }
    else {
        qDebug() << "New index out of bounds";
    }
}

void CFormula::moveLine(const QString& text, CIndexingFunction* pFunctionToApply) {
    // Initial check for indexPos. If final > initial last function was extracting, so indexes need to be brought together.
    if (m_result.indexPosition.final > m_result.indexPosition.initial) m_result.indexPosition.initial = m_result.indexPosition.final;

    int linesToMove = pFunctionToApply->getNum();
    // Case when we go back X linesToMove
    if (linesToMove < 0) {
        while (m_result.indexPosition.initial != 0) {
            m_result.indexPosition.initial--;
            if (text[m_result.indexPosition.initial] == '\n') {
                linesToMove++;
            }
            if (linesToMove == 1) {
                m_result.indexPosition.initial++;
                break;
            }
        }
        m_result.indexPosition.final = m_result.indexPosition.initial;
    }
    else { // Case when we go up X linesToMove
        while (1) {
            if (m_result.indexPosition.initial < static_cast<size_t>(text.size())) { m_result.indexPosition.initial++; }
            else {
                BeginLine(text);
                break;
            }
            if (text[m_result.indexPosition.initial] == '\n') {
                linesToMove--;
            }
            if (linesToMove == 0) {
                m_result.indexPosition.initial++;
                break;
            }
        }
        m_result.indexPosition.final = m_result.indexPosition.initial;
    }
    if(pFunctionToApply->getOption()) EndLine(text);
}

void CFormula::BeginLine(const QString& text) { // Moves index to the beggining of current line
    // Initial check for indexPos. If final > initial last function was extracting, so indexes need to be brought together.
    if (m_result.indexPosition.final > m_result.indexPosition.initial) m_result.indexPosition.initial = m_result.indexPosition.final;

    while (m_result.indexPosition.initial != 0) {
        m_result.indexPosition.initial--;
        if (text[m_result.indexPosition.initial] == '\n') {
            m_result.indexPosition.initial++;
            break;
        }
    }
    // Since this is an indexing function, we don't want the final index to be different than the starting index
    m_result.indexPosition.final = m_result.indexPosition.initial;
}

void CFormula::EndLine(const QString& text) { // Moves index to the ending of current line
    // Initial check for indexPos. If final > initial last function was extracting, so indexes need to be brought together.
    if (m_result.indexPosition.final > m_result.indexPosition.initial) m_result.indexPosition.initial = m_result.indexPosition.final;

    while (m_result.indexPosition.initial < static_cast<size_t>(text.size())) {
        m_result.indexPosition.initial++;
        if (text[m_result.indexPosition.initial] == '\n') {
            break;
        }
    }
    // Since this is an indexing function, we don't want the final index to be different than the starting index
    m_result.indexPosition.final = m_result.indexPosition.initial;
}

void CFormula::appendString(CIndexingFunction* pFunctionToApply) { //Appends or prepends string to m_result
    if(!pFunctionToApply->getOption()) m_result.result.append(pFunctionToApply->getText());
    else                               m_result.result.prepend(pFunctionToApply->getText());
}

void CFormula::appendData(CIndexingFunction* pFunctionToApply, std::vector<CData>* thisContainer) {
    for (size_t i = 0; i < thisContainer->size(); ++i) {
        if (thisContainer->at(i).getDataName() == pFunctionToApply->getText()) {
            m_result.result.append(thisContainer->at(i).getDataString());
        }
    }
}


// Functions related to CMathFunction are still unused and not working
bool CFormula::MathData(CMathFunction* pMathFunctionToApply) {

    QString sortedVal2;
    QString unitatsStrVal1{ "0" };
    QString decimalsStrVal1{ "0" };
    // Eliminem les comes dels dos valors
    bool comaAmountVal1 = false; // Conta quantes comes o punts hi ha. Si n'hi ha més d'una s'invalida la funció.
    bool numStart = false; // Serveix per a marcar quan es troba el primer digit. D'aquesta manera podem ignorar tot el que hi hagi abans del primer número i no ens trobarem amb coses com que una coma mal psoada al principi elimini tot el número per exces de comes
    for (unsigned int i{ 0 }; i < pMathFunctionToApply->m_val1.size(); i++) {
        if (!numStart) {
            if (pMathFunctionToApply->m_val1[i].isDigit()) {
                unitatsStrVal1 += pMathFunctionToApply->m_val1[i];
                numStart = true;
            }
        }
        else {
            if (pMathFunctionToApply->m_val1[i] == ',' || pMathFunctionToApply->m_val1[i] == '.') {
                if (!comaAmountVal1) comaAmountVal1 = true;
                else return false; // Si hi ha massa comes la funció retorna false
            }
            else {
                if (pMathFunctionToApply->m_val1[i].isDigit() && comaAmountVal1 == false) unitatsStrVal1 += pMathFunctionToApply->m_val1[i];
                else if (pMathFunctionToApply->m_val1[i].isDigit() && comaAmountVal1 == false) decimalsStrVal1 += pMathFunctionToApply->m_val1[i];
                else break; // Ara que ja tenim algun número, qualsevol simbol que no sigui coma ni número s'entendrà que el número que busquem ja s'ha acabat i es truncarà la funció.
            }
        }
    }
    bool comaAmountVal2 = false;

    for (unsigned int i{ 0 }; i < pMathFunctionToApply->m_val2.size(); i++) {
        if (pMathFunctionToApply->m_val2[i] == ',' || pMathFunctionToApply->m_val2[i] == '.') {
            sortedVal2 += '.';
            if (!comaAmountVal2) comaAmountVal2 = true;
            else return false; // Si hi ha massa comes la funció retorna false
        }
        else {
            if (pMathFunctionToApply->m_val2[i] == ',' || pMathFunctionToApply->m_val2[i] == '.') {
                sortedVal2 += '.';
                if (!comaAmountVal2) comaAmountVal2 = true;
                else return false; // Si hi ha massa comes la funció retorna false
            }
            else {
                if (pMathFunctionToApply->m_val2[i].isDigit() && comaAmountVal1 == false) unitatsStrVal1 += pMathFunctionToApply->m_val2[i];
                else if (pMathFunctionToApply->m_val2[i].isDigit() && comaAmountVal1 == false) decimalsStrVal1 += pMathFunctionToApply->m_val2[i];
                else break; // Ara que ja tenim algun número, qualsevol simbol que no sigui coma ni número s'entendrà que el número que busquem ja s'ha acabat i es truncarà la funció.
            }
        }
    }

    //if (comaAmountVal1) {
    //	for (unsigned i{ 0 }; i < pMathFunctionToApply->m_val1.length(); i++) {
    //		if (pMathFunctionToApply->m_val1[i] == ',') break;
    //		else unitats
    //	}
    //}

    switch(pMathFunctionToApply->m_operation)
    {
    case CMathFunction::Operation::add:
        m_result.result += pMathFunctionToApply->m_val1 + pMathFunctionToApply->m_val2;
        break;
    case CMathFunction::Operation::subtract:
        break;
    case CMathFunction::Operation::divide:
        break;
    case CMathFunction::Operation::multiply:
        break;
    }
    return true;
}

CConditionFunction::Action CFormula::doCondition(const QString& text, CConditionFunction *pFunctionToApply) {
    bool flag = false;
    QString &result = m_result.result;
    switch(pFunctionToApply->getOperator()) {
        using Op = CConditionFunction::Operator;
    case Op::Empty:
        flag = result.isEmpty();
        break;
    case Op::NotEmpty:
        flag = !result.isEmpty();
        break;
    case Op::Equal:
        flag = (result == pFunctionToApply->getCompared());
        break;
    case Op::NotEqual:
        flag = (result != pFunctionToApply->getCompared());
        break;
    case CConditionFunction::Operator::Found:
        flag = (text.contains(pFunctionToApply->getCompared()));
        break;
    case CConditionFunction::Operator::NotFound:
        flag = !(text.contains(pFunctionToApply->getCompared()));
        break;
    }

    if(flag) return pFunctionToApply->getAction();
    else     return CConditionFunction::Action::DoNothing;
}

void CFormula::extractData(const QString& text, CExtractingFunction* pFunctionToApply) {
    if(pFunctionToApply->getEndingStringBlock().empty()) return; // NEW BOOKMARK

    bool directionInverted = pFunctionToApply->isInverted(); // Flag to check wether we should extract upwards or backwards

    // Initial check for indexPos. If final > initial last function was extracting, so indexes need to be brought together.
    if (m_result.indexPosition.final > m_result.indexPosition.initial) m_result.indexPosition.initial = m_result.indexPosition.final;
    if(directionInverted && m_result.indexPosition.final > 0) {
        m_result.indexPosition.final--; // If direction is inverted we start from one char before.
        m_result.indexPosition.initial--;
    }
    bool allowed{ false };       // flag to mark if the character is allowed m_toAllow;
    bool avoided{ false };       // flag to mark if the character is to avoid m_toAvoid;
    int extractedAmount{ 0 };    // Counter for how many characters we have extracted already: m_charsToGet
    std::vector<QString> endingString = pFunctionToApply->getEndingStringBlock(); // Strings that marks the ending of the extraction
    QString extractedText;       // Temp storage for the extracted text
    size_t endingStringIndex{0}; // Counter for the ending str block
    QString currentEndingString(""); // Ending str in use

    // Initialize currentEndingString
    if(!endingString.empty()) {
        currentEndingString = endingString.at(0);
    }

    // EXTRACTION LOOP - limited by chars to read and to get and broken if last ending string reached
    while(pFunctionToApply->getCharsToRead() != 0 && pFunctionToApply->getCharsToGet()  != 0) {
        // Check text bounds (current index +/- ending str length needs to be inside text bounds)
        int remainingText{ 0 };
        if(directionInverted) {
            remainingText = static_cast<int>(m_result.indexPosition.final - currentEndingString.length() + 1);
        } else {
            remainingText = static_cast<int>(m_result.indexPosition.final + currentEndingString.length() - 1);
        }

        if(remainingText >= 0 && remainingText < text.length()) {
            // Is currentEndingString reached.
            const QString& nextText(text.mid(m_result.indexPosition.final, currentEndingString.length()));

            if(( directionInverted && nextText == currentEndingString) ||
               (!directionInverted && nextText == currentEndingString))
            {
                // Check next one or break if there are no more
                if(endingStringIndex < endingString.size() - 1) {
                    ++endingStringIndex;
                    currentEndingString = endingString.at(endingStringIndex);
                }
                else break;
            }
        }

        pFunctionToApply->setCharsToRead(pFunctionToApply->getCharsToRead() - 1); // m_charsToRead
        // CHECK IF IT'S ONE OF THE ALLOWED CHARACTERS: m_toAllow
        QString toAllow = pFunctionToApply->getToAllow();
        if (toAllow.size() > 0) {
            for (short i{ 0 }; i < toAllow.size(); i++) {
                if (text[m_result.indexPosition.final] == toAllow[i]) {
                    allowed = true;
                    break;
                }
            }
        }
        // CHECK IF IT'S ONE OF THE AVOIDED CHARACTERS: m_toAvoid
        QString toAvoid = pFunctionToApply->getToAvoid();
        if (toAvoid.size() > 0) {
            for (unsigned short i{ 0 }; i < toAvoid.size(); i++) {
                if (text[m_result.indexPosition.final] == toAvoid[i]) {
                    avoided = true;
                    break;
                }
            }
        }

        // Actual extraction if all conditions are met
        switch (pFunctionToApply->getCharTypeToGet()) {
        case CExtractingFunction::CharTypeToGet::digit:
            if ((text[m_result.indexPosition.final].isDigit() && !avoided) || allowed) {
                if(!directionInverted) extractedText.append(text[m_result.indexPosition.final]);
                else                                extractedText.prepend(text[m_result.indexPosition.final]);
                extractedAmount++;
            }
            break;
        case CExtractingFunction::CharTypeToGet::letter:
            if ((!text[m_result.indexPosition.final].isDigit() && !avoided) || allowed) {
                if(!directionInverted) extractedText.append(text[m_result.indexPosition.final]);
                else                                extractedText.prepend(text[m_result.indexPosition.final]);
                extractedAmount++;
            }
            break;
        case CExtractingFunction::CharTypeToGet::all:
            if (!avoided) {
                if(!directionInverted) extractedText.append(text[m_result.indexPosition.final]);
                else                   extractedText.prepend(text[m_result.indexPosition.final]);
                extractedAmount++;
            }
        }
        // Break if extracted ammount reached
        if (pFunctionToApply->getCharsToGet() == extractedAmount) {
            m_result.indexPosition.final++; // Place index after the extracted string
            break;
        }

        // Reset flags
        allowed = false;
        avoided = false;

        // Update index
        if(!directionInverted && static_cast<int>(m_result.indexPosition.final) < text.length() - 1) {
            m_result.indexPosition.final++;
        }
        else if (directionInverted && m_result.indexPosition.final > 0) {
            m_result.indexPosition.final--;
        }
        else break;
    }

    // Relplace extracted text if needed
    extractedText.replace(pFunctionToApply->getToReplace(), pFunctionToApply->getReplaceFor());

    // Update result and index
    m_result.result.append(std::move(extractedText));
    if(directionInverted) {
        m_result.indexPosition.initial = m_result.indexPosition.final - 1;
        m_result.indexPosition.final =  m_result.indexPosition.initial;
    }
}

void CFormula::addFunction(CFunction* function) {
    function->setParent(this);
    m_formulaPath.push_back(function);
}

void CFormula::deleteFunction(const size_t index) {
    // Check if the index is within bounds
    if (index >= 0 && index < m_formulaPath.size()) {
        // Check if the pointer at the specified index is valid
        if (m_formulaPath[index]) {
            // Delete function from vector
            delete m_formulaPath[index];
            m_formulaPath.erase(m_formulaPath.begin() + index);
        } else {
            qDebug() << "Pointer at index" << index << "is null";
        }
    } else {
        qDebug() << "Index" << index << "is out of bounds";
    }
}

void CFormula::reorderFunctionPath(const size_t objectToMoveIndex, const size_t destinationIndex) {
    if (objectToMoveIndex >= m_formulaPath.size() || destinationIndex >= m_formulaPath.size()) {
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

void CFormula::serialize(std::ofstream& out) const {
    /*  - SERIALIZATION ORDER -
     *  CData                   m_data
     *  size_t                  vector m_formulaPath size
     *  FunctionType            type
     *  std::vector<CFunction*> m_formulaPath
     *
     *  NO NEED
     *  m_result
     *  m_indexPosition
     */

    m_data.serialize(out);                                                             // m_data
    size_t formulaPathSize = m_formulaPath.size();
    out.write(reinterpret_cast<const char*>(&formulaPathSize), sizeof(size_t));        // size of formulaPath

    for (CFunction* function : m_formulaPath) {                                        // FunctionTye & m_formulaPath
        FunctionType type;
        if (CIndexingFunction* indexingFunction = dynamic_cast<CIndexingFunction*>(function)) {
            type = FunctionType::Indexing;
            out.write(reinterpret_cast<const char*>(&type), sizeof(FunctionType));
            indexingFunction->serialize(out);
        } else if (CExtractingFunction* extractingFunction = dynamic_cast<CExtractingFunction*>(function)) {
            type = FunctionType::Extracting;
            out.write(reinterpret_cast<const char*>(&type), sizeof(FunctionType));
            extractingFunction->serialize(out);
        } else if (CMathFunction* mathFunction = dynamic_cast<CMathFunction*>(function)) {
            type = FunctionType::Math;
            out.write(reinterpret_cast<const char*>(&type), sizeof(FunctionType));
            mathFunction->serialize(out);
        } else if (CModFunction* modFunction = dynamic_cast<CModFunction*>(function)) {
            type = FunctionType::Mod;
            out.write(reinterpret_cast<const char*>(&type), sizeof(FunctionType));
            modFunction->serialize(out);
        } else if (CConditionFunction *condFunction = dynamic_cast<CConditionFunction*>(function)) {
            type = FunctionType::Condition;
            out.write(reinterpret_cast<const char*>(&type), sizeof(FunctionType));
            condFunction->serialize(out);
        } else {
            qDebug() << "Function pointer not valid";
            continue;
        }
    }
}

void CFormula::deserialize(std::ifstream& in) {
    /*  - DESERIALIZATION ORDER -
     *
     *  CData    	  m_data
     *  std::vector<CFunction*> m_formulaPath
     *
     *  NO NEED
     *  - m_result
     */

    m_formulaPath.clear();

    size_t formulaPathSize;
    in.read(reinterpret_cast<char*>(&formulaPathSize), sizeof(size_t)); // Size of m_formulaPath
    for (size_t i{0}; i < formulaPathSize; i++) {                       // m_formulaPath
        FunctionType type;
        in.read(reinterpret_cast<char*>(&type), sizeof(FunctionType));

        switch (type) {
            case FunctionType::Indexing: {
                CIndexingFunction *indexingFunction = new CIndexingFunction(in, this);
                addFunction(indexingFunction);
                break;
            }
            case FunctionType::Extracting: {
                CExtractingFunction *extractingFunction = new CExtractingFunction(in, this);
                addFunction(extractingFunction);
                break;
            }
            case FunctionType::Math: {
                // CMathFunction *mathFunction = new CMathFunction(in, this); // Not used yet
                // addFunction(mathFunction);
                break;
            }
            case FunctionType::Mod: {
                CModFunction *modFunction = new CModFunction(in, this);
                addFunction(modFunction);
                break;
            }
            case FunctionType::Condition: {
                CConditionFunction *conditionFunction = new CConditionFunction(in, this);
                addFunction(conditionFunction);
                break;
            }
            default: {
                qDebug() << "Unknown function type during deserialization";
                break;
            }
        }
    }
}
