/****************/
/* Ma sinh vien: ................................ */
/* Ho ten: ....................................... */
/****************/
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>
#include <sstream>

// Ngoai le khi pin hoac chat luong tin hieu nam ngoai [0, 100].
class InvalidDeviceReadingException : public std::runtime_error
{
private:
    std::string deviceId_;
    int invalidValue_;

    static std::string BuildMessage(const std::string& deviceId, int value)
    {
        std::ostringstream out;
        out << "Invalid reading for device " << deviceId
            << ": " << value << " (expected 0..100).";
        return out.str();
    }

public:
    InvalidDeviceReadingException(const std::string& deviceId, int invalidValue)
        : std::runtime_error(BuildMessage(deviceId, invalidValue)),
          deviceId_(deviceId), invalidValue_(invalidValue)
    {
    }

    const std::string& GetDeviceId() const { return deviceId_; }
    int GetInvalidValue() const { return invalidValue_; }
};

// Ham dung chung de kiem tra cac gia tri phan tram.
inline void ValidatePercentage(const std::string& deviceId, int value)
{
    if (value < 0 || value > 100)
        throw InvalidDeviceReadingException(deviceId, value);
}

#endif // EXCEPTIONS_H
