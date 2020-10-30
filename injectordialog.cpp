#include "injectordialog.h"
#include "ui_injectordialog.h"

#include "injectortuner.h"
#include "model.h"

#include <QMessageBox>

InjectorDialog::InjectorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InjectorDialog)
{
    ui->setupUi(this);

    ui->tablewidgetLatency->setRowCount(2);
    ui->tablewidgetLatency->setColumnCount(4);
    ui->tablewidgetLatency->horizontalHeader()->hide();
    ui->tablewidgetLatency->setVerticalHeaderLabels({"Volts", "Latency (ms)"});
}

InjectorDialog::~InjectorDialog()
{
    delete ui;
}

void InjectorDialog::SetModel(Model *model)
{
    m_model = model;
}

void InjectorDialog::on_buttonCalculate_clicked()
{
    //Make sure scale is set
    int32_t nInjectorScalar = ui->spinboxLatency->value();
    if (nInjectorScalar <= 0) {
        QMessageBox msg;
        msg.setText("Injector Scale must be set before calculating changes.");
        msg.exec();
        return;
    }

    //Get the entered in injector latency data
    std::vector<std::pair<double, double>> vLatency; // {{volts => latency}}
    for (int column = 0; column < ui->tablewidgetLatency->columnCount(); column++) {
        QString qstrVolts = ui->tablewidgetLatency->item(0, column)->text();
        double dVolts = qstrVolts.toDouble();
        if (dVolts < 0 || qFuzzyCompare(0, dVolts)) {
            //Invalid or null data entered
            QMessageBox msg;
            QString str = QString("Volts for column %1 is invalid").arg(column);
            msg.setText(str);
            msg.exec();
            return;
        }

        QString qstrLatency = ui->tablewidgetLatency->item(1, column)->text();
        double dLatency = qstrLatency.toDouble();
        if (dLatency < 0 || qFuzzyCompare(0, dLatency)) {
            //Invalid or null data entered
            QMessageBox msg;
            QString str = QString("Latency for column %1 is invalid").arg(column);
            msg.setText(str);
            msg.exec();
            return;
        }

        vLatency.emplace_back(std::make_pair(dVolts, dLatency));
    }

    //Get the entered injector small IPW data
    std::vector<std::pair<double, double>> vSmallIpwCompensations;
    for (int column = 0; column < ui->tablewidgetSmallIPW->columnCount(); column++) {
        //Get the pulse widths
        double dPulseWidth = ui->tablewidgetSmallIPW->item(0, column)->text().toDouble();
        double dCompensation = ui->tablewidgetSmallIPW->item(1, column)->text().toDouble();
        vSmallIpwCompensations.emplace_back(std::make_pair(dPulseWidth, dCompensation));
    }

    //Add the current inputs to the current model
    m_model->SetInjectorScale(nInjectorScalar);
    m_model->SetInjectorLatency(vLatency);
    m_model->SetSmallIpwCompensations(vSmallIpwCompensations);

    //Process suggested tuning changes
    InjectorTuner tuner(m_model);
    tuner.CalculateTune(/*fOpenLoop*/false);
}
