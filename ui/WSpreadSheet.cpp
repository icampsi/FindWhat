#include "WSpreadSheet.h"
#include <QMessageBox>

WSpreadSheet::WSpreadSheet(QWidget *parent) :
    QTableView(parent), rowHeaderContextMenu(new QMenu(this)), columnHeaderContextMenu(new QMenu(this))
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTableView::customContextMenuRequested, this, &WSpreadSheet::showContextMenu);

    QAction *copyAction = new QAction("Copy", this);
    addAction(copyAction);
    connect(copyAction, &QAction::triggered, this, &WSpreadSheet::copySelectionToClipboard);

    QAction *cutAction = new QAction("Cut", this);
    addAction(cutAction);
    connect(cutAction, &QAction::triggered, this, &WSpreadSheet::cutSelectionToClipboard);

    QAction *pasteAction = new QAction("Paste", this);
    addAction(pasteAction);
    connect(pasteAction, &QAction::triggered, this, &WSpreadSheet::pasteFromClipboard);

    QAction *deleteAction = new QAction("Delete", this);
    addAction(deleteAction);
    connect(deleteAction, &QAction::triggered, this, &WSpreadSheet::deleteSelectedCells);


    // Enable custom context menu for row header
    verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(verticalHeader(), &QHeaderView::customContextMenuRequested, this, &WSpreadSheet::showRowHeaderContextMenu);

    // Enable custom context menu for column header
    horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(horizontalHeader(), &QHeaderView::customContextMenuRequested, this, &WSpreadSheet::showColumnHeaderContextMenu);

    // Insert row action
    QAction *insertRowBeforeAction = new QAction("Insert Row Before", this);
    QAction *insertRowAfterAction  = new QAction("Insert Row After", this);
    rowHeaderContextMenu->addAction(insertRowBeforeAction);
    rowHeaderContextMenu->addAction(insertRowAfterAction);
    addAction(insertRowBeforeAction);
    addAction(insertRowAfterAction);

    connect(insertRowBeforeAction, &QAction::triggered, this, [this]() {
        insertRow(true, currentIndex().row());
    });
    connect(insertRowAfterAction, &QAction::triggered, this, [this]() {
        insertRow(false, currentIndex().row());
    });

    // Delete row action
    QAction *deleteRowsAction = new QAction("Delete Selected Rows", this);
    rowHeaderContextMenu->addAction(deleteRowsAction);
    addAction(deleteRowsAction);

    connect(deleteRowsAction, &QAction::triggered, this, &WSpreadSheet::delSelectedRows);

    // Add actions to the column header context menu
    columnHeaderContextMenu->addAction("Add Column", this, &WSpreadSheet::addCol);
    columnHeaderContextMenu->addAction("Delete Selected Columns", this, &WSpreadSheet::delSelectedColumns);

    // Insert column action
    QAction *insertColBeforeAction = new QAction("Insert Column Before", this);
    QAction *insertColAfterAction  = new QAction("Insert Column After", this);
    columnHeaderContextMenu->addAction(insertColBeforeAction);
    columnHeaderContextMenu->addAction(insertColAfterAction);
    addAction(insertColBeforeAction);
    addAction(insertColAfterAction);

    connect(insertColBeforeAction, &QAction::triggered, this, [this]() {
        insertCol(true, currentIndex().column());
    });
    connect(insertColAfterAction, &QAction::triggered, this, [this]() {
        insertCol(false, currentIndex().column());
    });
}

void WSpreadSheet::VFit() {
    // int rowCount = this->model()->rowCount();
    // int sectionSize = verticalHeader()->defaultSectionSize();
    // int rowHeight   = sectionSize * 2;
    // if(rowCount > 0) rowHeight = sectionSize * (rowCount + 1);
    // setMaximumHeight(rowHeight);
}

void WSpreadSheet::HFit() {
    // int columnCount = this->model()->columnCount();
    // int sectionSize = horizontalHeader()->defaultSectionSize();
    // int columnHeight = sectionSize * 2;
    // if (columnCount > 0) columnHeight = sectionSize * (columnCount + 1);
    // setMaximumWidth(columnHeight);
}

void WSpreadSheet::keyPressEvent(QKeyEvent *event) {
    if (event->matches(QKeySequence::Copy)) {
        copySelectionToClipboard();
    } else if (event->matches(QKeySequence::Cut)) {
        cutSelectionToClipboard();
    } else if (event->matches(QKeySequence::Paste)) {
        pasteFromClipboard();
    } else if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        deleteSelectedCells();
    } else {
        QTableView::keyPressEvent(event);
    }
}

void WSpreadSheet::showContextMenu(const QPoint &pos) {
    QMenu contextMenu(tr("Context menu"), this);
    contextMenu.addAction("Cut"   , this, &WSpreadSheet::cutSelectionToClipboard);
    contextMenu.addAction("Copy"  , this, &WSpreadSheet::copySelectionToClipboard);
    contextMenu.addAction("Paste" , this, &WSpreadSheet::pasteFromClipboard);
    contextMenu.addAction("Delete", this, &WSpreadSheet::deleteSelectedCells);
    contextMenu.exec(mapToGlobal(pos));
}

