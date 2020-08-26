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
