/****************/
// Mã sinh viên: 202419074
// Họ tên: Vũ Thành Lâm
/****************/
#include "Subscribers.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "DeviceEnums.h"

void ConsoleNotificationService::HandleAlert(const TeachingDevice* sender, const DeviceAlertEventArgs& args) const
{
    (void)sender; // Sender duoc cung cap de subscriber co the truy vet publisher.
    std::cout << "[" << ToString(args.Severity) << "] "
              << args.DeviceId << ": " << args.Message << std::endl;
}

void DeviceAuditLogger::HandleStatusChanged(const TeachingDevice* sender, const DeviceStatusChangedEventArgs& args)
{
    (void)sender;
    std::ostringstream out;
    out << FormatTime(args.ChangedAt) << ": " << args.DeviceId << ", "
        << ToString(args.OldStatus) << " -> " << ToString(args.NewStatus);
    entries_.push_back(out.str());
}

const std::vector<std::string>& DeviceAuditLogger::GetEntries() const
{
    return entries_;
}

void MaintenanceService::HandleCriticalAlert(const TeachingDevice* sender, const DeviceAlertEventArgs& args)
{
    (void)sender;
    if (args.Severity != AlertSeverity::Critical)
        return;

    std::ostringstream out;
    out << "Maintenance request for " << args.DeviceId << ": " << args.Message;
    requests_.push_back(out.str());
}

const std::vector<std::string>& MaintenanceService::GetRequests() const
{
    return requests_;
}

FilteredAlertSubscriber::FilteredAlertSubscriber(const AlertFilter& filter,
                                                 const AlertAction& action)
    : filter_(filter), action_(action)
{
    if (!filter_ || !action_)
        throw std::invalid_argument("Filter and action must not be empty.");
}

void FilteredAlertSubscriber::HandleAlert(const TeachingDevice* sender, const DeviceAlertEventArgs& args) const
{
    (void)sender;
    if (filter_(args))
        action_(args);
}
