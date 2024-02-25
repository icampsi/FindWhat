/*
 * FindWhat - by Ignasi Camps
 *
 * Copyright (c) 2024 Ignasi Camps
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

/*
 * This program uses the Qt framework, which is © The Qt Company Ltd. and/or its subsidiaries
 * and is available under the terms of the GNU General Public License (GPL) version 3,
 * as published by the Free Software Foundation. See https://www.qt.io/licensing/ for more information.
*/

/*
 * This program uses poppler for pdf to text conversion, which is available under the terms of the GNU General Public License (GPL) version 3,
 * as published by the Free Software Foundation. See https://poppler.freedesktop.org/ for more information.
*/

#include "ui/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mainWindow w;
    w.showMaximized(); // Show the main window in full screen

    w.show();
    return a.exec();
}
