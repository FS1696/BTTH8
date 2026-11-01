/****************/
// Mã sinh viên: 202419074
// Họ tên: Vũ Thành Lâm
/****************/
#ifndef SUBSCRIBERS_H
#define SUBSCRIBERS_H

#include <functional>
#include <string>
#include <vector>
#include "DeviceEventArgs.h"

class TeachingDevice;

// Subscriber 1: hien thi canh bao tren console.
class ConsoleNotificationService
{
public:
    void HandleAlert(const TeachingDevice* sender, const DeviceAlertEventArgs& args) const;
};

// Subscriber 2: ghi nhat ky cac lan thay doi trang thai.
class DeviceAuditLogger
{
public:
    void HandleStatusChanged(const TeachingDevice* sender, const DeviceStatusChangedEventArgs& args);
    const std::vector<std::string>& GetEntries() const;

private:
    std::vector<std::string> entries_;
};

// Subscriber 3: chi tao yeu cau bao tri voi canh bao Critical.
class MaintenanceService
{
public:
    void HandleCriticalAlert(const TeachingDevice* sender, const DeviceAlertEventArgs& args);
    const std::vector<std::string>& GetRequests() const;

private:
    std::vector<std::string> requests_;
};

// Delegate tu dinh nghia mo phong AlertFilter trong de bai.
typedef std::function<bool(const DeviceAlertEventArgs&)> AlertFilter;
typedef std::function<void(const DeviceAlertEventArgs&)> AlertAction;

// Subscriber co dieu kien: chi thuc hien action neu filter tra ve true.
class FilteredAlertSubscriber
{
public:
    FilteredAlertSubscriber(const AlertFilter& filter, const AlertAction& action);
    void HandleAlert(const TeachingDevice* sender, const DeviceAlertEventArgs& args) const;

private:
    AlertFilter filter_;
    AlertAction action_;
};

#endif // SUBSCRIBERS_H
