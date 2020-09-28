#ifndef LOG_H
#define LOG_H

#include "logoutput.h"
#include "logrow.h"
#include <QVariant>
#include <map>

class Log
{
private:
    std::map<uint32_t, LogRow> m_mapRows;
    std::map<LogOutput, int> m_mapOutputColumn;
    std::map<int, LogOutput> m_mapColumnOutput;

    QVariant ParseStringData(LogOutput output, const QString& qstrData);

public:
    Log()
    {

    }

    Log(const std::map<LogOutput, int>& mapOutputColumn)
    {
        m_mapOutputColumn = mapOutputColumn;
        for (const auto& pair : mapOutputColumn) {
            m_mapColumnOutput.emplace(pair.second, pair.first);
        }
    }

    void AddRow(const QStringList& listRow);
    unsigned int RowCount() const;
    const LogRow& Row(int nRow) const;
};

#endif // LOG_H