void WSpreadSheet::copySelectionToClipboard() {
    // Retrieve a list of all selected indexes (cells)
    QModelIndexList indexes = this->selectionModel()->selectedIndexes();

    if (indexes.isEmpty())
        return;

    // Sort the selected indexes first by row, then by column
    std::sort(indexes.begin(), indexes.end(), [](const QModelIndex &a, const QModelIndex &b) {
        if (a.row() == b.row()) {
            return a.column() < b.column();
        }
        return a.row() < b.row();
    });

    QString selectedText; // will hold the copied text
    int currentRow = indexes.first().row(); // Row of the first selected index

    bool isNewLine = true; // flag to check if we are changing line
    // Iterate over each selected index
    for (const QModelIndex &index : indexes) {
        // Check if the current index is in a new row
        if (index.row() != currentRow) {
            // Add a newline character to move to the next row in the output string
            selectedText.append('\n');
            // Update the current row tracker
            currentRow = index.row();
            isNewLine = true;
        }
        if(!isNewLine) {
            selectedText.append('\t');
        }
        isNewLine = false;
        // Append the data from the current cell to the output string
        selectedText.append(index.data().toString());
    }

    // Access the system clipboard
    QClipboard *clipboard = QApplication::clipboard();
    // Set the copied text to the clipboard
    clipboard->setText(selectedText);
}

void WSpreadSheet::cutSelectionToClipboard() {
    copySelectionToClipboard();
    deleteSelectedCells();
}

void WSpreadSheet::pasteFromClipboard() {
    QString clipboardText = QApplication::clipboard()->text();

    QModelIndexList indexes = this->selectionModel()->selectedIndexes();
    if (indexes.isEmpty())
        return;

    // Sort indexes by row and then by column
    std::sort(indexes.begin(), indexes.end(), [](const QModelIndex &a, const QModelIndex &b) {
        if (a.row() == b.row()) {
            return a.column() < b.column();
        }
        return a.row() < b.row();
    });

    QStringList rows = clipboardText.split('\n', Qt::SkipEmptyParts);
    int startRow = indexes.first().row();
    int startColumn = indexes.first().column();

    for (int i = 0; i < rows.size(); ++i) {
        QStringList columns = rows[i].split('\t');
        for (int j = 0; j < columns.size(); ++j) {
            if (startRow + i < this->model()->rowCount() && startColumn + j < this->model()->columnCount()) {
                QModelIndex pasteIndex = this->model()->index(startRow + i, startColumn + j);
                this->model()->setData(pasteIndex, columns[j]);
            }
        }
    }
}

void WSpreadSheet::deleteSelectedCells() {
    QModelIndexList indexes = this->selectionModel()->selectedIndexes();

    if (indexes.isEmpty())
        return;

    for (const QModelIndex &index : indexes) {
        this->model()->setData(index, QString(""));
    }
}

void WSpreadSheet::addRow() {
    QAbstractItemModel* model = this->model();
    if (!model) {
        qWarning() << "Model is null";
        return;
    }

    // Insert an empty row at the end of the table
    int rowCount = model->rowCount();
    model->insertRow(rowCount);

    // Resize
    VFit();
}

void WSpreadSheet::insertRow(bool before, int row) {
    QAbstractItemModel* model = this->model();
    if (!model) {
        qWarning() << "Model is null";
        return;
    }

    if (before) {
        model->insertRow(row);
    } else {
        model->insertRow(row + 1);
    }

    VFit();
}

void WSpreadSheet::delSelectedRows() {
    QAbstractItemModel* model = this->model();
    if (!model) {
        qWarning() << "Model is null";
        return;
    }

    // Get selected rows
    QModelIndexList selectedRows = this->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        qWarning() << "No rows selected";
        return;
    }

    // Avoid deletition of all rows
    bool allSelected = selectedRows.size() == model->rowCount();
    if(allSelected) {
        QMessageBox::warning(this, "Warning", "You cannot delete all rows in the table");
        return;
    }

    // Sort the rows in descending order
    std::sort(selectedRows.begin(), selectedRows.end(), [](const QModelIndex &a, const QModelIndex &b) {
        return a.row() > b.row();
    });

    // Remove rows in reverse order
    for (const QModelIndex &index : selectedRows) {
        model->removeRow(index.row());
    }

    VFit();
}

void WSpreadSheet::addCol() {
    QAbstractItemModel* model = this->model();
    if (!model) {
        qWarning() << "Model is null";
        return;
    }

    // Insert a new column at the end of the table
    int columnCount = model->columnCount();
    model->insertColumn(columnCount);

    HFit();
}

void WSpreadSheet::insertCol(bool before, int col) {
    QAbstractItemModel* model = this->model();
    if (!model) {
        qWarning() << "Model is null";
        return;
    }

    if (before) {
        model->insertColumn(col);
    } else {
        model->insertColumn(col + 1);
    }

    HFit();
}

void WSpreadSheet::delSelectedColumns() {
    QAbstractItemModel* model = this->model();
    if (!model) {
        qWarning() << "Model is null";
        return;
    }

    // Get selected columns
    QModelIndexList selectedColumns = this->selectionModel()->selectedColumns();
    if (selectedColumns.isEmpty()) {
        qWarning() << "No columns selected";
        return;
    }

    // Avoid deletition of all columns
    bool allSelected = selectedColumns.size() == model->columnCount();
    if(allSelected) {
        QMessageBox::warning(this, "Warning", "You cannot delete all columns in the table");
        return;
    }

    // Sort the columns in descending order
    std::sort(selectedColumns.begin(), selectedColumns.end(), [](const QModelIndex &a, const QModelIndex &b) {
        return a.column() > b.column();
    });

    // Remove columns in reverse order
    for (const QModelIndex &index : selectedColumns) {
        model->removeColumn(index.column());
    }

    HFit();
}

void WSpreadSheet::showRowHeaderContextMenu(const QPoint &pos) {
    rowHeaderContextMenu->exec(verticalHeader()->viewport()->mapToGlobal(pos));
}

void WSpreadSheet::showColumnHeaderContextMenu(const QPoint &pos) {
    columnHeaderContextMenu->exec(horizontalHeader()->viewport()->mapToGlobal(pos));
}
