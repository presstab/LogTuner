#include "injectortuner.h"

#include "log.h"
#include "model.h"

#include <QMessageBox>

#include <iostream>

InjectorTuner::InjectorTuner(Model *model)
{
    m_model = model;
}

/**
 * @brief InjectorTuner::CalculateSmallIpw
 * @return [{pulsewidth => {record count, avg correction}}]
 */
std::vector<std::pair<int32_t, std::pair<int, double>>> InjectorTuner::CalculateSmallIpw()
{
    std::map<int32_t, std::vector<int32_t>> mapPulseWidthCorrections; // PulseWidth => Corrections
    const Log& log = m_model->GetLog();
    LogRow logrowLast;
    double dVoltsPrevChange = 0;
    for (unsigned int row = 0; row < log.RowCount(); row++) {
        const LogRow& logrow = log.Row(row);
        if (row == 0 || logrow.IsOpenLoop()) {
            logrowLast = logrow;
            continue;
        }

        //Filter out differences that are larger than 0.2 volts/second, or if two records ago is a big change
        int32_t nTimeDifference = logrow.Time() - logrowLast.Time();
        int32_t nVoltDifference = logrow.MafVolts() - logrowLast.MafVolts();
        double dVoltsSecChange = double(nVoltDifference)/double(nTimeDifference);
        if (dVoltsSecChange >= 0.2 || dVoltsPrevChange >= 0.2) {
            dVoltsPrevChange = dVoltsSecChange;
            logrowLast = logrow;
            continue;
        }
        dVoltsPrevChange = dVoltsSecChange;

        int32_t nPulseWidthKey = logrow.InjectorBasePulseWidth() / 10;
        int32_t nRemainder = nPulseWidthKey % 5;
        if (nRemainder >= 3)
            nPulseWidthKey += 5-nRemainder;
        else if (nRemainder > 0)
            nPulseWidthKey -= nRemainder;

        if (!mapPulseWidthCorrections.count(nPulseWidthKey))
            mapPulseWidthCorrections.emplace(nPulseWidthKey, std::vector<int32_t>());

        int32_t nTotalError = logrow.TotalAf1Correction();
        mapPulseWidthCorrections.at(nPulseWidthKey).emplace_back(nTotalError);
        logrowLast = logrow;
    }

    //Calculate average correction for each pulse width
    std::vector<std::pair<int32_t, std::pair<int, double>>> vPulseWidthCorrections;
    QString strOut;
    for (const auto& pair : mapPulseWidthCorrections) {
        const std::vector<int32_t>& vIpwCorrections = pair.second;
        //If there are not at least 30 data points then skip this correction range
        if (vIpwCorrections.size() < 30)
            continue;

        double dTotalCorrection = 0;
        for (const int32_t& nCorrection : vIpwCorrections)
            dTotalCorrection += nCorrection;
        dTotalCorrection /= vIpwCorrections.size();
        dTotalCorrection /= 100;
        vPulseWidthCorrections.emplace_back(pair.first, std::make_pair(vIpwCorrections.size(), dTotalCorrection));
    }

    return vPulseWidthCorrections;
}

void InjectorTuner::CalculateTune(bool fOpenLoop)
{
    std::map<int32_t, std::vector<int32_t>> mapMafCorrections; // MafVoltage => Corrections
    std::map<int32_t, std::vector<int32_t>> mapPulseWidthCorrections; // PulseWidth => Corrections
    const Log& log = m_model->GetLog();
    LogRow logrowLast;
    double dVoltsPrevChange = 0;
    for (unsigned int row = 0; row < log.RowCount(); row++) {
        const LogRow& logrow = log.Row(row);
        if (row == 0) {
            logrowLast = logrow;
            continue;
        }

        //Filter out differences that are larger than 0.2 volts/second, or if two records ago is a big change
        int32_t nTimeDifference = logrow.Time() - logrowLast.Time();
        int32_t nVoltDifference = logrow.MafVolts() - logrowLast.MafVolts();
        double dVoltsSecChange = double(nVoltDifference)/double(nTimeDifference);
        if (dVoltsSecChange >= 0.2 || dVoltsPrevChange >= 0.2) {
            dVoltsPrevChange = dVoltsSecChange;
            logrowLast = logrow;
            continue;
        }
        dVoltsPrevChange = dVoltsSecChange;

        int32_t nPulseWidthKey = logrow.InjectorBasePulseWidth() / 10;
        int32_t nRemainder = nPulseWidthKey % 5;
        if (nRemainder >= 3)
            nPulseWidthKey += 5-nRemainder;
        else if (nRemainder > 0)
            nPulseWidthKey -= nRemainder;

        if (!mapPulseWidthCorrections.count(nPulseWidthKey))
            mapPulseWidthCorrections.emplace(nPulseWidthKey, std::vector<int32_t>());

        int32_t nTotalError = logrow.TotalAf1Correction();
        if (fOpenLoop &&logrow.IsOpenLoop() && row + 3 < log.RowCount()) {
            //Calculate the error manually for closed loop, going by wideband
            double dCommandedFuel = logrow.CommandedFuel();

            //Wideband AF is delayed, get it from 3 rows back
            LogRow logrowWB = log.Row(row + 2);
            double dAFR = logrowWB.WideBand();
            nTotalError = int32_t((dCommandedFuel - dAFR)/dCommandedFuel*10000);

            //Todo filter out decel type events where AFR is way higher than commanded fuel
        }

        mapPulseWidthCorrections.at(nPulseWidthKey).emplace_back(nTotalError);

        if (!fOpenLoop && logrow.IsOpenLoop()) {
            logrowLast = logrow;
            continue;
        }

        //Numbers do not need to be exact, only need to see linearity of the corrections, so make voltage less granular
        int32_t nMafKey = logrow.MafVolts() / 10;
        if (!mapMafCorrections.count(nMafKey))
            mapMafCorrections.emplace(nMafKey, std::vector<int32_t>());
        mapMafCorrections.at(nMafKey).emplace_back(logrow.TotalAf1Correction());
        logrowLast = logrow;
    }

    //Calculate average correction for each mafkey
    std::vector<std::pair<int32_t, int32_t>> vCorrections;
    for (const auto& pair : mapMafCorrections) {
        const std::vector<int32_t>& vMafCorrections = pair.second;
        double dTotalCorrection = 0;
        for (const int32_t& nCorrection : vMafCorrections)
            dTotalCorrection += nCorrection;
        int32_t nCorrection = dTotalCorrection / vMafCorrections.size();
        vCorrections.emplace_back(std::make_pair(pair.first, nCorrection));
    }

    //Calculate average correction for each pulse width
    std::vector<std::pair<int32_t, double>> vPulseWidthCorrections;
    QString strOut;
    for (const auto& pair : mapPulseWidthCorrections) {
        const std::vector<int32_t>& vIpwCorrections = pair.second;
        if (vIpwCorrections.size() < 30)
            continue;
        double dTotalCorrection = 0;
        for (const int32_t& nCorrection : vIpwCorrections)
            dTotalCorrection += nCorrection;
        dTotalCorrection /= vIpwCorrections.size();
        dTotalCorrection /= 100;
        vPulseWidthCorrections.emplace_back(pair.first, dTotalCorrection);
        strOut += QString("PW: ") + std::to_string(pair.first).c_str() + QString(": ") + std::to_string(dTotalCorrection).c_str() + "% " + std::to_string(vIpwCorrections.size()).c_str() + "\n";
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
    //QString str = QString("Best fit line slope is %1").arg(dSlope);
    box.setText(strOut);
    box.exec();
    std::cout << strOut.toStdString() << std::endl;
}
