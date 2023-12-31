#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QFileDialog>

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

    public:
        explicit SettingsDialog(QWidget *parent = nullptr);
        ~SettingsDialog();
        QString getLibraryPath();
        void setLibraryPath(QString path);

    private:
        Ui::SettingsDialog *ui;

    private slots:
        void buttonFindPath();
};

#endif // SETTINGSDIALOG_H
