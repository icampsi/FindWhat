#include "CFormula.h"
#include <QDebug>

void inline comaToDot(QString& textWithComa, QString& emptyStr) {
    emptyStr.clear();  // Clear the result string if it contains any previous content
    emptyStr.reserve(textWithComa.size());  // Reserve memory to avoid reallocations
    for (unsigned int i{ 0 }; i < textWithComa.size(); i++) {
        if (textWithComa[i] == ',') {
            emptyStr += '.';
        }
        else {
            emptyStr += textWithComa[i];
        }
    }
}
// Copy constructor
CFormula::CFormula(const CFormula& other) : m_data(other.m_data) {
    // Copy simple members
    m_result        = other.m_result;
    m_indexPosition = other.m_indexPosition;

    // Deep copy of the formula path vector
    for (CFunction* function : other.m_formulaPath) {
        CFunction* castedFunction = castedFunction = dynamic_cast<CIndexingFunction*>(function);

        // I am creating a new function for ensuring livespan of it. Probably need to rethink this part when i understand better how QStandarditem works
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
        m_indexPosition = other.m_indexPosition;

        // Deep copy of the formula path vector
        // Clear the existing formula path
        for (CFunction* function : m_formulaPath) {
            delete function;
        }
        m_formulaPath.clear();

        // Perform deep copy of the formula path from 'other'
        for (CFunction* function : other.m_formulaPath) {
            // Perform deep copy based on function type (similar to copy constructor)
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


// Destructor to release memory held by m_formulaPath
CFormula::~CFormula() {
    // Release memory held by CFunctionContent instances
    for (CFunction* function : m_formulaPath) {
        if (function) {
            delete function;
        }
    }
}

QString CFormula::applyFormula(QString& text,  unsigned int from, int to) {
    m_result = ""; // Reset result
    if(m_formulaPath.size() == 0) return m_result; // If there are no functions loaded, we have reseted the result value and stop here.
    if(to < 0) to = m_formulaPath.size() - 1; // so you can specify -1 as a "go throwgh the full path"
    if(from > m_formulaPath.size() - 1) { throw "out of range"; }

    for (unsigned int i = from ; i <= to; i++) {
        if(i == from) {
            m_indexPosition = {0, 0};
        }
        CIndexingFunction* pIndexingFunction = dynamic_cast<CIndexingFunction*>(m_formulaPath[i]);
        CMathFunction* pMathFunction = dynamic_cast<CMathFunction*>(m_formulaPath[i]);

        switch (m_formulaPath[i]->getFunctionType()) {
        case CFunction::FunctionType::Find:
            if (pIndexingFunction) {
                if (findText(text, pIndexingFunction) == -1) return m_result;
            }
            break;
        case CFunction::FunctionType::MoveIndex:
            if (pIndexingFunction) { moveIndex(text, pIndexingFunction); }
            break;
        case CFunction::FunctionType::MoveLine:
            if (pIndexingFunction) { moveLine(text, pIndexingFunction); }
            break;
        // case FunctionType::BeginLine:
        //     BeginLine(text);
        //     break;
        // case FunctionType::EndLine:
        //     EndLine(text);
        //     break;
        case CFunction::FunctionType::AppendString:
            appendString(pIndexingFunction); // I DONT LIKE IT BEING INDEXING FUNCTION. CREATE A SUBCLASS FOR APPENDING/SUBSTRACTING STRINGS?
            break;
        // case FunctionType::AppendData:
        //     //AppendData(pIndexingFunction, thisContainer); // std::vector<CData>* thisContainer as an argument for funciton call
        //     break;

        // case FunctionType::MathData:
        //     //MathData(pMathFunction, thisContainer); // std::vector<CData>* thisContainer as an argument for funciton call
        //     break;

        case CFunction::FunctionType::ExtractData:
            CExtractingFunction* pExctractingFunction = static_cast<CExtractingFunction*>(m_formulaPath[i]);
            if (!pExctractingFunction->getInvertedDirection()) { extractData(text, pExctractingFunction); }
            else { extractDataInverted(text, pExctractingFunction); }
            break;
        }
    }
    //thisContainer	= nullptr;
    m_data.setDataString(m_result);
    return m_result;
}

inline int CFormula::findText(QString& text, CIndexingFunction* pFunctionToApply) {
    if (pFunctionToApply->getStartFromBeggining()) m_indexPosition = {0, 0};
    // TRUE = end text. FALSE = begin text
    if (text.indexOf(pFunctionToApply->getText(), m_indexPosition.initial) != -1) {
        if (pFunctionToApply->getOption()) {
            m_indexPosition.initial = text.indexOf(pFunctionToApply->getText(), m_indexPosition.initial) + pFunctionToApply->getText().length();
        }
        else {
            m_indexPosition.initial = text.indexOf(pFunctionToApply->getText(), m_indexPosition.initial);
        }
        m_indexPosition.final = m_indexPosition.initial;
    }
    else {
        qDebug() << "Couldn't find string '" << pFunctionToApply->getText() << "' for: '" << m_data.getDataName() << "'";
        return -1;
    }
}

void CFormula::moveLine(QString& text, CIndexingFunction* pFunctionToApply) {
    int linesToMove = pFunctionToApply->getNum(); // utilitzem un número intern per evitar que canviï el valor dins CFormula, ja que formula ha de ser reutilitzable
    // Case when we go back X linesToMove
    if (linesToMove < 0) {
        while (m_indexPosition.initial != 0) {
            m_indexPosition.initial--;
            if (text[m_indexPosition.initial] == '\n') {
                linesToMove++;
            }
            if (linesToMove == 1) {
                m_indexPosition.initial++;
                break;
            }
        }
        m_indexPosition.final = m_indexPosition.initial;
    }
    else { // Case when we go up X linesToMove
        while (1) {
            if (m_indexPosition.initial < text.size()) { m_indexPosition.initial++; }
            else {
                BeginLine(text);
                break;
            }
            if (text[m_indexPosition.initial] == '\n') {
                linesToMove--;
            }
            if (linesToMove == 0) {
                m_indexPosition.initial++;
                break;
            }
        }
        m_indexPosition.final = m_indexPosition.initial;
    }
    if(pFunctionToApply->getOption()) EndLine(text);
}

inline void CFormula::BeginLine(QString& text) { // Moves index to the beggining of current line
    while (m_indexPosition.initial != 0) {
        m_indexPosition.initial--;
        if (text[m_indexPosition.initial] == '\n') {
            m_indexPosition.initial++;
            break;
        }
    }
    // Since this is an indexing function, we don't want the final index to be different than the starting index
    m_indexPosition.final = m_indexPosition.initial;
}

inline void CFormula::EndLine(QString& text) { // Moves index to the ending of current line
    while (m_indexPosition.initial < text.size()) {
        m_indexPosition.initial++;
        if (text[m_indexPosition.initial] == '\n') {
            m_indexPosition.initial--;
            break;
        }
    }
    // Since this is an indexing function, we don't want the final index to be different than the starting index
    m_indexPosition.final = m_indexPosition.initial;
}

inline void CFormula::appendData(CIndexingFunction* pFunctionToApply, std::vector<CData>* thisContainer) { // Col·loca l'index al final de la línia actual
    for (auto i = 0; i < thisContainer->size(); ++i) {
        if (thisContainer->at(i).getDataName() == pFunctionToApply->getText()) {
            m_result += thisContainer->at(i).getDataString();
        }
    }
}

inline bool CFormula::MathData(CMathFunction* pMathFunctionToApply, std::vector<CData>* thisContainer) {

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
    numStart = false;

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
        m_result += pMathFunctionToApply->m_val1 + pMathFunctionToApply->m_val2;
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

void CFormula::extractData(QString& text, CExtractingFunction* pFunctionToApply) {
    bool allowed{ false };      // flag to mark if the character is allowed m_toAllow;
    bool avoided{ false };      // flag to mark if the character is to avoid m_toAvoid;
    int extractedAmount{ 0 };   // Conta quants caracters hem llegit: m_charsToGet
    QString endingString = pFunctionToApply->getEndingString(); // String that marks the ending of the extraction

    for (;
         m_indexPosition.final < text.length()
         && pFunctionToApply->getCharsToRead() != 0
         && pFunctionToApply->getCharsToGet()  != 0;

         m_indexPosition.final++)
    {
        // Check if endingString reached. If it's empty, read to the end of the string.
        if (!endingString.isEmpty() && text.mid(m_indexPosition.final, endingString.length()) == endingString)  break; // Check if endingString reached. If it's empty, read to the end of the string.

        pFunctionToApply->setCharsToRead(pFunctionToApply->getCharsToRead() - 1); // m_charsToRead
        // CHECK IF IT'S ONE OF THE ALLOWED CHARACTERS: m_toAllow
        QString toAllow = pFunctionToApply->getToAllow();
        if (toAllow.size() > 0) { // Si hem posat caracters a l'string fa el loop, sino ja no s'ho mira
            for (short i{ 0 }; i < toAllow.size(); i++) {
                if (text[m_indexPosition.final] == toAllow[i]) {
                    allowed = true;
                    break;
                }
            }
        }
        // CHECK IF IT'S ONE OF THE AVOIDED CHARACTERS: m_toAvoid
        QString toAvoid = pFunctionToApply->getToAvoid();
        if (toAvoid.size() > 0) { // Si hem posat caracters a l'string fa el loop, sino ja no s'ho mira
            for (unsigned short i{ 0 }; i < toAvoid.size(); i++) {
                if (text[m_indexPosition.final] == toAvoid[i]) {
                    avoided = true;
                    break;
                }
            }
        }

        switch (pFunctionToApply->getCharTypeToGet()) {
        case CExtractingFunction::CharTypeToGet::digit:
            if ((text[m_indexPosition.final].isDigit() && !avoided) || (allowed)) {
                m_result.append(text[m_indexPosition.final]);
                extractedAmount++;
            }
            break;
        case CExtractingFunction::CharTypeToGet::letter:
            if ((!text[m_indexPosition.final].isDigit() && !avoided) || allowed) {
                m_result.append(text[m_indexPosition.final]);
                extractedAmount++;
            }
            break;
        case CExtractingFunction::CharTypeToGet::all:
            if (!avoided) {
                m_result.append(text[m_indexPosition.final]);
                extractedAmount++;
            }
        }
        if (pFunctionToApply->getCharsToGet() == extractedAmount) {
            m_indexPosition.final++; // Cal fer aquesta última suma per tal que l'index es situi passat el caracter extret, sino la següent funció començaria aqui i es podria extreure dos vegades el mateix caracter.
            break;
        }

        allowed = false;
        avoided = false;
    }
}

void CFormula::extractDataInverted(QString& text, CExtractingFunction* pFunctionToApply) {
    bool allowed{ false };      // flag to mark if the character is allowed m_toAllow;
    bool avoided{ false };      // flag to mark if the character is to avoid m_toAvoid;
    int extractedAmount{ 0 };   // Conta quants caracters hem llegit: m_charsToGet
    QString endingString = pFunctionToApply->getEndingString(); // String that marks the ending of the extraction

    for (;
         m_indexPosition.final > 0
         && pFunctionToApply->getCharsToRead() != 0
         && pFunctionToApply->getCharsToGet()  != 0;

         m_indexPosition.final--)
    {
        // Check if endingString reached. If it's empty, read to the end of the string.
        if (!endingString.isEmpty() && text.mid(m_indexPosition.final, endingString.length()) == endingString)  break;

        pFunctionToApply->setCharsToRead(pFunctionToApply->getCharsToRead() - 1); // m_charsToRead--
        // CHECK IF IT'S ONE OF THE ALLOWED CHARACTERS: m_toAllow
        QString toAllow = pFunctionToApply->getToAllow();
        if (toAllow.size() > 0) { // Si hem posat caracters a l'string fa el loop, sino ja no s'ho mira
            for (short i{ 0 }; i < toAllow.size(); i++) {
                if (text[m_indexPosition.final] == toAllow[i]) {
                    allowed = true;
                    break;
                }
            }
        }
        // CHECK IF IT'S ONE OF THE AVOIDED CHARACTERS: m_toAvoid
        QString toAvoid = pFunctionToApply->getToAvoid();
        if (toAvoid.size() > 0) { // Si hem posat caracters a l'string fa el loop, sino ja no s'ho mira
            for (unsigned short i{ 0 }; i < toAvoid.size(); i++) {
                if (text[m_indexPosition.final] == toAvoid[i]) {
                    avoided = true;
                    break;
                }
            }
        }

        switch (pFunctionToApply->getCharTypeToGet()) {
        case CExtractingFunction::CharTypeToGet::digit:
            if ((text[m_indexPosition.final].isDigit() && !avoided) || (allowed)) {
                m_result.prepend(text[m_indexPosition.final]);
                extractedAmount++;
            }
            break;
        case CExtractingFunction::CharTypeToGet::letter:
            if ((!text[m_indexPosition.final].isDigit() && !avoided) || allowed) {
                m_result.prepend(text[m_indexPosition.final]);
                extractedAmount++;
            }
            break;
        case CExtractingFunction::CharTypeToGet::all:
            if (!avoided) {
                m_result.prepend(text[m_indexPosition.final]);
                extractedAmount++;
            }
        }
        if (pFunctionToApply->getCharsToGet() == extractedAmount) break;

        allowed = false;
        avoided = false;
    }
}