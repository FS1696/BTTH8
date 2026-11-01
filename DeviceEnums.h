/****************/
/* Ma sinh vien: ................................ */
/* Ho ten: ....................................... */
/****************/
#ifndef DEVICE_ENUMS_H
#define DEVICE_ENUMS_H

#include <string>

// Trang thai chung cua thiet bi trong phong day.
enum class DeviceStatus
{
    Available,
    Borrowed,
    UnderMaintenance
};

// Muc do nghiem trong cua canh bao.
enum class AlertSeverity
{
    Information,
    Warning,
    Critical
};

// Chuyen enum sang chuoi de hien thi va ghi nhat ky.
inline std::string ToString(DeviceStatus status)
{
    switch (status)
    {
        case DeviceStatus::Available: return "Available";
        case DeviceStatus::Borrowed: return "Borrowed";
        case DeviceStatus::UnderMaintenance: return "UnderMaintenance";
    }
    return "Unknown";
}

inline std::string ToString(AlertSeverity severity)
{
    switch (severity)
    {
        case AlertSeverity::Information: return "Information";
        case AlertSeverity::Warning: return "Warning";
        case AlertSeverity::Critical: return "Critical";
    }
    return "Unknown";
}

#endif // DEVICE_ENUMS_H
