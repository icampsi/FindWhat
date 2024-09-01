#ifndef CPARSEDFILE_H
#define CPARSEDFILE_H

#include "CPagedText.h"

#include <QFileInfo>
#include <unordered_map>
#include <QString>
#include <optional>

class CEsquema;
class QSString;

class CParsedFile {
public:
    explicit CParsedFile(const QString& filePath, CEsquema **assignedEsquema)
        : m_filePath(filePath), m_assignedEsquema{assignedEsquema} {}

    // Rename file
    bool rename(const QString& newName);

    const QString& filePath() const { return m_filePath; }
    QString fileName() const { return QFileInfo(m_filePath).fileName(); }

    bool parseFileValues();

    // Using std::optional to handle non-existent fields
    const std::optional<QString> getValue(const QString& fieldName) const;

private:
    QString     m_filePath; // Relation to the pdf document that we are parsing and storing.
    std::unordered_map<QString, QString> m_fieldValueMap; // Maps field to value
    CEsquema **m_assignedEsquema;
};

#endif // CPARSEDFILE_H
