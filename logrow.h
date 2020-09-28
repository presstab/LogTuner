#ifndef LOGROW_H
#define LOGROW_H

#include "logoutput.h"
#include <QVariant>
#include <map>

class LogRow
{
private:
    std::map<LogOutput, QVariant> m_mapData;

public:
    void AddData(LogOutput output, const QVariant& data);
    uint32_t Time() const;
    bool IsOpenLoop() const;
    bool IsClosedLoop() const;
    int32_t AfCorrection1() const;
    int32_t AfLearning1() const;
    int32_t TotalAf1Correction() const;
    uint32_t MafVolts() const;
};

#endif // LOGROW_H
