#include "mafdialog.h"
#include "ui_mafdialog.h"

#include "model.h"
#include "utilstring.h"
#include <QClipboard>
#include <QMenu>

MafDialog::MafDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MafDialog)
{
    ui->setupUi(this);
    ui->tableExistingScale->setColumnCount(54);
    ui->tableCorrectedScale->setColumnCount(54);
    ui->tableExistingScale->setRowCount(2);
    ui->tableCorrectedScale->setRowCount(4);
    ui->tableExistingScale->setVerticalHeaderLabels({"Volts", "g/s"});
    ui->tableCorrectedScale->setVerticalHeaderLabels({"Volts", "g/s", "Change", "Data Count"});
    ui->tableExistingScale->horizontalHeader()->hide();
    ui->tableCorrectedScale->horizontalHeader()->hide();
    for (int i = 0; i < 54; i++) {
        ui->tableExistingScale->setColumnWidth(i, 10);
        ui->tableCorrectedScale->setColumnWidth(i, 10);
    }

    ui->tableExistingScale->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableExistingScale, &QTableWidget::customContextMenuRequested, this, &MafDialog::ExistingScaleContextMenu);
}

MafDialog::~MafDialog()
{
    delete ui;
}

void MafDialog::SetModel(Model *model)
{
    m_model = model;
}

/**
 * @brief MafDialog::GetMafScale Return the Maf Scale that is currently loaded in the Existing Scale table widget
 * @return All values are converted from doubles to uint32_t by multiplying them by 100
 */
std::map<uint32_t, uint32_t> MafDialog::GetMafScale() const
{
    std::map<uint32_t, uint32_t> mapMafScale;
    for (int i = 0; i < ui->tableExistingScale->columnCount(); i++) {
        auto itemVolts = ui->tableExistingScale->item(0, i);
        auto itemGs = ui->tableExistingScale->item(1, i);
        if (!itemVolts || !itemGs)
            break;
        mapMafScale.emplace(uint32_t(itemVolts->text().toDouble()*100), uint32_t(itemGs->text().toDouble()*100));
    }

    return mapMafScale;
}


/**
 * @brief MafDialog::ExistingScaleContextMenu Right click on the MafScale table and a context menu pops up to allow pasting
 * @param pos
 */
void MafDialog::ExistingScaleContextMenu(const QPoint &pos)
{
    QModelIndex index = ui->tableExistingScale->indexAt(pos);

    QMenu* menu = new QMenu(this);
    auto actionPaste = new QAction("Paste", menu);
    actionPaste->setProperty("row", index.row());
    actionPaste->setProperty("column", index.column());
    menu->addAction(actionPaste);
    connect(actionPaste, &QAction::triggered, this, &MafDialog::ExistingScalePaste);

    menu->popup(ui->tableExistingScale->viewport()->mapToGlobal(pos));
}

void MafDialog::ExistingScalePaste()
{
    //The index coordinate is saved in the action's properties
    int row = sender()->property("row").toInt();
    int column = sender()->property("column").toInt();

    QClipboard* clipboard = qApp->clipboard();
    QString qstrPasteText = clipboard->text();

    //Possible that both the volt and gs rows were pasted at the same.
    QStringList listPastedRows = qstrPasteText.split("\n");

    QStringList listPasteCellsVolts;
    QStringList listPasteCellsGs;

    if (listPastedRows.size() > 1) {
        listPasteCellsVolts = listPastedRows.at(0).split("\t");
        listPasteCellsGs = listPastedRows.at(1).split("\t");
    }

    for (int i = 0; i < listPasteCellsVolts.size(); i++) {
        QString qstrPasteValue = listPasteCellsVolts.at(i);
        int columnPaste = column + i;
        QTableWidgetItem* item = new QTableWidgetItem(qstrPasteValue);
        ui->tableExistingScale->setItem(0, columnPaste, item);
    }

    for (int i = 0; i < listPasteCellsGs.size(); i++) {
        QString qstrPasteValue = listPasteCellsGs.at(i);
        int columnPaste = column + i;
        QTableWidgetItem* item = new QTableWidgetItem(qstrPasteValue);
        ui->tableExistingScale->setItem(1, columnPaste, item);
    }

    m_model->SetMafScale(GetMafScale());
}

/**
 * @brief MafDialog::on_buttonRescaleMaf_clicked : Rescale the MAF according to the current log that is loaded.
 */
