/****************/
/* Ma sinh vien: ................................ */
/* Ho ten: ....................................... */
/****************/
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include "Devices.h"
#include "Exceptions.h"
#include "Subscribers.h"
#include "DeviceMonitoringCoordinator.h"

// Bo kiem thu don gian: moi test doc lap va in PASS/FAIL.
static int passedTests = 0;
static int failedTests = 0;

template <typename TFunction>
void RunTest(const std::string& name, const TFunction& test)
{
    try
    {
        test();
        ++passedTests;
        std::cout << "[PASS] " << name << std::endl;
    }
    catch (const std::exception& ex)
    {
        ++failedTests;
        std::cout << "[FAIL] " << name << " - " << ex.what() << std::endl;
    }
}

static void Require(bool condition, const std::string& message)
{
    if (!condition)
        throw std::runtime_error(message);
}

int main()
{
    std::cout << "========== W08 - DELEGATE & EVENT ==========" << std::endl;

    RunTest("1. ChangeStatus fires once and updates status before event", []()
    {
        ProjectorRemote device("PR01", "Projector remote", 80);
        int count = 0;
        DeviceStatus observed = DeviceStatus::Available;
        bool eventDataValid = false;
        device.SubscribeStatusChanged([&](const TeachingDevice*, const DeviceStatusChangedEventArgs& args)
        {
            ++count;
            observed = device.GetStatus();
            eventDataValid = (args.OldStatus == DeviceStatus::Available &&
                              args.NewStatus == DeviceStatus::Borrowed);
        });
        device.ChangeStatus(DeviceStatus::Borrowed);
        device.ChangeStatus(DeviceStatus::Borrowed); // Khong tao event lan hai.
        Require(count == 1, "Status event should be raised exactly once.");
        Require(observed == DeviceStatus::Borrowed, "Status was not updated before event.");
        Require(eventDataValid, "Event arguments contain incorrect old/new status.");
    });

    RunTest("2. Projector warning and critical alerts do not repeat in same region", []()
    {
        ProjectorRemote device("PR02", "Projector remote", 60);
        std::vector<AlertSeverity> received;
        device.SubscribeAlertRaised([&](const TeachingDevice*, const DeviceAlertEventArgs& args)
        {
            received.push_back(args.Severity);
        });
        device.UpdateBatteryLevel(19);
        device.UpdateBatteryLevel(15); // Van o vung Warning.
        device.UpdateBatteryLevel(0);
        device.UpdateBatteryLevel(0);  // Van o vung Critical.
        device.UpdateBatteryLevel(40); // Tro lai vung an toan.
        Require(received.size() == 2, "Unexpected number of projector alerts.");
        Require(received[0] == AlertSeverity::Warning, "Missing Warning alert.");
        Require(received[1] == AlertSeverity::Critical, "Missing Critical alert.");
    });

    RunTest("3. Air conditioner alerts only on responsive-to-unresponsive transition", []()
    {
        AirConditionerRemote device("AC01", "AC remote", "A203");
        int count = 0;
        bool severityValid = true;
        device.SubscribeAlertRaised([&](const TeachingDevice*, const DeviceAlertEventArgs& args)
        {
            if (args.Severity != AlertSeverity::Critical) severityValid = false;
            ++count;
        });
        device.UpdateResponseState(false);
        device.UpdateResponseState(false);
        device.UpdateResponseState(true);
        Require(count == 1, "Air conditioner alert should be raised once.");
        Require(severityValid, "Air conditioner alert should be Critical.");
    });

    RunTest("4. Microphone battery and signal thresholds", []()
    {
        WirelessMicrophone device("MIC01", "Wireless microphone", 80, 80);
        std::vector<AlertSeverity> received;
        device.SubscribeAlertRaised([&](const TeachingDevice*, const DeviceAlertEventArgs& args)
        {
            received.push_back(args.Severity);
        });
        device.UpdateBatteryLevel(29);
        device.UpdateBatteryLevel(20); // Khong lap lai Warning cua pin.
        device.UpdateBatteryLevel(0);
        device.UpdateSignalQuality(39);
        device.UpdateSignalQuality(0);
        Require(received.size() == 4, "Unexpected microphone alert count.");
        Require(received[0] == AlertSeverity::Warning, "Battery Warning missing.");
        Require(received[1] == AlertSeverity::Critical, "Battery Critical missing.");
        Require(received[2] == AlertSeverity::Warning, "Signal Warning missing.");
        Require(received[3] == AlertSeverity::Critical, "Signal Critical missing.");
    });

    RunTest("5. Invalid battery reading throws custom exception", []()
    {
        ProjectorRemote device("PR03", "Projector remote", 50);
        bool thrown = false;
        try { device.UpdateBatteryLevel(101); }
        catch (const InvalidDeviceReadingException& ex)
        {
            thrown = true;
            Require(ex.GetDeviceId() == "PR03", "Wrong device ID in exception.");
            Require(ex.GetInvalidValue() == 101, "Wrong invalid value in exception.");
        }
        Require(thrown, "InvalidDeviceReadingException was not thrown.");
    });

    RunTest("6. Invalid microphone signal reading throws custom exception", []()
    {
        WirelessMicrophone device("MIC02", "Wireless microphone", 70, 70);
        bool thrown = false;
        try { device.UpdateSignalQuality(-1); }
        catch (const InvalidDeviceReadingException& ex)
        {
            thrown = true;
            Require(ex.GetInvalidValue() == -1, "Wrong invalid value in exception.");
        }
        Require(thrown, "InvalidDeviceReadingException was not thrown.");
    });

    RunTest("7. Multiple subscribers receive the same alert", []()
    {
        ProjectorRemote device("PR04", "Projector remote", 90);
        int first = 0;
        int second = 0;
        device.SubscribeAlertRaised([&](const TeachingDevice*, const DeviceAlertEventArgs&) { ++first; });
        device.SubscribeAlertRaised([&](const TeachingDevice*, const DeviceAlertEventArgs&) { ++second; });
        device.UpdateBatteryLevel(10);
        Require(first == 1 && second == 1, "Not all subscribers received the event.");
    });

    RunTest("8. Maintenance service stores Critical alerts only", []()
    {
        MaintenanceService maintenance;
        WirelessMicrophone device("MIC03", "Wireless microphone", 80, 80);
        device.SubscribeAlertRaised([&](const TeachingDevice* sender, const DeviceAlertEventArgs& args)
        {
            maintenance.HandleCriticalAlert(sender, args);
        });
        device.UpdateBatteryLevel(20); // Warning, khong tao yeu cau bao tri.
        device.UpdateBatteryLevel(0);  // Critical, co tao yeu cau.
        Require(maintenance.GetRequests().size() == 1,
                "Maintenance service must store exactly one Critical request.");
    });

    RunTest("9. FilteredAlertSubscriber handles Critical alerts only", []()
    {
        ProjectorRemote device("PR05", "Projector remote", 90);
        int handled = 0;
        FilteredAlertSubscriber filtered(
            [](const DeviceAlertEventArgs& args)
            {
                return args.Severity == AlertSeverity::Critical;
            },
            [&](const DeviceAlertEventArgs&) { ++handled; });
        device.SubscribeAlertRaised([&](const TeachingDevice* sender, const DeviceAlertEventArgs& args)
        {
            filtered.HandleAlert(sender, args);
        });
        device.UpdateBatteryLevel(10); // Warning.
        device.UpdateBatteryLevel(0);  // Critical.
        Require(handled == 1, "Filter should pass only Critical alerts.");
    });

    RunTest("10. Coordinator avoids duplicate subscriptions and unsubscribes", []()
    {
        ProjectorRemote device("PR06", "Projector remote", 80);
        ConsoleNotificationService notification;
        DeviceAuditLogger audit;
        MaintenanceService maintenance;
        std::vector<TeachingDevice*> devices;
        devices.push_back(&device);
        devices.push_back(&device); // Con tro trung duoc loai bo.
        DeviceMonitoringCoordinator coordinator(devices, notification, audit, maintenance);
        coordinator.Subscribe();
        coordinator.Subscribe(); // Goi lan hai khong dang ky trung.
        device.ChangeStatus(DeviceStatus::Borrowed);
        Require(audit.GetEntries().size() == 1, "Duplicate status subscription detected.");
        coordinator.Unsubscribe();
        device.ChangeStatus(DeviceStatus::Available);
        Require(audit.GetEntries().size() == 1, "Handler still active after unsubscribe.");
    });

    RunTest("11. Coordinator Dispose unsubscribes automatically", []()
    {
        ProjectorRemote device("PR07", "Projector remote", 80);
        ConsoleNotificationService notification;
        DeviceAuditLogger audit;
        MaintenanceService maintenance;
        std::vector<TeachingDevice*> devices(1, &device);
        {
            DeviceMonitoringCoordinator coordinator(devices, notification, audit, maintenance);
            coordinator.Subscribe();
            Require(coordinator.IsSubscribed(), "Coordinator should be subscribed.");
        }
        device.ChangeStatus(DeviceStatus::Borrowed);
        Require(audit.GetEntries().empty(), "Coordinator destructor did not unsubscribe.");
    });

    RunTest("12. A failing subscriber does not block later subscribers", []()
    {
        ProjectorRemote device("PR08", "Projector remote", 90);
        int laterSubscriber = 0;
        device.SubscribeAlertRaised([](const TeachingDevice*, const DeviceAlertEventArgs&)
        {
            throw std::runtime_error("Intentional subscriber failure");
        });
        device.SubscribeAlertRaised([&](const TeachingDevice*, const DeviceAlertEventArgs&) { ++laterSubscriber; });
        device.UpdateBatteryLevel(10);
        Require(laterSubscriber == 1, "Later subscriber was blocked by an earlier failure.");
    });

    RunTest("13. Audit logger records old and new status", []()
    {
        ProjectorRemote device("PR09", "Projector remote", 80);
        DeviceAuditLogger audit;
        device.SubscribeStatusChanged([&](const TeachingDevice* sender, const DeviceStatusChangedEventArgs& args)
        {
            audit.HandleStatusChanged(sender, args);
        });
        device.ChangeStatus(DeviceStatus::UnderMaintenance);
        Require(audit.GetEntries().size() == 1, "Audit entry was not created.");
        Require(audit.GetEntries()[0].find("Available -> UnderMaintenance") != std::string::npos,
                "Audit entry does not contain old/new status.");
    });

    RunTest("14. Subscriber can be removed by its subscription ID", []()
    {
        ProjectorRemote device("PR10", "Projector remote", 90);
        int count = 0;
        TeachingDevice::AlertSubscriptionId token = device.SubscribeAlertRaised(
            [&](const TeachingDevice*, const DeviceAlertEventArgs&) { ++count; });
        Require(device.UnsubscribeAlertRaised(token), "Unsubscribe should return true.");
        device.UpdateBatteryLevel(10);
        Require(count == 0, "Removed subscriber was still called.");
    });

    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "Test result: " << passedTests << " PASS, "
              << failedTests << " FAIL" << std::endl;
    std::cout << "========== END W08 TESTS ==========" << std::endl;

    return failedTests == 0 ? 0 : 1;
}
