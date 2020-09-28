#include "model.h"

std::map<uint32_t, uint32_t> Model::MafScale() const
{
    return m_mapMafScale;
}

void Model::SetMafScale(const std::map<uint32_t, uint32_t> &mapMafScale)
{
    m_mapMafScale = mapMafScale;
}

void Model::SetLog(const Log &log)
{
    m_log = log;
}

Log Model::GetLog() const
{
    return m_log;
}