#include <iostream>
void MafDialog::on_buttonRescaleMaf_clicked()
{
    auto mapMafScale = GetMafScale();
    Log log = m_model->GetLog();

    std::map<uint32_t, std::vector<int32_t>> mapCorrections; // voltage => vector of corrections

    uint32_t nLastVolts = 0;
    bool fLastSkipped = false;
    for (unsigned int i = 0; i < log.RowCount(); i++) {
        const LogRow& row = log.Row(i);

        //Filter out if it is currently open loop status
        if (row.IsOpenLoop()) {
            fLastSkipped = true;
            continue;
        }

        int32_t nTotalCorrection = row.TotalAf1Correction() + row.TotalAf3Correction();
        uint32_t nMafVolts = row.MafVolts();

        //Skip if there is a large shift in volts from previous row
        if (!fLastSkipped) {
            int32_t nVoltageDifference = int32_t(nMafVolts) - int32_t(nLastVolts);
            if (nVoltageDifference < 0)
                nVoltageDifference = ~nVoltageDifference;
            double dVoltageChange = double(nVoltageDifference) / double(nLastVolts);
            if (dVoltageChange > 0.03) {
                fLastSkipped = true;
                continue;
            }
        }

        if (!mapCorrections.count(nMafVolts))
            mapCorrections.emplace(nMafVolts, std::vector<int32_t>());
        mapCorrections.at(nMafVolts).emplace_back(nTotalCorrection);

        fLastSkipped = false;
        nLastVolts = nMafVolts;
    }

    //Now calculate a corrected table
    std::map<uint32_t, uint32_t> mapCorrectedMAF = m_model->MafScale();
    std::map<uint32_t, double> mapChangeToApply;
    std::map<uint32_t, int> mapDataCount; // Volts => datacount
    for (const std::pair<uint32_t, std::vector<int32_t>>& pairCorrection : mapCorrections) {
        const uint32_t& nVolts = pairCorrection.first;

        //Find where in the existing scale that this voltage applies to
        uint32_t nVoltsPrev = 0;
        for (const auto& pair : m_model->MafScale()) {
            const uint32_t& nVoltsMapped = pair.first;
            if (nVolts <= nVoltsMapped) {
                //Found slot. Adjust to fit the scaled value.
                double nVoltsDifferenceLow = nVolts - nVoltsPrev;
                double nVoltsDifferenceHigh = nVoltsMapped - nVolts;
                double nVoltsInterval = nVoltsMapped - nVoltsPrev;
                double nApplyLow = nVoltsDifferenceLow/nVoltsInterval;
                double nApplyHigh = nVoltsDifferenceHigh/nVoltsInterval;

                int32_t nTotalChange = 0;
                int nDataPoints = pairCorrection.second.size();
                for (const int32_t& nLoggedCorrection : pairCorrection.second) {
                    nTotalChange += nLoggedCorrection;
                }
                double dTotalChange = double(nTotalChange) / pairCorrection.second.size();
                dTotalChange /= 10000;

                if (mapChangeToApply.count(nVoltsPrev)) {
                    //Already a corrected value for this voltage, average the two values
                    auto nValueExisting = mapChangeToApply.at(nVoltsPrev);
                    auto nValueNew = (nValueExisting + (dTotalChange*nApplyLow)) / 2;
                    mapChangeToApply.at(nVoltsPrev) = nValueNew;
                } else {
                    mapChangeToApply.emplace(nVoltsPrev, (dTotalChange*nApplyLow));
                }

                if (mapChangeToApply.count(nVoltsMapped)) {
                    //Already a corrected value for this voltage, average the two values
                    auto nValueExisting = mapChangeToApply.at(nVoltsMapped);
                    auto nValueNew = (nValueExisting + (dTotalChange*nApplyHigh)) / 2;
                    mapChangeToApply.at(nVoltsMapped) = nValueNew;
                } else {
                    mapChangeToApply.emplace(nVoltsMapped, (dTotalChange*nApplyHigh));
                }
                mapDataCount[nVoltsPrev] += nDataPoints;
                mapDataCount[nVoltsMapped] += nDataPoints;

                break;
            }
            nVoltsPrev = nVoltsMapped;
        }
    }

    //Values to apply are loaded, now apply to the corrected map
    for (const auto& pair : mapChangeToApply) {
        double existing = mapCorrectedMAF.at(pair.first);

        //The reported correction adds fuel if positive, or removes if negative.
        //A negative correction means the fuel mixture is too rich and fuel is being removed.
        //If there is a negative correction, then it means MAF likely reported more air being added than is actually there.
        double correction = pair.second;

        //If there are not a lot of corrections to apply, then use a max correction of 2.5%
        int nCorrectionCount = mapDataCount.at(pair.first);
        if (nCorrectionCount < 10) {

        }
        double newValue = existing * (1+pair.second);
        mapCorrectedMAF.at(pair.first) = uint32_t(newValue);
    }

    //Now fill in widget table with values
    int column = 0;
    for (const auto& pair : mapCorrectedMAF) {
        QString qstrVolts = PrecisionToQString(double(pair.first)/100, 2);
        QTableWidgetItem* itemVolts = new QTableWidgetItem(qstrVolts);
        ui->tableCorrectedScale->setItem(0, column, itemVolts);

        QString qstrGs = PrecisionToQString(double(pair.second)/100, 2);
        QTableWidgetItem* itemGs = new QTableWidgetItem(qstrGs);
        ui->tableCorrectedScale->setItem(1, column, itemGs);

        if (mapChangeToApply.count(pair.first)) {
            auto nValueChange = mapChangeToApply.at(pair.first)*100;
            QString qstrChange = PrecisionToQString(nValueChange, 2);
            qstrChange += "%";
            QTableWidgetItem* itemChange = new QTableWidgetItem(qstrChange);

            //Set green if increased, red if decreased
            QColor colorText = (nValueChange > 0 ? Qt::darkGreen : Qt::red);
            itemChange->setForeground(colorText);

            ui->tableCorrectedScale->setItem(2, column, itemChange);

            //Set the data count
            int nCount = mapDataCount.at(pair.first);
            QString qstrCount = QString::number(nCount);
            QTableWidgetItem* itemCount = new QTableWidgetItem(qstrCount);
            ui->tableCorrectedScale->setItem(3, column, itemCount);
        }

        column++;
    }
}










