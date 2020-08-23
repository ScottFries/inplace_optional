[![Actions Status](https://github.com/ScottFries/inplace_optional/workflows/MacOS/badge.svg)](https://github.com/ScottFries/inplace_optional/actions)
[![Actions Status](https://github.com/ScottFries/inplace_optional/workflows/Windows/badge.svg)](https://github.com/ScottFries/inplace_optional/actions)
[![Actions Status](https://github.com/ScottFries/inplace_optional/workflows/Ubuntu/badge.svg)](https://github.com/ScottFries/inplace_optional/actions)
[![Actions Status](https://github.com/ScottFries/inplace_optional/workflows/Style/badge.svg)](https://github.com/ScottFries/inplace_optional/actions)
[![Actions Status](https://github.com/ScottFries/inplace_optional/workflows/Install/badge.svg)](https://github.com/ScottFries/inplace_optional/actions)
[![codecov](https://codecov.io/gh/ScottFries/inplace_optional/branch/master/graph/badge.svg)](https://codecov.io/gh/ScottFries/inplace_optional)

# inplace_optional

C++17 finally brings the amazing [std::optional](https://en.cppreference.com/w/cpp/utility/optional) type, but it has implementation details that impose significant memory overhead when using it with primitive types.
inplace_optional is my attempt at offering an alternative implementation for cases when the memory overhead of std::optional isn't acceptable.

## Background

`std::optional` is typically implemented as a data object containing a `bool` and the templated type the optional holds.
``` c++
template <typename T>
class optional
{
  std::aligned_storage_t<sizeof(T), alignof(T)> _storage;
  bool _initialized;
};
```
While this implementation is simple enough, it's memory implications can be insidious. Due to alignment rules, the measely 1 byte of storage the `bool` requires has padding (wasted space).
In the case of holding an optional of a custom type that holds more than a couple of primitives, this is negligable and will only surface as up to 7 bytes of padding. In the more minute case of primitive types however, using an optional requires _twice the memory footprint_.
[An example of all of these can be found on this Godbolt.](https://gcc.godbolt.org/z/4osE7P)

## inplace_optional Approach

While the standard implementors (rightfully) opted for this 'memory wasteful' approach, this is not required in many use cases. Not only do we not need the (up to) 7 bytes of padding, we don't even need to waste 1 byte on the `bool`! A `bool` is simply a 1 _bit_ indicator; the fact that `bool` values type up an entire byte is yet again due to alignment rules.
Luckily, we can use this to our advantage by bundling our `bool` into the memory space of our primitive value.

### Bit Fields

C++ allows us to explicitly state the bit size of integral values via [bit fields](https://en.cppreference.com/w/cpp/language/bit_field). A common usecase for this I've seen is packing multiple `bool`s into a single byte, but in inplace_optional, I use it to 'steal' a bit from the primitive value the type represents.
In short, this means that the memory usage of an inplace_optional value is _the same as_ it's normal counterpart, but the value's representable range is reduced by half.
Put differently, `std::optional` sacrifices memory to represent an optional value, while the values in inplace_optional sacrifices the scale of data that can be represented.

### Justification

While the idea of effectively turning e.g. a `uint16_t` into an 'optional uint15_t' is no doubt alarming, it also makes a lot of sense in many (perhaps even most) cases. If you know that your 16 bit value will never use its 16th bit, this type effectively comes free of cost. If your value does/may truly need 16 bits, then using an `inplace_optional::uint32_t` comes at the same cost as using an `std::optional<uint16_t>`, and gives you another 16 bits to work with!

#### Caveat

While the cost of memory is free in this case, I have yet to explore how this packing may effect performance, specifically when taking things such as SIMD into account.

## Usage

inplace_optional was modeled after the API of std::optional and can be used as a drop-in alternative.
TODO: Additionally, I've added 

## Coming soon


