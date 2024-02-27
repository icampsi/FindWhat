#ifndef GENERALFUNCTIONS_H
#define GENERALFUNCTIONS_H
#include <QString>
#include <QWidget>

QString getUserHomeDirectory();

// Allows for scape characters on the text edit widgets (right now only /n is supported since there shouldn't be a need for any other)
QString parseFromText(const QString& text);
QString parseToText(const QString& text);

// Helps retrieving the last parent of a widget
// In theory it should allways be QMainWindow in this app, but i am returning QWidget for  flexibility
QWidget* getLastParent(QWidget* widget);

#endif // GENERALFUNCTIONS_H
