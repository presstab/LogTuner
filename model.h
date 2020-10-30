#ifndef MODEL_H
#define MODEL_H

#include <map>
#include <log.h>

class Model
{
private:
    Log m_log;
    std::map<uint32_t, uint32_t> m_mapMafScale;
    std::vector<std::pair<double, double>> m_vInjectorLatency; // volts => latency
    int32_t m_nInjectorScale;
    std::vector<std::pair<double, double>> m_vSmallIpwCompensations; // pulse width => fuel compensation %

public:
    std::map<uint32_t, uint32_t> MafScale() const;
    void SetMafScale(const std::map<uint32_t, uint32_t>& mapMafScale);

    void SetInjectorLatency(const std::vector<std::pair<double, double>>& vLatency);
    void SetInjectorScale(const int32_t& nScale);
    void SetLog(const Log& log);
    Log GetLog() const;
    void SetSmallIpwCompensations(const std::vector<std::pair<double, double>>& vCompensations);
    std::vector<std::pair<double, double>> GetSmallIpwCompensations() const;
};

#endif // MODEL_H
