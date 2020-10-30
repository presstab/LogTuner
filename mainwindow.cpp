#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "log.h"
#include "injectordialog.h"
#include "mafdialog.h"
#include "model.h"

#include <QFile>
#include <QFileDialog>

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_model = new Model();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_buttonMafScalling_clicked()
{
    MafDialog* dialog = new MafDialog(this);
    //connect(dialog, &MafDialog::MafScaleChanged, this, &MainWindow::MafScaleChanged);
    dialog->SetModel(m_model);
    dialog->show();
}

void MainWindow::MafScaleChanged()
{
    MafDialog* dialog = (MafDialog*)sender();
    auto mapMafScale = dialog->GetMafScale();
    m_model->SetMafScale(mapMafScale);
}

void MainWindow::on_buttonLoadLog_clicked()
{
    //Ask user to select the file
    QString qstrFileName = QFileDialog::getOpenFileName(this, "Open Log File", "", "CSV (*.csv)");
    if (qstrFileName.isEmpty())
        return;

    QFile file(qstrFileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    //Parse the column names
    QStringList listColumnNames;
    QByteArray arrColumnNames = file.readLine();
    QList<QByteArray> listArr = arrColumnNames.split(',');
    for (int i = 0; i < listArr.size(); i++) {
        listColumnNames.append(listArr.at(i));
    }

    std::map<LogOutput, int> mapOutputColumn;
    for (int col = 0; col < listColumnNames.size(); col++) {
        const QString& qstrName = listColumnNames.at(col);
        if (mapOutputNames.count(qstrName)) {
            LogOutput output = mapOutputNames.at(qstrName);
            mapOutputColumn.emplace(output, col);
        } else {
            std::cout << "Could not find log category: " << qstrName.toStdString() << std::endl;
        }
    }

    //Parse the row data
    Log log(mapOutputColumn);
    while (!file.atEnd()) {
        QStringList listRow;
        QByteArray arrRow = file.readLine();
        QList<QByteArray> listArr = arrRow.split(',');
        for (int i = 0; i < listArr.size(); i++)
            listRow.append(listArr.at(i));
        log.AddRow(listRow);
    }

    //Set the new log in the model
    m_model->SetLog(log);
}

void MainWindow::on_buttonInjector_clicked()
{
    InjectorDialog* dialog = new InjectorDialog(this);
    dialog->SetModel(m_model);
    dialog->show();
}
