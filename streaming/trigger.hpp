#include <cstddef>
#include <functional>
#include <vector>

// A simple event handler
template<class DataType, class Func = std::function<void(const DataType&)>>
struct Trigger : public std::vector<Func> {
    using Parent = std::vector<Func>;

    Trigger() {}
    Trigger(std::nullptr_t nil) {}
    Trigger(const Func& func) {
        *this += func;
    }
    // Perfect forwarding so we can pass lambdas directly to Trigger.
    // C++ allows only 1 step conversion, so we need forwarding to do lambda -> func -> trigger conversion.
    template<typename... Args>
    Trigger(Args&&... args) {
        *this += Func(std::forward<Args>(args)...);
    }

    void operator+=(std::function<void(const DataType&)> fn) {
        Parent::push_back(fn);
    }

    void operator-=(std::function<void(const DataType&)> fn) {
        for (auto it = Parent::begin(); it != Parent::end(); it++) {
            if (*it == fn) {
                erase(it);
                break;
            }
        }
    }

    void operator()(const DataType& d) const {
        for (auto& fn : (*this)) {
            fn(d);
        }
    }

};
