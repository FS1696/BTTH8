/****************/
/* Ma sinh vien: ................................ */
/* Ho ten: ....................................... */
/****************/
#include "Devices.h"
#include "Exceptions.h"

ProjectorRemote::ProjectorRemote(const std::string& id, const std::string& name,
                                 int batteryLevel)
    : TeachingDevice(id, name), batteryLevel_(batteryLevel)
{
    ValidatePercentage(id, batteryLevel);
}

int ProjectorRemote::GetBatteryLevel() const { return batteryLevel_; }

// 0 = vung an toan, 1 = Warning, 2 = Critical.
int ProjectorRemote::BatteryAlertRegion(int value) const
{
    if (value == 0) return 2;
    if (value < 20) return 1;
    return 0;
}

void ProjectorRemote::UpdateBatteryLevel(int newLevel)
{
    ValidatePercentage(GetId(), newLevel);
    const int oldRegion = BatteryAlertRegion(batteryLevel_);
    const int newRegion = BatteryAlertRegion(newLevel);
    batteryLevel_ = newLevel;

    // Chi phat canh bao khi di vao mot vung canh bao moi, tranh lap lai.
    if (newRegion != oldRegion && newRegion == 1)
        RaiseAlert("Projector remote battery is below 20%.", AlertSeverity::Warning);
    else if (newRegion != oldRegion && newRegion == 2)
        RaiseAlert("Projector remote battery is 0%.", AlertSeverity::Critical);
}

AirConditionerRemote::AirConditionerRemote(const std::string& id,
                                           const std::string& name,
                                           const std::string& assignedRoom)
    : TeachingDevice(id, name), assignedRoom_(assignedRoom), isResponsive_(true)
{
    if (assignedRoom_.empty())
        throw std::invalid_argument("Assigned room must not be empty.");
}

const std::string& AirConditionerRemote::GetAssignedRoom() const { return assignedRoom_; }
bool AirConditionerRemote::IsResponsive() const { return isResponsive_; }

void AirConditionerRemote::UpdateResponseState(bool isResponsive)
{
    const bool wasResponsive = isResponsive_;
    isResponsive_ = isResponsive;

    // Chi canh bao tai thoi diem chuyen tu co phan hoi sang mat phan hoi.
    if (wasResponsive && !isResponsive_)
        RaiseAlert("Air-conditioner remote is not responding in room " + assignedRoom_ + ".",
                   AlertSeverity::Critical);
}

WirelessMicrophone::WirelessMicrophone(const std::string& id, const std::string& name,
                                       int batteryLevel, int signalQuality)
    : TeachingDevice(id, name), batteryLevel_(batteryLevel), signalQuality_(signalQuality)
{
    ValidatePercentage(id, batteryLevel);
    ValidatePercentage(id, signalQuality);
}

int WirelessMicrophone::GetBatteryLevel() const { return batteryLevel_; }
int WirelessMicrophone::GetSignalQuality() const { return signalQuality_; }

int WirelessMicrophone::BatteryAlertRegion(int value) const
{
    if (value == 0) return 2;
    if (value < 30) return 1;
    return 0;
}

int WirelessMicrophone::SignalAlertRegion(int value) const
{
    if (value == 0) return 2;
    if (value < 40) return 1;
    return 0;
}

void WirelessMicrophone::UpdateBatteryLevel(int newLevel)
{
    ValidatePercentage(GetId(), newLevel);
    const int oldRegion = BatteryAlertRegion(batteryLevel_);
    const int newRegion = BatteryAlertRegion(newLevel);
    batteryLevel_ = newLevel;

    if (newRegion != oldRegion && newRegion == 1)
        RaiseAlert("Wireless microphone battery is below 30%.", AlertSeverity::Warning);
    else if (newRegion != oldRegion && newRegion == 2)
        RaiseAlert("Wireless microphone battery is 0%.", AlertSeverity::Critical);
}

void WirelessMicrophone::UpdateSignalQuality(int newQuality)
{
    ValidatePercentage(GetId(), newQuality);
    const int oldRegion = SignalAlertRegion(signalQuality_);
    const int newRegion = SignalAlertRegion(newQuality);
    signalQuality_ = newQuality;

    if (newRegion != oldRegion && newRegion == 1)
        RaiseAlert("Wireless microphone signal quality is below 40%.", AlertSeverity::Warning);
    else if (newRegion != oldRegion && newRegion == 2)
        RaiseAlert("Wireless microphone signal quality is 0%.", AlertSeverity::Critical);
}
