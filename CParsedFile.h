/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef CPARSEDFILE_H
#define CPARSEDFILE_H

#include "CPagedText.h"

#include <QFileInfo>
#include <QString>
#include <optional>
#include <QCryptographicHash>

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
    const QHash<QString, QString>& getFields() const { return m_fieldValueMap; }


    QString computePdfHash(const QString &pdfText) {
        QByteArray byteArray = pdfText.toUtf8();  // Convert QString to QByteArray using UTF-8 encoding
        return QCryptographicHash::hash(byteArray, QCryptographicHash::Sha256).toHex();
    }

    const QString& getPdfHash() const { return m_pdfHash; }

private:
    QString     m_filePath; // Relation to the pdf document that we are parsing and storing.
    QHash<QString, QString> m_fieldValueMap; // Maps field to value
    CEsquema **m_assignedEsquema;
    QString     m_pdfHash;
};

#endif // CPARSEDFILE_H
