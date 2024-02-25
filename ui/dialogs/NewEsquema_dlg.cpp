#include <QAbstractButton>

#include "NewEsquema_dlg.h"
#include "ui/mainwindow.h"
#include "ui_NewEsquema_dlg.h"

#include "qmessagebox.h"

#include "document/CMDoc.h"

newEsquema_dlg::newEsquema_dlg(QWidget *parent, const std::vector<QString>& loadedEsquemaNames)
    : QDialog(parent), ui(new Ui::newEsquema_dlg), m_loadedEsquemaNames{loadedEsquemaNames}
{
    ui->setupUi(this);
}

newEsquema_dlg::~newEsquema_dlg() { delete ui; }

void newEsquema_dlg::on_buttonBox_accepted() {
    // Check if a name is been added
    if(ui->lineEdit_nameEsquema->text().isEmpty()) {
        QMessageBox::information(this, "Empty Name", "You need to specify a name for the esquema");
        return;
    }
    mainWindow* mainWin = dynamic_cast<mainWindow*>(parent());
    if(!mainWin) return;

    // Read fields values and store them inside the new esquema directly
    QString esquemaName = ui->lineEdit_nameEsquema->text().toUtf8();

    // Check if a document with the same name is already loaded
    auto it = std::find(m_loadedEsquemaNames.begin(), m_loadedEsquemaNames.end(), esquemaName);
    if (it != m_loadedEsquemaNames.end()) {
        QMessageBox::warning(this, "Already existing esquema", "An esquema with that name is already loaded, please, choose another name.");
        return;
    }

    // Creates and sets-up the document
    CEsquema* esquema = new CEsquema();
    esquema->setName(esquemaName);
    esquema->setIDText(ui->lineEdit_idString->text().toUtf8());

    // Creates a esquemaDoc, passes ownership of the esquema and stores it in the CMDoc singleton.
    CEsquemaDoc* esquemaDoc = CMDoc::getMDoc().newDoc(esquema);

    // Check and loads it to the ui
    if(esquemaDoc && esquema) mainWin->loadEsquema(esquemaDoc);
    else           qDebug() << "Failed to create CEsquemaDoc.";

    delete this;
}

void newEsquema_dlg::on_buttonBox_rejected() {
    delete this;
}

