/****************/
/* Ma sinh vien: ................................ */
/* Ho ten: ....................................... */
/****************/
#ifndef EVENT_H
#define EVENT_H

#include <cstddef>
#include <exception>
#include <functional>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

class TeachingDevice;

// Lop Event mo phong event/delegate cua C# bang std::function.
// Ben ngoai chi duoc Subscribe/Unsubscribe; ham Emit la rieng tu.
template <typename TEventArgs>
class Event
{
public:
    typedef std::function<void(const TeachingDevice*, const TEventArgs&)> Handler;
    typedef std::size_t SubscriptionId;

    Event() : nextId_(1) {}

    // Dang ky mot subscriber va tra ve ma de huy dang ky sau nay.
    SubscriptionId Subscribe(const Handler& handler)
    {
        if (!handler)
            throw std::invalid_argument("Event handler must not be empty.");

        const SubscriptionId id = nextId_++;
        handlers_[id] = handler;
        return id;
    }

    // Huy dang ky theo ma da nhan tu Subscribe.
    bool Unsubscribe(SubscriptionId id)
    {
        return handlers_.erase(id) > 0;
    }

    std::size_t SubscriberCount() const
    {
        return handlers_.size();
    }

private:
    std::map<SubscriptionId, Handler> handlers_;
    SubscriptionId nextId_;

    // Sao chep danh sach handler truoc khi phat de viec them/xoa subscriber
    // trong luc xu ly khong lam vo iterator cua danh sach hien tai.
    // Loi tu mot subscriber duoc ghi nhan va khong ngan subscriber khac chay.
    void Emit(const TeachingDevice* sender, const TEventArgs& args) const
    {
        std::vector<Handler> snapshot;
        typename std::map<SubscriptionId, Handler>::const_iterator it;
        for (it = handlers_.begin(); it != handlers_.end(); ++it)
            snapshot.push_back(it->second);

        for (std::size_t i = 0; i < snapshot.size(); ++i)
        {
            try
            {
                snapshot[i](sender, args);
            }
            catch (const std::exception& ex)
            {
                std::cerr << "[EVENT HANDLER ERROR] " << ex.what() << std::endl;
            }
            catch (...)
            {
                std::cerr << "[EVENT HANDLER ERROR] Unknown exception." << std::endl;
            }
        }
    }

    friend class TeachingDevice;
};

#endif // EVENT_H
