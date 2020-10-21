#ifndef INJECTORTUNER_H
#define INJECTORTUNER_H

class Model;

class InjectorTuner
{
private:
    Model* m_model;

public:
    InjectorTuner(Model* model);
    void CalculateTune();
};

#endif // INJECTORTUNER_H
