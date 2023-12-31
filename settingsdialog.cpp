#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    connect(ui->pushButtonFindPath, SIGNAL(clicked(bool)), this, SLOT(buttonFindPath()));
    connect(ui->pushButtonCancel, SIGNAL(clicked(bool)), this, SLOT(reject()));
    connect(ui->pushButtonOk, SIGNAL(clicked(bool)), this, SLOT(accept()));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

QString SettingsDialog::getLibraryPath()
{
    return ui->lineEditPathTiLibrary->text();
}

void SettingsDialog::setLibraryPath(QString path)
{
    ui->lineEditPathTiLibrary->setText(path);
}

void SettingsDialog::buttonFindPath()
{
    ui->lineEditPathTiLibrary->setText(QFileDialog::getExistingDirectory());
}
