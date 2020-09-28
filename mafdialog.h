#ifndef MAFDIALOG_H
#define MAFDIALOG_H

#include <QDialog>

namespace Ui {
class MafDialog;
}

class Model;

class MafDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MafDialog(QWidget *parent = nullptr);
    ~MafDialog();
    std::map<uint32_t, uint32_t> GetMafScale() const;
    void SetModel(Model* model);

private:
    Ui::MafDialog *ui;
    Model* m_model;

public slots:
    void ExistingScaleContextMenu(const QPoint& pos);
    void ExistingScalePaste();

signals:
    void MafScaleChanged();
private slots:
    void on_buttonRescaleMaf_clicked();
};

#endif // MAFDIALOG_H
