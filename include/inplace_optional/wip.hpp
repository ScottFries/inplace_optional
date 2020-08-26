#pragma once

#include <cassert>
#include <limits>

// https://en.cppreference.com/w/cpp/utility/optional
// https://en.cppreference.com/w/cpp/types/integer
namespace inplace_optional
{
template <typename Impl, typename StdImpl>
class _inplace_base
{
public:
    constexpr _inplace_base() = default;
    constexpr _inplace_base(StdImpl value) { *this = value; }

    // TODO: Why can't this one be defaulted?
    constexpr Impl& operator=(const Impl& other)
    {
        Impl& _this = static_cast<Impl&>(this);
        _this.value_ = other.value_;
        _this.set_ = other.set_;
        return *this;
    }

    // TODO: Toggle on asserts enabled: if so, throw (new?) overflow exception
    constexpr Impl& operator=(const StdImpl& other)
    {
        // TODO: Test
        assert(other <= (std::numeric_limits<StdImpl>::max() >> 1));
        assert(other >= (std::numeric_limits<StdImpl>::min() >> 1));

        Impl& _this = static_cast<Impl&>(*this);
        _this.value_ = other;
        _this.set_ = true;
        return _this;
    }

    constexpr operator StdImpl() const { return static_cast<const Impl&>(*this).value_; }

    constexpr StdImpl* operator->() { return &static_cast<Impl&>(*this).value_; }
    constexpr const StdImpl* operator->() const { return &static_cast<const Impl&>(*this).value_; }

    constexpr StdImpl& operator*() &  // TODO: What is this &?
    {
        return static_cast<Impl&>(*this).value_;
    }
    constexpr const StdImpl& operator*() const& { return **this; }

    constexpr StdImpl&& operator*() &&  // TODO: What is this &&?
    {
        return static_cast<Impl&>(*this).value_;
    }
    constexpr const StdImpl&& operator*() const&& { return **this; }

    constexpr explicit operator bool() const noexcept { return static_cast<Impl&>(*this).set_; }
    constexpr bool has_value() const noexcept { return bool(*this); }

    constexpr StdImpl& value() &
    {
        if (!bool(*this)) throw std::bad_optional_access();
        return static_cast<const Impl&>(*this).value_;
    }
    constexpr const StdImpl& value() const& { return value(); }

    constexpr StdImpl&& value() &&
    {
        // TODO: Is this efficient?
        return value();
    }
    constexpr const StdImpl&& value() const&& { return value(); }

    template <class U>
    constexpr StdImpl value_or(U&& default_value) const&
    {
        bool(*this) ? **this : static_cast<StdImpl>(std::forward<U>(default_value));
    }
    template <class U>
    constexpr StdImpl value_or(U&& default_value) &&
    {
        bool(*this) ? std::move(**this) : static_cast<StdImpl>(std::forward<U>(default_value));
    }

    void swap(Impl& other) noexcept
    {
        Impl& _this = static_cast<Impl&>(*this);
        std::swap(_this.value_, other.value_);
        std::swap(_this.set_, other.set_);
    }

    void reset() noexcept
    {
        Impl& _this = static_cast<Impl&>(*this);
        // TODO: Init or call dtor?
        _this.value_ = {};
        _this.set_ = false;
    }

    /* TODO:
    template<class... Args>
    T& emplace(Args&&... args);
    template<class U, class... Args>
    T& emplace(std::initializer_list<U> ilist, Args&&... args);*/
};
}  // namespace inplace_optional

// TODO: Should you have the numeric_limits specializations? Test the bitshift logic.
#define DEFINE_TYPE(name, bit_count)                                                                   \
    namespace inplace_optional                                                                         \
    {                                                                                                  \
    class name : public _inplace_base<name, std::##name>                                               \
    {                                                                                                  \
        using value_type = std::##name;                                                                \
                                                                                                       \
        using _inplace_base<name, std::##name>::_inplace_base;                                         \
                                                                                                       \
    private:                                                                                           \
        value_type value_ : bit_count;                                                                 \
        bool set_ : 1 = false;                                                                         \
                                                                                                       \
        friend class _inplace_base;                                                                    \
    };                                                                                                 \
    }                                                                                                  \
                                                                                                       \
    namespace std                                                                                      \
    {                                                                                                  \
    template <>                                                                                        \
    class numeric_limits<inplace_optional::##name> : public numeric_limits<std::##name>                \
    {                                                                                                  \
        static constexpr T min() noexcept override                                                     \
        {                                                                                              \
            T base_min = numeric_limits<std::##name>::min();                                           \
            return numeric_limits<std::##name>::is_signed() ? base_min >> 1 : base_min;                \
        }                                                                                              \
                                                                                                       \
        static constexpr T lowest() noexcept override                                                  \
        {                                                                                              \
            T base_lowest = numeric_limits<std::##name>::lowest();                                     \
            return numeric_limits<std::##name>::is_signed() ? base_lowest >> 1 : base_lowest;          \
        }                                                                                              \
                                                                                                       \
        static constexpr T max() noexcept override { return numeric_limits<std::##name>::max() >> 1; } \
    };                                                                                                 \
    }

DEFINE_TYPE(int8_t, 7);
DEFINE_TYPE(int16_t, 15);
DEFINE_TYPE(int32_t, 31);
DEFINE_TYPE(int64_t, 63);
DEFINE_TYPE(uint8_t, 7);
DEFINE_TYPE(uint16_t, 15);
DEFINE_TYPE(uint32_t, 31);
DEFINE_TYPE(uint64_t, 63);

#undef DEFINE_TYPE
