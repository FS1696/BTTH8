/****************/
/* Ma sinh vien: ................................ */
/* Ho ten: ....................................... */
/****************/
#include "TeachingDevice.h"
#include <stdexcept>
#include <ctime>

TeachingDevice::TeachingDevice(const std::string& id, const std::string& name)
    : id_(id), name_(name), status_(DeviceStatus::Available)
{
    if (id_.empty())
        throw std::invalid_argument("Device ID must not be empty.");
    if (name_.empty())
        throw std::invalid_argument("Device name must not be empty.");
}

TeachingDevice::~TeachingDevice()
{
}

const std::string& TeachingDevice::GetId() const { return id_; }
const std::string& TeachingDevice::GetName() const { return name_; }
DeviceStatus TeachingDevice::GetStatus() const { return status_; }

void TeachingDevice::ChangeStatus(DeviceStatus newStatus)
{
    // Khong phat event neu trang thai moi trung voi trang thai hien tai.
    if (newStatus == status_)
        return;

    const DeviceStatus oldStatus = status_;
    status_ = newStatus; // Cap nhat truoc khi phat su kien.

    DeviceStatusChangedEventArgs args(id_, oldStatus, newStatus, std::time(NULL));
    OnStatusChanged(args);
}

TeachingDevice::StatusSubscriptionId TeachingDevice::SubscribeStatusChanged(
    const Event<DeviceStatusChangedEventArgs>::Handler& handler)
{
    return statusChanged_.Subscribe(handler);
}

bool TeachingDevice::UnsubscribeStatusChanged(StatusSubscriptionId id)
{
    return statusChanged_.Unsubscribe(id);
}

TeachingDevice::AlertSubscriptionId TeachingDevice::SubscribeAlertRaised(
    const Event<DeviceAlertEventArgs>::Handler& handler)
{
    return alertRaised_.Subscribe(handler);
}

bool TeachingDevice::UnsubscribeAlertRaised(AlertSubscriptionId id)
{
    return alertRaised_.Unsubscribe(id);
}

void TeachingDevice::RaiseAlert(const std::string& message, AlertSeverity severity)
{
    DeviceAlertEventArgs args(id_, message, severity, std::time(NULL));
    OnAlertRaised(args);
}

void TeachingDevice::OnStatusChanged(const DeviceStatusChangedEventArgs& args)
{
    statusChanged_.Emit(this, args);
}

void TeachingDevice::OnAlertRaised(const DeviceAlertEventArgs& args)
{
    alertRaised_.Emit(this, args);
}
