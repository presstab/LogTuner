#include "model.h"

std::map<uint32_t, uint32_t> Model::MafScale() const
{
    return m_mapMafScale;
}

void Model::SetMafScale(const std::map<uint32_t, uint32_t> &mapMafScale)
{
    m_mapMafScale = mapMafScale;
}

void Model::SetInjectorLatency(const std::vector<std::pair<double, double> > &vLatency)
{
    m_vInjectorLatency = vLatency;
}

void Model::SetInjectorScale(const int32_t &nScale)
{
    m_nInjectorScale = nScale;
}

void Model::SetLog(const Log &log)
{
    m_log = log;
}

Log Model::GetLog() const
{
    return m_log;
}

void Model::SetSmallIpwCompensations(const std::vector<std::pair<double, double> > &vCompensations)
{
    m_vSmallIpwCompensations = vCompensations;
}

std::vector<std::pair<double, double> > Model::GetSmallIpwCompensations() const
{
    return m_vSmallIpwCompensations;
}
