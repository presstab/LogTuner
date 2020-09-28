#include "log.h"

QVariant Log::ParseStringData(LogOutput output, const QString &qstrData)
{
    QVariant data;
    switch (output) {
        case LogOutput::Time: {
            //Convert to milliseconds
            data = uint32_t(qstrData.toDouble() * 1000);
            break;
        }
        case LogOutput::CalculatedLoad:
        case LogOutput::AfSens1:
        case LogOutput::ThrottlePos:
        case LogOutput::InjectorDutyCycle:
        case LogOutput::MafGs:
        case LogOutput::MafVolts: {
            //Convert from double to uint32_t, decimal shifted right two places
            data = uint32_t(qstrData.toDouble()*100);
            break;
        }
        case LogOutput::AfCorrection1:
        case LogOutput::AfLearning1:
        case LogOutput::Boost:
        case LogOutput::DynamicAdvance:
        case LogOutput::FeedbackKnock:
        case LogOutput::FineKnockLearning:
        case LogOutput::IgnitionTiming:
        case LogOutput::InjectorLatency: {
            //Convert from double to int32_t, decimal shifted right two places
            data = int32_t(qstrData.toDouble()*100);
            break;
        }
        case LogOutput::ClosedLoopSwitch: {
            //Convert to bool from string
            data = bool(qstrData == "on");
            break;
        }
        case LogOutput::RPM:
        case LogOutput::VehicleSpeed: {
            //convert from string to uint32_t
            data = uint32_t(qstrData.toULong());
        }
    }
    return data;
}

/**
 * @brief Log::AddRow add a new row of data to the log
 * @param listRow
 */
void Log::AddRow(const QStringList &listRow)
{
    LogRow row;
    for (int i = 0; i < listRow.size(); i++) {
        LogOutput output = m_mapColumnOutput.at(i);
        QString qstrData = listRow.at(i);
        QVariant data = ParseStringData(output, qstrData);
        row.AddData(output, data);
    }

    m_mapRows.emplace(m_mapRows.size(), row);
}

unsigned int Log::RowCount() const
{
    return m_mapRows.size();
}

const LogRow &Log::Row(int nRow) const
{
    return m_mapRows.at(nRow);
}


