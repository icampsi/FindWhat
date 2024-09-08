#ifndef CPARSEDFILEMODEL_H
#define CPARSEDFILEMODEL_H

#include <QAbstractItemModel>
class CParsedFile;

namespace Qt { class QVector; }
class CExportCSV;

class CParsedFileModel : public QAbstractItemModel {
public:
    explicit CParsedFileModel(CExportCSV &relatedExpCSV, QObject *parent = nullptr);

    // Required method: Number of rows in the model
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // Required method: Number of columns in the model
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // Required method: Data at a specific index
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void appendData(const QString &value);
    bool removeData(const QModelIndexList &selectedIndexes);

    // Required method: Return index for the given row, column, and parent
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    // Required method: Return the parent index of the given index
    QModelIndex parent(const QModelIndex &index) const override;

    // Optional: Header data (for row and column headers)
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    CExportCSV& m_relExpCSV; // Files associated with the document
};

#endif // CPARSEDFILEMODEL_H
