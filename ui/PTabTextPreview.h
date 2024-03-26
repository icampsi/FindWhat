/* =================================================== *
 * ====        Copyright (c) 2024 icampsi         ==== *
 * ==== SPDX-License-Identifier: GPL-3.0-or-later ==== *
 * =================================================== */

#ifndef PTABTEXTPREVIEW_H
#define PTABTEXTPREVIEW_H

#include <QTextEdit>
#include <QWidget>

class PTabTextPreview : public QTextEdit {
    Q_OBJECT

public:
    explicit PTabTextPreview(QWidget *parent = nullptr);
    explicit PTabTextPreview(const QString& text, QWidget *parent = nullptr);

    ~PTabTextPreview();

    void setPreviewText(const QString& text) { setPlainText(text); }

    // BOOKMARK - Needs real implementation
    void setShowWS_NL_T(const bool enable);
    bool showWS_NL_T_isEnabled() const { return m_showWS_NL_T; }

protected:
    // MEMBERS
    bool m_showWS_NL_T;
};

#endif // PTABTEXTPREVIEW_H
