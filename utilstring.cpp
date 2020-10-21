#include "utilstring.h"

#include <iomanip>
#include <sstream>

QString PrecisionToQString(double d, int precision)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision);
    stream << d;
    return QString::fromStdString(stream.str());
}
