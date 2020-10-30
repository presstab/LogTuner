#include "logrow.h"

void LogRow::AddData(LogOutput output, const QVariant &data)
{
    m_mapData.emplace(output, data);
}

uint32_t LogRow::Time() const
{
    if (m_mapData.count(LogOutput::Time))
        return m_mapData.at(LogOutput::Time).toUInt();
    return 0;
}

bool LogRow::IsOpenLoop() const
{
    return !IsClosedLoop();
}

bool LogRow::IsClosedLoop() const
{
    if (m_mapData.count(LogOutput::ClosedLoopSwitch))
        return m_mapData.at(LogOutput::ClosedLoopSwitch).toBool();
    return false;
}

int32_t LogRow::AfCorrection1() const
{
    if (m_mapData.count(LogOutput::AfCorrection1))
        return m_mapData.at(LogOutput::AfCorrection1).toInt();
    return 0;
}

int32_t LogRow::AfCorrection3() const
{
    if (m_mapData.count(LogOutput::AfCorrection3))
        return m_mapData.at(LogOutput::AfCorrection3).toInt();
    return 0;
}

int32_t LogRow::AfLearning1() const
{
    if (m_mapData.count(LogOutput::AfLearning1))
        return m_mapData.at(LogOutput::AfLearning1).toInt();
    return 0;
}

int32_t LogRow::AfLearning3() const
{
    if (m_mapData.count(LogOutput::AfLearning3))
        return m_mapData.at(LogOutput::AfLearning3).toInt();
    return 0;
}

/**
 * @brief LogRow::TotalAf1Correction
 * @return TotalCorrection * 100. If datalog shows 1.04 then TotalCorrection() will return 104.
 * Note that 104 would indicate 1.04% correction.
 */
int32_t LogRow::TotalAf1Correction() const
{
    return AfCorrection1() + AfLearning1();
}

int32_t LogRow::TotalAf3Correction() const
{
    return AfCorrection3() + AfLearning3();
}

int32_t LogRow::InjectorBasePulseWidth() const
{
    if (m_mapData.count(LogOutput::InjectorBasePulseWidth))
        return m_mapData.at(LogOutput::InjectorBasePulseWidth).toInt();
    return 0;
}

double LogRow::CommandedFuel() const
{
    if (m_mapData.count(LogOutput::CommandedFuel))
        return m_mapData.at(LogOutput::CommandedFuel).toDouble();
    return 0;
}

double LogRow::WideBand() const
{
    if (m_mapData.count(LogOutput::WideBand))
        return m_mapData.at(LogOutput::WideBand).toDouble();
    return 0;
}

/**
 * @brief LogRow::MafVolts
 * @return MafVolts * 100. If datalog shows 1.04 then MafVolts() will return 104.
 */
uint32_t LogRow::MafVolts() const
{
    if (m_mapData.count(LogOutput::MafVolts))
        return m_mapData.at(LogOutput::MafVolts).toInt();
    return 0;
}
