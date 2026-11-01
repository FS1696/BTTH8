/****************/
/* Ma sinh vien: ................................ */
/* Ho ten: ....................................... */
/****************/
#ifndef DEVICES_H
#define DEVICES_H

#include "TeachingDevice.h"

// Dieu khien may chieu: canh bao khi pin chuyen vao vung duoi 20%.
class ProjectorRemote : public TeachingDevice
{
public:
    ProjectorRemote(const std::string& id, const std::string& name, int batteryLevel);
    int GetBatteryLevel() const;
    void UpdateBatteryLevel(int newLevel);

private:
    int batteryLevel_;
    int BatteryAlertRegion(int value) const;
};

// Dieu khien dieu hoa: canh bao khi mat kha nang phan hoi.
class AirConditionerRemote : public TeachingDevice
{
public:
    AirConditionerRemote(const std::string& id, const std::string& name,
                         const std::string& assignedRoom);
    const std::string& GetAssignedRoom() const;
    bool IsResponsive() const;
    void UpdateResponseState(bool isResponsive);

private:
    std::string assignedRoom_;
    bool isResponsive_;
};

// Micro khong day: theo doi ca pin va chat luong tin hieu.
class WirelessMicrophone : public TeachingDevice
{
public:
    WirelessMicrophone(const std::string& id, const std::string& name,
                       int batteryLevel, int signalQuality);
    int GetBatteryLevel() const;
    int GetSignalQuality() const;
    void UpdateBatteryLevel(int newLevel);
    void UpdateSignalQuality(int newQuality);

private:
    int batteryLevel_;
    int signalQuality_;
    int BatteryAlertRegion(int value) const;
    int SignalAlertRegion(int value) const;
};

#endif // DEVICES_H
