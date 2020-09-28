#ifndef MODEL_H
#define MODEL_H

#include <map>
#include <log.h>

class Model
{
private:
    std::map<uint32_t, uint32_t> m_mapMafScale;
    Log m_log;

public:
    std::map<uint32_t, uint32_t> MafScale() const;
    void SetMafScale(const std::map<uint32_t, uint32_t>& mapMafScale);

    void SetLog(const Log& log);
    Log GetLog() const;
};

#endif // MODEL_H
