#include "injectortuner.h"

#include "log.h"
#include "model.h"

#include <QMessageBox>

InjectorTuner::InjectorTuner(Model *model)
{
    m_model = model;
}

void InjectorTuner::CalculateTune()
{
    std::map<int32_t, std::vector<int32_t>> mapCorrections; // MafVoltage => Corrections
    const Log& log = m_model->GetLog();
    LogRow logrowLast;
    for (unsigned int row = 0; row < log.RowCount(); row++) {
        const LogRow& logrow = log.Row(row);
        if (row == 0 || !logrow.IsClosedLoop()) {
            logrowLast = logrow;
            continue;
        }

        //Filter out differences that are larger than 0.2 volts/second
        int32_t nTimeDifference = logrow.Time() - logrowLast.Time();
        int32_t nVoltDifference = logrow.MafVolts() - logrowLast.MafVolts();
        double dVoltsSecChange = double(nVoltDifference)/double(nTimeDifference);
        if (dVoltsSecChange >= 0.2) {
            logrowLast = logrow;
            continue;
        }

        //Numbers do not need to be exact, only need to see linearity of the corrections, so make voltage less granular
        int32_t nMafKey = logrow.MafVolts() / 10;
        if (!mapCorrections.count(nMafKey))
            mapCorrections.emplace(nMafKey, std::vector<int32_t>());
        mapCorrections.at(nMafKey).emplace_back(logrow.TotalAf1Correction());
    }

    //Calculate average correction for each mafkey
    std::vector<std::pair<int32_t, int32_t>> vCorrections;
    for (const auto& pair : mapCorrections) {
        const std::vector<int32_t>& vMafCorrections = pair.second;
        double dTotalCorrection = 0;
        for (const int32_t& nCorrection : vMafCorrections)
            dTotalCorrection += nCorrection;
        int32_t nCorrection = dTotalCorrection / vMafCorrections.size();
        vCorrections.emplace_back(std::make_pair(pair.first, nCorrection));
    }

    //Calculate a best fit line

    //Average x and y
    double dAvgX = 0; // X is the voltage
    double dAvgY = 0; // Y is the correction
    for (const auto& pair : vCorrections) {
        dAvgX += pair.first;
        dAvgY += pair.second;
    }
    dAvgX /= vCorrections.size();
    dAvgY /= vCorrections.size();

    //(xi - avgx)(yi - avgy)
    double dAvgDiff = 0;
    double dX2 = 0;
    for (const auto& pair : vCorrections) {
        dAvgDiff += (pair.first - dAvgX) * (pair.second - dAvgY);
        dX2 += int32_t(pair.first - dAvgX)^2;
    }

    //Slope
    double dSlope = dAvgDiff/dX2;

    QMessageBox box;
    QString str = QString("Best fit line slope is %1").arg(dSlope);
    box.setText(str);
    box.exec();
}
