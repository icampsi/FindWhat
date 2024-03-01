#ifndef CESQUEMA_H
#define CESQUEMA_H
#include <QString>

#include <QMessageBox>
#include <fstream>
#include "CData.h"
#include "CFormula.h"

// Guarda tota la informació necessària per a extreure cada un dels valors especificats d'un PDF i guardar-los com a CData
class CEsquema {
public:
    // Constructors and destructors
    CEsquema(const QString nameEsquema, std::vector<CFormula *> tExtractDataFormula, std::vector<CData*>& valorsEstatics, QString IDText);
    CEsquema(const QString nameEsquema, std::vector<CFormula *> tExtractDataFormula, QString IDText = "");
    CEsquema(std::ifstream& in) { deserliazile(in); }
    CEsquema() = default;
    ~CEsquema();

protected:
    //Members
    QString m_nameEsquema; // Name for the esquema
    QString m_IDText;      // Text que identifica el document

    std::vector<CFormula*> t_extractDataFormula;
    std::vector<CData*>    m_staticData;        // Static values for the esquema. They are not extracting text from a file but rather youcreate a variable with a fixed string attached
    std::vector<QString>   m_csvFormatFormula;  // { "", "data", "data", "num", "\n", "BI" } Especifica l'ordre en que es rpèsentaràn les dades al XSV. "\n" per salt de columna.

    std::vector<std::vector<CData*>> m_XSVStructureResult; // Stores the result as an xsv structure

    std::vector<unsigned int> m_dataForCheck1; // { 8, 9, 5} For the future. Índex dels valors que volem fer check. val1 + val2 == val 3? (molaria donar més opcions d'operacions complexes)
    std::vector<unsigned int> m_dataForCheck2; // For the future. Índex dels valors que volem fer check. L'últim és el valor a comparar.

    QMap<QString, CData*> m_dataMap; // This maps every CData structure with it's name for efficient lookup
    QString               m_fileNameFormula; // { "$6$_$2$_$4$_$1$.pdf" } Use $1$ for insert value 1. etc.
    QString               m_outputDirectori; // { "LLUM\\" }  // On es vol guardar l'arxiu

public:
    void inline formatDate(QString& data);  // Still unused
    void renameFile(const char* oldName, const char* newName);

    // Future updates:
    // void createFileData(QString& text, CFileData& file); // This is not used, it is part of an old idea and will probably be deleted
    // bool checkData(CFileData& file); // Still unused
    // void createFileName(CFileData& file);

    // Breaks a csv line into a vector to set up m_csvFormatFormula.
    void setCsvFormatFormula(const QString& rLine, char enclosureChar, char separator);
    void setCsvFormatFormula(const std::vector<QString>& format) { m_csvFormatFormula = format; }

    // Takes m_csvFormatFormula to find the variables and organise them insto a xsvm structure of Strings to be stored in m_XSVStructureResult
    void generateXSVStructure(QString &text);
    // ENCARA S'HA DE PROVAR. lI HE TRET EL VECTOR DE VECTORS DE CDATA COM A ARGUMENT I L'HE SUBSTITUIT PEL MEMBRE m_XSVStructureResult.
    void xsvm_structureToString(QString* pFullFileString, char enclosureChar, char separator);
    // Deletes the formula at index pos and cleans
    void deleteFormula(int index);

    // SETTERS AND GETTERS
    void addStaticData(CData* data );
    const std::vector<CData*>& getStaticData() const { return m_staticData; }
    void deleteStaticData(int index);
    void setStaticDataName(CData* data, QString name) {
        // Update m_dataMap
        QString oldName = data->getDataName();
        m_dataMap.insert(name, data);
        if (m_dataMap.contains(oldName)) {
            m_dataMap.take(oldName);        // Remove the old key-value pair and get the value
            m_dataMap.insert(name, data);   // Insert the value with the new key
        }
        data->setDataName(name);
    }

    const std::vector<CFormula*>& getExtractDataFormula() const         { return t_extractDataFormula; }
    void setExtractDataFormula(const std::vector<CFormula*>& formulas)  { t_extractDataFormula = formulas; }

    void addExtractDataFormula(CFormula* formula) {
        m_dataMap.insert(formula->getDataName(), formula->getData());
        t_extractDataFormula.push_back(formula);
    }

    void setFormulaName(CFormula* formula, QString name) {
        m_dataMap.insert(name, formula->getData());
        // Update m_dataMap
        QString oldName = formula->getDataName();
        if (m_dataMap.contains(oldName)) {
            m_dataMap.take(oldName);  // Remove the old key-value pair and get the value
            m_dataMap.insert(name, formula->getData()); // Insert the value with the new key
        }
        formula->setDataName(name);
    }

    void setIDText(const QString& idText)   { m_IDText = idText; }
    const QString& getIDText() const        { return m_IDText; }

    const QString& getName() const      { return m_nameEsquema; }
    void setName(const QString& name)   { m_nameEsquema = name; }

    const std::vector<std::vector<CData*>>& getXSVStructureResult() const { return m_XSVStructureResult; };

    void serialize(std::ofstream& out) const;
    void deserliazile(std::ifstream& in);
};

#endif // CESQUEMA_H
