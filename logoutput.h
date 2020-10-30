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
    CommandedFuel,
    DynamicAdvance,
    FeedbackKnock,
    FineKnockLearning,
    IgnitionTiming,
    InjectorBasePulseWidth,
    InjectorDutyCycle,
    InjectorLatency,
    MafGs,
    MafVolts,
    RPM,
    ThrottlePos,
    Time,
    VehicleSpeed,
    WideBand
};

extern std::map<QString, LogOutput> mapOutputNames;



#endif // LOGOUTPUT_H
