# FindWhat
FindWhat is a Qt-powered program that provides tools for users to create paths or methods to extract changing values from a series of PDF files (such as numbers within different bills) and output them to a CSV file in any desired order.

THIS PROGRAM IS CURRENTLY UNDER CONSTRUCTION and some functionalities may not work, but it already fulfills its main purpose. Currently, I am working on serialization so that the created Esquemes do not need to be recreated each time. Also, keep in mind that this is my first time using QT and I'm learning on the spot.

*This program is part of a larger application that initially allows users to extract data from PDFs (using FindWhat), followed by sorting this data to generate customized .rtf and .pdf bills, which are then sent to a list of email addresses. Currently, I am utilizing both programs for a housing cooperative. We receive 32 monthly utility bills from various companies and need to extract diverse values from them to create 24 bills, one for each house, and subsequently send them via email. The bill creation program is currently inflexible and tailored specifically to our case. However, I plan to enhance its flexibility, like I'm doing with FindWhat, and eventually make it publicly available on GitHub. This would enable other housing cooperatives or individuals with similar needs to utilize it freely.

# Dependencies:
- [Poppler](https://poppler.freedesktop.org/) (for converting PDF files into text)
- [QT](https://www.qt.io)

# How to use
The program is not yet intuitive and does not provide much guidance on how to use it. To get started, experiment with it and keep the following points in mind:

- This program is initially designed to extract variable values from bills, such as numbers, dates, or any other information that needs to be extracted and stored in a CSV format.

- Esquemas are the main document objects. They allow you to construct a path (formulas) for each variable. Each formula contains instructions (functions) that you define to find text, navigate through the document, extract the variable, etc. You should work with constants that are consistent in every PDF to reach the desired index and extract the data from there.

- Esquemas also allow you to specify values that are constant and do not need to be searched for inside the PDF. For example, if you need your CSV to have a cell with the name of the company, which remains constant, you can create static data instead of using a formula. (The Preview docked window should help you keep track of your functions for the selected PDF file on the file browser docked window.)

- Once you have set up your Esquemas, you can configure the resulting CSV file on the "parse document" tab. Add pages to link an Esquema to the documents it needs to work on and specify the cell order by writing the name of each formula or static data, separated by commas.

# Example usage:
- Create an Esquema named "esquemaName".

- Add static Data named "NIF" with the value "F12345". (When added the data will be "Unnamed data". You can rename it by double clicking on it, or delete it by pressing "delete" key while the item is selected.)

- Add a formula named "Total" and write a path to find the total amount of the bill. For example, add a function "Find" to locate the text in the upper line, then add "move line" to move one line down, followed by "Extract Data" up until the end of the line (\n, or until a space " ", etc.), and finally, perhaps append " €" at the end of the extracted string.

- You can add more Esquemas or more PDFs, or repeat the same Esquema with different cell formatting, etc.

- Finally, click "Export CSV" and wait until every PDF is parsed correctly.

- Go to "Parse Document", add a page, and link it to the Esquema with the combo box. Then, underneath in the text edit, write "<NIF>,<Total>,Some Fixed Text etc." and drag and drop every bill that matches these specifications (for example, drop 20 PDF files you received from the same company, which are all the same except for the Total amount of the bill and some other numbers).

- You'll get a .csv file with 20 rows, each with 3 cells, stating "F12345", "Whatever-Total-was-extracted-from-the-pdf", and "Some Fixed Text".

# Latest updates
21/03/24
- Fixed Serialization
- Fixed a couple of things that werent updating when using multiple ending strings
- Improved performance when using multiple ending strings. Still got a way to go.

20/03/24
- CPdfDoc has been remade entirely to be able to handle multiple pdf pages separately. This has been used to add an option to look for data on a specific page and to improve Preview window to be able to display the document broke in multiple pages.
Note: Because of that, serialization temporaly doesen't work properly.
- Improved extraction mechanism to be able to handle multiple ending strings (so user can now specify different ending strings and extraction wont stop until each one has been found. This way user can say, for example, that extraction will keep going until 3 '\n' are found).
- Display textBox now shows formating symbols, so the user can know if the current index is on an end-of-line character.
- Added a way to replace extracted characters (so now user can, for example, say that each extracted whitespace should be a "-").
- Started a basic view menu to toggle visibility of docked windows.
- Some minor code clean up.

15/03/2024
- Switched from qmake to CMake build system.
- Cleaned code and made some re-estructuring.
- Fixed minor bugs.
  
03/03/2024
- Improved Serialization and serialization UI.
- Added a way to rename files.
- Fixed minor bugs.
- Cleaned code.

01/03/2024
- Added serialization features to save and load esquemes merging serialization branch. Still needs a bit of pulishing.
- Added option to delete loaded esquemas (press "del" on selected esquema).
- Fixed some minor bugs.
- Cleaned code.

# Planned future updates
- A feature to Relocate or copy parsed PDF files to another folder.
- Add a check to see if a file you want to load is corrupted so the program doesen't crash if you do so.
- Functionality to ensure that no two formulas or static data have the same name (right now it's a mess if you do that).
- Additional functions to enhance capabilities related to extracting variables from the pdf.
- Implementation of a verification mechanism to ensure the accuracy of extracted variables, with user notification for possible discrepancies.
- Improvement of the CSV structure definition method to mitigate user errors.
- Integration of tooltips, text placeholders, and other user-friendly navigation aids.
- Visual enchancing

# Licencing
Copyright (c) 2024 Ignasi Camps

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.

This program uses the Qt framework, which is © The Qt Company Ltd. and/or its subsidiaries and is available under the terms of the GNU General Public License (GPL) version 3, as published by the Free Software Foundation. See https://www.qt.io/licensing/ for more information.

This program uses poppler for pdf to text conversion, which is available under the terms of the GNU General Public License (GPL) version 3,
as published by the Free Software Foundation. See https://poppler.freedesktop.org/ for more information.
