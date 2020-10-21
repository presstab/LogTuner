#ifndef INJECTORDIALOG_H
#define INJECTORDIALOG_H

#include <QDialog>

namespace Ui {
class InjectorDialog;
}

class Model;

class InjectorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InjectorDialog(QWidget *parent = nullptr);
    ~InjectorDialog();
    void SetModel(Model* model);

private slots:
    void on_buttonCalculate_clicked();

private:
    Ui::InjectorDialog *ui;
    Model* m_model;
};

#endif // INJECTORDIALOG_H
