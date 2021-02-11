#ifndef IPWDIALOG_H
#define IPWDIALOG_H

#include <QDialog>

class Model;

namespace Ui {
class IPWDialog;
}

class IPWDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IPWDialog(QWidget *parent = nullptr);
    ~IPWDialog();
    void SetModel(Model *model);


private slots:
    void on_buttonShowData_clicked();

private:
    Ui::IPWDialog *ui;
    Model* m_model;
};

#endif // IPWDIALOG_H
