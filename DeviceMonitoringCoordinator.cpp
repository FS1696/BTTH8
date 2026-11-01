/****************/
/* Ma sinh vien: ................................ */
/* Ho ten: ....................................... */
/****************/
#include "DeviceMonitoringCoordinator.h"
#include <algorithm>

DeviceMonitoringCoordinator::DeviceMonitoringCoordinator(
    const std::vector<TeachingDevice*>& devices,
    ConsoleNotificationService& notificationService,
    DeviceAuditLogger& auditLogger,
    MaintenanceService& maintenanceService)
    : notificationService_(notificationService), auditLogger_(auditLogger),
      maintenanceService_(maintenanceService), subscribed_(false)
{
    // Loai bo con tro trung de tranh dang ky cung handler hai lan cho mot device.
    for (std::size_t i = 0; i < devices.size(); ++i)
    {
        if (devices[i] != NULL &&
            std::find(devices_.begin(), devices_.end(), devices[i]) == devices_.end())
            devices_.push_back(devices[i]);
    }
}

DeviceMonitoringCoordinator::~DeviceMonitoringCoordinator()
{
    Dispose();
}

void DeviceMonitoringCoordinator::Subscribe()
{
    if (subscribed_)
        return;

    subscriptions_.clear();
    for (std::size_t i = 0; i < devices_.size(); ++i)
    {
        TeachingDevice* device = devices_[i];
        SubscriptionInfo info;
        info.device = device;
        info.statusToken = device->SubscribeStatusChanged(
            [this](const TeachingDevice* sender, const DeviceStatusChangedEventArgs& args)
            {
                auditLogger_.HandleStatusChanged(sender, args);
            });
        info.notificationToken = device->SubscribeAlertRaised(
            [this](const TeachingDevice* sender, const DeviceAlertEventArgs& args)
            {
                notificationService_.HandleAlert(sender, args);
            });
        info.maintenanceToken = device->SubscribeAlertRaised(
            [this](const TeachingDevice* sender, const DeviceAlertEventArgs& args)
            {
                maintenanceService_.HandleCriticalAlert(sender, args);
            });
        subscriptions_.push_back(info);
    }
    subscribed_ = true;
}

void DeviceMonitoringCoordinator::Unsubscribe()
{
    if (!subscribed_)
        return;

    for (std::size_t i = 0; i < subscriptions_.size(); ++i)
    {
        SubscriptionInfo& info = subscriptions_[i];
        info.device->UnsubscribeStatusChanged(info.statusToken);
        info.device->UnsubscribeAlertRaised(info.notificationToken);
        info.device->UnsubscribeAlertRaised(info.maintenanceToken);
    }
    subscriptions_.clear();
    subscribed_ = false;
}

void DeviceMonitoringCoordinator::Dispose()
{
    Unsubscribe();
}

bool DeviceMonitoringCoordinator::IsSubscribed() const
{
    return subscribed_;
}
