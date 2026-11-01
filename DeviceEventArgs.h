/****************/
// Mã sinh viên: 202419074
// Họ tên: Vũ Thành Lâm
/****************/
#ifndef DEVICE_EVENT_ARGS_H
#define DEVICE_EVENT_ARGS_H

#include <ctime>
#include <string>
#include "DeviceEnums.h"

// Du lieu gui kem khi trang thai thiet bi thay doi.
struct DeviceStatusChangedEventArgs
{
    std::string DeviceId;
    DeviceStatus OldStatus;
    DeviceStatus NewStatus;
    std::time_t ChangedAt;

    DeviceStatusChangedEventArgs(const std::string& deviceId,
                                 DeviceStatus oldStatus,
                                 DeviceStatus newStatus,
                                 std::time_t changedAt)
        : DeviceId(deviceId), OldStatus(oldStatus), NewStatus(newStatus), ChangedAt(changedAt)
    {
    }
};

// Du lieu gui kem khi thiet bi phat canh bao.
struct DeviceAlertEventArgs
{
    std::string DeviceId;
    std::string Message;
    AlertSeverity Severity;
    std::time_t OccurredAt;

    DeviceAlertEventArgs(const std::string& deviceId,
                         const std::string& message,
                         AlertSeverity severity,
                         std::time_t occurredAt)
        : DeviceId(deviceId), Message(message), Severity(severity), OccurredAt(occurredAt)
    {
    }
};

// Dinh dang moc thoi gian theo dang de doc.
inline std::string FormatTime(std::time_t value)
{
    char buffer[32];
    std::tm* timeInfo = std::localtime(&value);
    if (timeInfo == NULL)
        return "unknown-time";
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);
    return buffer;
}

#endif // DEVICE_EVENT_ARGS_H
