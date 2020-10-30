#ifndef INJECTORTUNER_H
#define INJECTORTUNER_H

class Model;

#include <cstdint>
#include <vector>

class InjectorTuner
{
private:
    Model* m_model;

public:
    InjectorTuner(Model* model);
    std::vector<std::pair<int32_t, std::pair<int, double>>> CalculateSmallIpw();
    void CalculateTune(bool fOpenLoop);
};

#endif // INJECTORTUNER_H
