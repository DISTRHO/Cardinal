//
//    ██████ ██   ██  ██████   ██████
//   ██      ██   ██ ██    ██ ██            ** Clean Header-Only Classes **
//   ██      ███████ ██    ██ ██
//   ██      ██   ██ ██    ██ ██           https://github.com/Tracktion/choc
//    ██████ ██   ██  ██████   ██████
//
//   CHOC is (C)2021 Tracktion Corporation, and is offered under the terms of the ISC license:
//
//   Permission to use, copy, modify, and/or distribute this software for any purpose with or
//   without fee is hereby granted, provided that the above copyright notice and this permission
//   notice appear in all copies. THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
//   WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
//   AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
//   CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//   WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
//   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#ifndef CHOC_SPAN_HEADER_INCLUDED
#define CHOC_SPAN_HEADER_INCLUDED

#include <vector>
#include "DistrhoUtils.hpp"

namespace choc
{

//==============================================================================
/** This is a temporary stunt-double for std::span, with the intention of it being
    deprecated when there's more widespread compiler support for the real std::span.

    This class has fewer bells-and-whistles than a real std::span, but it does have
    the advantage of calling CHOC_ASSERT when mistakes are made like out-of-range
    accesses, which can be useful for getting clean error handling rather than UB.
*/
template <typename Item>
struct span
{
    span() = default;
    span (const span&) = default;
    span (span&&) = default;
    span& operator= (span&&) = default;
    span& operator= (const span&) = default;

    /// Construct from some raw start and end pointers. For an empty span, these
    /// can both be nullptr, but if one is a real pointer then the caller must ensure
    /// that start <= end.
    span (Item* start, Item* end) noexcept  : s (start), e (end) {}

    /// Constructs a span from a pointer and length.
    /// The pointer must not be nullptr unless the length is 0.
    span (const Item* start, size_t length) noexcept  : span (const_cast<Item*> (start), const_cast<Item*> (start) + length) {}

    /// Constructor taking a raw C++ array.
    template <size_t length>
    span (Item (&array)[length])  : span (array, length) {}

    /// Constructor which takes some kind of class like std::vector or std::array.
    /// Any class that provides data() and size() methods can be passed in.
    template <typename VectorOrArray>
    span (const VectorOrArray& v)  : span (v.data(), v.size()) {}

    /// Returns true if the span is empty.
    bool empty() const                              { return s == e; }

    /// Returns the number of elements.
    /// The length() and size() methods are equivalent.
    size_t size() const                             { return static_cast<size_t> (e - s); }

    /// Returns the number of elements.
    /// The length() and size() methods are equivalent.
    size_t length() const                           { return static_cast<size_t> (e - s); }

    /// Returns a raw pointer to the start of the data.
    Item* data() const noexcept                     { return s; }

    const Item& front() const                       { DISTRHO_SAFE_ASSERT_RETURN (! empty(), _nullValue()); return *s; }
    const Item& back() const                        { DISTRHO_SAFE_ASSERT_RETURN (! empty(), _nullValue()); return *(e - 1); }
    Item& front()                                   { DISTRHO_SAFE_ASSERT_RETURN (! empty(), _nullValue()); return *s; }
    Item& back()                                    { DISTRHO_SAFE_ASSERT_RETURN (! empty(), _nullValue()); return *(e - 1); }

    const Item& operator[] (size_t index) const     { DISTRHO_SAFE_ASSERT_RETURN (index < length(), _nullValue()); return s[index]; }
    Item& operator[] (size_t index)                 { DISTRHO_SAFE_ASSERT_RETURN (index < length(), _nullValue()); return s[index]; }

    /// A handy bonus function for getting a (non-empty) span's tail elements
    span tail() const                               { DISTRHO_SAFE_ASSERT_RETURN (! empty(), _nullValue()); return { s + 1, e }; }

    const Item* begin() const noexcept              { return s; }
    const Item* end() const noexcept                { return e; }
    Item* begin() noexcept                          { return s; }
    Item* end() noexcept                            { return e; }

    /// Helper function to return a std::vector copy of the span's elements.
    std::vector<typename std::remove_const<Item>::type> createVector() const
    {
        return std::vector<typename std::remove_const<Item>::type> (s, e);
    }

    /// Two spans are considered identical if their elements are all comparable
    template <typename OtherSpan>
    bool operator== (const OtherSpan& other) const
    {
        auto sz = size();

        if (sz != other.size())
            return false;

        for (decltype (sz) i = 0; i < sz; ++i)
            if (s[i] != other.s[i])
                return false;

        return true;
    }

    template <typename OtherSpan>
    bool operator!= (const OtherSpan& other) const  { return ! operator== (other); }

private:
    Item* s = {};
    Item* e = {};

    static inline Item& _nullValue() noexcept { static Item e = {}; return e; }
};

} // namespace choc

#endif // CHOC_SPAN_HEADER_INCLUDED
