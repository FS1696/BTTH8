/****************/
/* Ma sinh vien: ................................ */
/* Ho ten: ....................................... */
/****************/
#ifndef DEVICE_MONITORING_COORDINATOR_H
#define DEVICE_MONITORING_COORDINATOR_H

#include <vector>
#include "TeachingDevice.h"
#include "Subscribers.h"

// Tap trung viec dang ky/huy dang ky cac subscriber cho nhieu thiet bi.
class DeviceMonitoringCoordinator
{
public:
    DeviceMonitoringCoordinator(const std::vector<TeachingDevice*>& devices,
                                 ConsoleNotificationService& notificationService,
                                 DeviceAuditLogger& auditLogger,
                                 MaintenanceService& maintenanceService);
    ~DeviceMonitoringCoordinator();

    DeviceMonitoringCoordinator(const DeviceMonitoringCoordinator&) = delete;
    DeviceMonitoringCoordinator& operator=(const DeviceMonitoringCoordinator&) = delete;

    void Subscribe();
    void Unsubscribe();
    void Dispose();
    bool IsSubscribed() const;

private:
    struct SubscriptionInfo
    {
        TeachingDevice* device;
        TeachingDevice::StatusSubscriptionId statusToken;
        TeachingDevice::AlertSubscriptionId notificationToken;
        TeachingDevice::AlertSubscriptionId maintenanceToken;
    };

    std::vector<TeachingDevice*> devices_;
    std::vector<SubscriptionInfo> subscriptions_;
    ConsoleNotificationService& notificationService_;
    DeviceAuditLogger& auditLogger_;
    MaintenanceService& maintenanceService_;
    bool subscribed_;
};

#endif // DEVICE_MONITORING_COORDINATOR_H
