#include "mafdialog.h"
#include "ui_mafdialog.h"

#include "model.h"
#include <QClipboard>
#include <QMenu>

MafDialog::MafDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MafDialog)
{
    ui->setupUi(this);
    ui->tableExistingScale->setColumnCount(54);
    ui->tableExistingScale->setRowCount(2);
    ui->tableExistingScale->setVerticalHeaderLabels({"Volts", "g/s"});
    ui->tableExistingScale->horizontalHeader()->hide();
    for (int i = 0; i < 54; i++) {
        ui->tableExistingScale->setColumnWidth(i, 10);
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

    QStringList listPasteCells = qstrPasteText.split("\t");

    for (int i = 0; i < listPasteCells.size(); i++) {
        QString qstrPasteValue = listPasteCells.at(i);
        int columnPaste = column + i;
        QTableWidgetItem* item = new QTableWidgetItem(qstrPasteValue);
        ui->tableExistingScale->setItem(row, columnPaste, item);
    }

    emit MafScaleChanged();
}

/**
 * @brief MafDialog::on_buttonRescaleMaf_clicked : Rescale the MAF according to the current log that is loaded.
 */
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

        int32_t nTotalCorrection = row.TotalAf1Correction();
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
}
