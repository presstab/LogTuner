#include "ipwdialog.h"
#include "ui_ipwdialog.h"
#include "injectortuner.h"
#include "model.h"

IPWDialog::IPWDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IPWDialog)
{
    ui->setupUi(this);
}

IPWDialog::~IPWDialog()
{
    delete ui;
}
void IPWDialog::SetModel(Model *model)
{
    m_model = model;
}

void IPWDialog::on_buttonShowData_clicked()
{
    ui->tablewidgetResults->setRowCount(3);

    InjectorTuner tuner(m_model);
    std::vector<std::pair<int32_t, std::pair<int, double>>> vCorrections = tuner.CalculateSmallIpw();

    ui->tablewidgetResults->setRowCount(3);

    int column = 0;
    for (const std::pair<int32_t, std::pair<int, double>>& record : vCorrections) {
        const int32_t& nPulseWidth = record.first;
        const int& nRecordCount = record.second.first;
        const double& dAvgCorrection = record.second.second;

        ui->tablewidgetResults->insertColumn(column);

        QTableWidgetItem* itemPulseWidth = new QTableWidgetItem(QString::number(nPulseWidth));
        ui->tablewidgetResults->setItem(0, column, itemPulseWidth);
        QTableWidgetItem* itemCorrection = new QTableWidgetItem(QString::number(dAvgCorrection));
        ui->tablewidgetResults->setItem(1, column, itemCorrection);
        QTableWidgetItem* itemCount = new QTableWidgetItem(QString::number(nRecordCount));
        ui->tablewidgetResults->setItem(2, column, itemCount);

        column++;
    }
}

