#ifndef LOGOUTPUT_H
#define LOGOUTPUT_H

#include <QString>

#include <map>

enum LogOutput
{
    AfCorrection1,
    AfLearning1,
    AfLearning3,
    AfCorrection3,
    AfSens1,
    Boost,
    CalculatedLoad,
    ClosedLoopSwitch,
    DynamicAdvance,
    FeedbackKnock,
    FineKnockLearning,
    IgnitionTiming,
    InjectorDutyCycle,
    InjectorLatency,
    MafGs,
    MafVolts,
    RPM,
    ThrottlePos,
    Time,
    VehicleSpeed
};

extern std::map<QString, LogOutput> mapOutputNames;



#endif // LOGOUTPUT_H
