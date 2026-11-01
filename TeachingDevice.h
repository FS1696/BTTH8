/****************/
/* Ma sinh vien: ................................ */
/* Ho ten: ....................................... */
/****************/
#ifndef TEACHING_DEVICE_H
#define TEACHING_DEVICE_H

#include <string>
#include "DeviceEnums.h"
#include "DeviceEventArgs.h"
#include "Event.h"

// Lop co so quan ly thong tin chung va cong bo cac su kien cua thiet bi.
// Lop khong phu thuoc truc tiep vao logger, giao dien hay dich vu bao tri.
class TeachingDevice
{
public:
    typedef Event<DeviceStatusChangedEventArgs>::SubscriptionId StatusSubscriptionId;
    typedef Event<DeviceAlertEventArgs>::SubscriptionId AlertSubscriptionId;

    TeachingDevice(const std::string& id, const std::string& name);
    virtual ~TeachingDevice() = 0;

    const std::string& GetId() const;
    const std::string& GetName() const;
    DeviceStatus GetStatus() const;

    // Thay doi trang thai va phat StatusChanged neu gia tri thuc su thay doi.
    void ChangeStatus(DeviceStatus newStatus);

    // API dang ky/huy dang ky su kien; khong cung cap API phat su kien ra ngoai.
    StatusSubscriptionId SubscribeStatusChanged(
        const Event<DeviceStatusChangedEventArgs>::Handler& handler);
    bool UnsubscribeStatusChanged(StatusSubscriptionId id);

    AlertSubscriptionId SubscribeAlertRaised(
        const Event<DeviceAlertEventArgs>::Handler& handler);
    bool UnsubscribeAlertRaised(AlertSubscriptionId id);

protected:
    // Lop dan xuat goi RaiseAlert khi phat hien dieu kien can canh bao.
    void RaiseAlert(const std::string& message, AlertSeverity severity);

    // Hai diem mo rong protected virtual theo yeu cau de bai.
    virtual void OnStatusChanged(const DeviceStatusChangedEventArgs& args);
    virtual void OnAlertRaised(const DeviceAlertEventArgs& args);

private:
    std::string id_;
    std::string name_;
    DeviceStatus status_;

    Event<DeviceStatusChangedEventArgs> statusChanged_;
    Event<DeviceAlertEventArgs> alertRaised_;
};

#endif // TEACHING_DEVICE_H
