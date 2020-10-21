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

int32_t LogRow::TotalAf1Correction() const
{
    return AfCorrection1() + AfLearning1();
}

int32_t LogRow::TotalAf3Correction() const
{
    return AfCorrection3() + AfLearning3();
}

uint32_t LogRow::MafVolts() const
{
    if (m_mapData.count(LogOutput::MafVolts))
        return m_mapData.at(LogOutput::MafVolts).toInt();
    return 0;
}
