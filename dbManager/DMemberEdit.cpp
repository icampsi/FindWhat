#include "DMemberEdit.h"
#include "ui_DMemberEdit.h"

#include <QFormLayout>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QSqlError>

DMemberEdit::DMemberEdit(QSqlTableModel *model, const QModelIndex &modelIndex,  QWidget *parent)
    : QDialog(parent), ui(new Ui::DMemberEdit), m_model{model}, m_index{modelIndex}
{
    QBoxLayout *Layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    // QFormLayout *formLayout = new QFormLayout(this);
    setLayout(Layout);

    // Initialize the table widget
    m_table = new QTableWidget(this);
    m_table->setColumnCount(2);
    m_table->setHorizontalHeaderLabels(QStringList() << "Field" << "Value");
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->setEditTriggers(QAbstractItemView::AllEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);

    // Populate the table with data from the record
    int rowCount = 0;
    QSqlRecord rec = model->record(m_index.row());

    rowCount = rec.count();
    m_table->setRowCount(rowCount);
    for (int i = 0; i < rowCount; ++i) {
        QTableWidgetItem *fieldItem = new QTableWidgetItem(rec.fieldName(i));
        fieldItem->setFlags(fieldItem->flags() & ~Qt::ItemIsEditable);  // Make the field name non-editable
        QTableWidgetItem *valueItem = new QTableWidgetItem(rec.value(i).toString());
        m_table->setItem(i, 0, fieldItem);
        m_table->setItem(i, 1, valueItem);
    }

    // Calculate the appropriate maximum height for the table
    int rowHeight = m_table->verticalHeader()->defaultSectionSize();
    int headerHeight = m_table->horizontalHeader()->height();
    int maxTableHeight = rowHeight * rowCount + headerHeight + 2; // 2 added to avoid scrollbar to appear

    // Set the maximum height for the table
    m_table->setMaximumHeight(maxTableHeight);

    // Add the table to the form layout
    Layout->addWidget(m_table);
    m_table->show();

    // Add OK and Cancel buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &DMemberEdit::submit);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_table, &QTableWidget::cellChanged, this, &DMemberEdit::updateModel);
    Layout->addWidget(buttonBox);

    int maxWindowHeight = maxTableHeight + buttonBox->height() + 25;
    setFixedHeight(maxWindowHeight);
    setMaximumWidth(300);
}

DMemberEdit::~DMemberEdit() {
    delete m_table;
}

bool DMemberEdit::updateRecord() {
    // Submit changes to the database
    if (m_model->submitAll()) {
        qDebug() << "Row modified successfully!";
        accept();
        return true;
    } else {
        qDebug() << "Error modifying row:" << m_model->lastError().text();
        return false;
    }
}

void DMemberEdit::updateModel(int row, int column) {
    const QString value = m_table->item(row, column)->text();
    QModelIndex index = m_model->index(m_index.row(), row);
    m_model->setData(index, value); // Set new data
}

void DMemberEdit::submit() {
    if(updateRecord()) accept();
    else qDebug() << "Coulnd't update record";
}
