#include "logoutput.h"

std::map<QString, LogOutput> mapOutputNames
{
    {"AF Correction 1 (%)", LogOutput::AfCorrection1},
    {"AF Correction 3 (%)", LogOutput::AfCorrection3},
    {"AF Learning 1 (%)", LogOutput::AfLearning1},
    {"AF Learning 3 (%)", LogOutput::AfLearning3},
    {"AF Sens 1 Ratio (AFR)", LogOutput::AfSens1},
    {"Boost (psi)", LogOutput::Boost},
    {"Calculated Load (g/rev)", LogOutput::CalculatedLoad},
    {"Closed Loop Sw (on/off)", LogOutput::ClosedLoopSwitch},
    {"Comm Fuel Final (AFR)", LogOutput::CommandedFuel},
    {"Dynamic Adv (°)", LogOutput::DynamicAdvance},
    {"Feedback Knock (°)", LogOutput::FeedbackKnock},
    {"Fine Knock Learn (°)", LogOutput::FineKnockLearning},
    {"Ignition Timing (°)", LogOutput::IgnitionTiming},
    {"Inj PW Base (ms)", LogOutput::InjectorBasePulseWidth},
    {"Inj Duty Cycle (%)", LogOutput::InjectorDutyCycle},
    {"Inj Latency (ms)", LogOutput::InjectorLatency},
    {"MAF (g/s)", LogOutput::MafGs},
    {"MAF Volt (V)", LogOutput::MafVolts},
    {"RPM (RPM)", LogOutput::RPM},
    {"Throttle Pos (%)", LogOutput::ThrottlePos},
    {"Time (sec)", LogOutput::Time},
    {"Vehicle Speed (mph)", LogOutput::VehicleSpeed},
    {"Sns Only Rear O2 (raw)", LogOutput::WideBand}
};
