#ifndef GENERALFUNCTIONS_H
#define GENERALFUNCTIONS_H
#include <QString>
#include <QWidget>

QString getUserHomeDirectory();

QString parseFromText(const QString& text);

QString parseToText(const QString& text);

QWidget* getLastParent(QWidget* widget);

#endif // GENERALFUNCTIONS_H
