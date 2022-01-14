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

#ifndef CHOC_SMALLVECTOR_HEADER_INCLUDED
#define CHOC_SMALLVECTOR_HEADER_INCLUDED

#include <algorithm>
#include "choc_Span.h"

namespace choc
{

/**
    A std::vector-style container class, which uses some pre-allocated storage
    to avoid heap allocation when the number of elements is small.

    Inspired by LLVM's SmallVector, I've found this to be handy in many situations
    where you know there's only likely to be a small or fixed number of elements,
    and where performance is important.

    It retains most of the same basic methods as std::vector, but without some of
    the more exotic tricks that the std library uses, just to avoid things getting
    too complicated.
*/
template <typename ElementType, size_t numPreallocatedElements>
struct SmallVector
{
    using value_type       = ElementType;
    using reference        = ElementType&;
    using const_reference  = const ElementType&;
    using iterator         = ElementType*;
    using const_iterator   = const ElementType*;
    using size_type        = size_t;

    SmallVector() noexcept;
    ~SmallVector() noexcept;

    SmallVector (SmallVector&&) noexcept;
    SmallVector (const SmallVector&);
    SmallVector& operator= (SmallVector&&) noexcept;
    SmallVector& operator= (const SmallVector&);

    /// Creates a SmallVector as a copy of some kind of iterable container.
    template <typename VectorType>
    SmallVector (const VectorType& initialContent);

    /// Replaces the contents of this vector with a copy of some kind of iterable container.
    template <typename VectorType>
    SmallVector& operator= (const VectorType&);

    reference        operator[] (size_type index);
    const_reference  operator[] (size_type index) const;

    value_type* data() const noexcept;

    const_iterator  begin() const noexcept;
    const_iterator  end() const noexcept;
    const_iterator  cbegin() const noexcept;
    const_iterator  cend() const noexcept;
    iterator        begin() noexcept;
    iterator        end() noexcept;

    const_reference front() const;
    reference       front();
    const_reference back() const;
    reference       back();

    bool empty() const noexcept;
    size_type size() const noexcept;
    size_type length() const noexcept;
    size_type capacity() const noexcept;

    bool contains (const ElementType&) const;

    void clear() noexcept;
    void resize (size_type newSize);
    void reserve (size_type requiredNumElements);

    void push_back (const value_type&);
    void push_back (value_type&&);

    /// Handy method to add multiple elements with a single push_back call.
    template <typename... Others>
    void push_back (const value_type& first, Others&&... others);

    template <typename... ConstructorArgs>
    void emplace_back (ConstructorArgs&&... args);

    void pop_back();

    void insert (iterator insertPosition, const value_type& valueToInsert);
    void insert (iterator insertPosition, value_type&& valueToInsert);

    void erase (iterator startPosition);
    void erase (iterator startPosition, iterator endPosition);

    bool operator== (span<value_type>) const;
    bool operator!= (span<value_type>) const;

private:
    value_type* elements;
    size_type numElements = 0, numAllocated = numPreallocatedElements;
    uint64_t internalStorage[(numPreallocatedElements * sizeof (value_type) + sizeof (uint64_t) - 1) / sizeof (uint64_t)];

    void shrink (size_type);
    value_type* getInternalStorage() noexcept      { return reinterpret_cast<value_type*> (internalStorage); }
    bool isUsingInternalStorage() const noexcept   { return numAllocated <= numPreallocatedElements; }
    void resetToInternalStorage() noexcept;
    void freeHeapAndResetToInternalStorage() noexcept;

    static inline ElementType& _nullValue() noexcept { static ElementType e = {}; return e; }
};



//==============================================================================
//        _        _           _  _
//     __| |  ___ | |_   __ _ (_)| | ___
//    / _` | / _ \| __| / _` || || |/ __|
//   | (_| ||  __/| |_ | (_| || || |\__ \ _  _  _
//    \__,_| \___| \__| \__,_||_||_||___/(_)(_)(_)
//
//   Code beyond this point is implementation detail...
//
//==============================================================================

template <typename ElementType, size_t preSize>
SmallVector<ElementType, preSize>::SmallVector() noexcept  : elements (getInternalStorage())
{
}

template <typename ElementType, size_t preSize>
SmallVector<ElementType, preSize>::~SmallVector() noexcept
{
    clear();
}

template <typename ElementType, size_t preSize>
SmallVector<ElementType, preSize>::SmallVector (const SmallVector& other)  : SmallVector()
{
    operator= (other);
}

template <typename ElementType, size_t preSize>
template <typename VectorType>
SmallVector<ElementType, preSize>::SmallVector (const VectorType& initialContent)  : SmallVector()
{
    reserve (initialContent.size());

    for (auto& i : initialContent)
        emplace_back (i);
}

template <typename ElementType, size_t preSize>
SmallVector<ElementType, preSize>::SmallVector (SmallVector&& other) noexcept
{
    if (other.isUsingInternalStorage())
    {
        elements = getInternalStorage();
        numElements = other.numElements;

        for (size_type i = 0; i < numElements; ++i)
            new (elements + i) value_type (std::move (other.elements[i]));
    }
    else
    {
        elements = other.elements;
        numElements = other.numElements;
        numAllocated = other.numAllocated;
        other.resetToInternalStorage();
        other.numElements = 0;
    }
}

template <typename ElementType, size_t preSize>
SmallVector<ElementType, preSize>& SmallVector<ElementType, preSize>::operator= (SmallVector&& other) noexcept
{
    clear();

    if (other.isUsingInternalStorage())
    {
        numElements = other.numElements;

        for (size_type i = 0; i < numElements; ++i)
            new (elements + i) value_type (std::move (other.elements[i]));
    }
    else
    {
        elements = other.elements;
        numElements = other.numElements;
        numAllocated = other.numAllocated;
        other.resetToInternalStorage();
        other.numElements = 0;
    }

    return *this;
}

template <typename ElementType, size_t preSize>
SmallVector<ElementType, preSize>& SmallVector<ElementType, preSize>::operator= (const SmallVector& other)
{
    if (other.size() > numElements)
    {
        reserve (other.size());

        for (size_type i = 0; i < numElements; ++i)
            elements[i] = other.elements[i];

        for (size_type i = numElements; i < other.size(); ++i)
            new (elements + i) value_type (other.elements[i]);

        numElements = other.size();
    }
    else
    {
        shrink (other.size());

        for (size_type i = 0; i < numElements; ++i)
            elements[i] = other.elements[i];
    }

    return *this;
}

template <typename ElementType, size_t preSize>
template <typename VectorType>
SmallVector<ElementType, preSize>& SmallVector<ElementType, preSize>::operator= (const VectorType& other)
{
    if (other.size() > numElements)
    {
        reserve (other.size());

        for (size_type i = 0; i < numElements; ++i)
            elements[i] = other[i];

        for (size_type i = numElements; i < other.size(); ++i)
            new (elements + i) value_type (other[i]);

        numElements = other.size();
    }
    else
    {
        shrink (other.size());

        for (size_type i = 0; i < numElements; ++i)
            elements[i] = other[i];
    }

    return *this;
}

template <typename ElementType, size_t preSize>
void SmallVector<ElementType, preSize>::resetToInternalStorage() noexcept
{
    elements = getInternalStorage();
    numAllocated = preSize;
}

template <typename ElementType, size_t preSize>
void SmallVector<ElementType, preSize>::freeHeapAndResetToInternalStorage() noexcept
{
    if (! isUsingInternalStorage())
    {
        delete[] reinterpret_cast<char*> (elements);
        resetToInternalStorage();
    }
}

template <typename ElementType, size_t preSize>
typename SmallVector<ElementType, preSize>::reference SmallVector<ElementType, preSize>::operator[] (size_type index)
{
    DISTRHO_SAFE_ASSERT_RETURN (index < numElements, _nullValue());
    return elements[index];
}

template <typename ElementType, size_t preSize>
typename SmallVector<ElementType, preSize>::const_reference SmallVector<ElementType, preSize>::operator[] (size_type index) const
{
    DISTRHO_SAFE_ASSERT_RETURN (index < numElements, _nullValue());
    return elements[index];
}

template <typename ElementType, size_t preSize>
typename SmallVector<ElementType, preSize>::value_type* SmallVector<ElementType, preSize>::data() const noexcept      { return elements; }
template <typename ElementType, size_t preSize>
typename SmallVector<ElementType, preSize>::const_iterator SmallVector<ElementType, preSize>::begin() const noexcept  { return elements; }
template <typename ElementType, size_t preSize>
typename SmallVector<ElementType, preSize>::const_iterator SmallVector<ElementType, preSize>::end() const noexcept    { return elements + numElements; }
template <typename ElementType, size_t preSize>
typename SmallVector<ElementType, preSize>::const_iterator SmallVector<ElementType, preSize>::cbegin() const noexcept { return elements; }
template <typename ElementType, size_t preSize>
typename SmallVector<ElementType, preSize>::const_iterator SmallVector<ElementType, preSize>::cend() const noexcept   { return elements + numElements; }
template <typename ElementType, size_t preSize>
typename SmallVector<ElementType, preSize>::iterator SmallVector<ElementType, preSize>::begin() noexcept              { return elements; }
template <typename ElementType, size_t preSize>
typename SmallVector<ElementType, preSize>::iterator SmallVector<ElementType, preSize>::end() noexcept                { return elements + numElements; }

template <typename ElementType, size_t preSize>
typename SmallVector<ElementType, preSize>::reference SmallVector<ElementType, preSize>::front()
{
    DISTRHO_SAFE_ASSERT_RETURN (! empty(), _nullValue());
    return elements[0];
}

template <typename ElementType, size_t preSize>
typename SmallVector<ElementType, preSize>::const_reference SmallVector<ElementType, preSize>::front() const
{
    DISTRHO_SAFE_ASSERT_RETURN (! empty(), _nullValue());
    return elements[0];
}

template <typename ElementType, size_t preSize>
typename SmallVector<ElementType, preSize>::reference SmallVector<ElementType, preSize>::back()
{
    DISTRHO_SAFE_ASSERT_RETURN (! empty(), _nullValue());
    return elements[numElements - 1];
}

template <typename ElementType, size_t preSize>
typename SmallVector<ElementType, preSize>::const_reference SmallVector<ElementType, preSize>::back() const
{
    DISTRHO_SAFE_ASSERT_RETURN (! empty(), _nullValue());
    return elements[numElements - 1];
}

template <typename ElementType, size_t preSize>
typename SmallVector<ElementType, preSize>::size_type SmallVector<ElementType, preSize>::size() const noexcept      { return numElements; }

template <typename ElementType, size_t preSize>
typename SmallVector<ElementType, preSize>::size_type SmallVector<ElementType, preSize>::length() const noexcept    { return numElements; }

template <typename ElementType, size_t preSize>
typename SmallVector<ElementType, preSize>::size_type SmallVector<ElementType, preSize>::capacity() const noexcept  { return numAllocated; }

template <typename ElementType, size_t preSize>
bool SmallVector<ElementType, preSize>::empty() const noexcept      { return numElements == 0; }

template <typename ElementType, size_t preSize>
bool SmallVector<ElementType, preSize>::contains (const ElementType& target) const
{
    for (size_t i = 0; i < numElements; ++i)
        if (elements[i] == target)
            return true;

    return false;
}

template <typename ElementType, size_t preSize>
bool SmallVector<ElementType, preSize>::operator== (span<value_type> other) const   { return span<value_type> (*this) == other; }

template <typename ElementType, size_t preSize>
bool SmallVector<ElementType, preSize>::operator!= (span<value_type> other) const   { return span<value_type> (*this) != other; }

template <typename ElementType, size_t preSize>
void SmallVector<ElementType, preSize>::push_back (const value_type& item)
{
    reserve (numElements + 1);
    new (elements + numElements) value_type (item);
    ++numElements;
}

template <typename ElementType, size_t preSize>
void SmallVector<ElementType, preSize>::push_back (value_type&& item)
{
    reserve (numElements + 1);
    new (elements + numElements) value_type (std::move (item));
    ++numElements;
}

template <typename ElementType, size_t preSize>
template <typename... Others>
void SmallVector<ElementType, preSize>::push_back (const value_type& first, Others&&... others)
{
    reserve (numElements + 1 + sizeof... (others));
    push_back (first);
    push_back (std::forward<Others> (others)...);
}

template <typename ElementType, size_t preSize>
template <typename... ConstructorArgs>
void SmallVector<ElementType, preSize>::emplace_back (ConstructorArgs&&... args)
{
    reserve (numElements + 1);
    new (elements + numElements) value_type (std::forward<ConstructorArgs> (args)...);
    ++numElements;
}

template <typename ElementType, size_t preSize>
void SmallVector<ElementType, preSize>::insert (iterator insertPos, const value_type& item)
{
    DISTRHO_SAFE_ASSERT_RETURN (insertPos != nullptr && insertPos >= begin() && insertPos <= end(),);
    auto index = insertPos - begin();
    push_back (item);
    std::rotate (begin() + index, end() - 1, end());
}

template <typename ElementType, size_t preSize>
void SmallVector<ElementType, preSize>::insert (iterator insertPos, value_type&& item)
{
    DISTRHO_SAFE_ASSERT_RETURN (insertPos != nullptr && insertPos >= begin() && insertPos <= end(),);
    auto index = insertPos - begin();
    push_back (std::move (item));
    std::rotate (begin() + index, end() - 1, end());
}

template <typename ElementType, size_t preSize>
void SmallVector<ElementType, preSize>::pop_back()
{
    if (numElements == 1)
    {
        clear();
    }
    else
    {
        DISTRHO_SAFE_ASSERT_RETURN (numElements > 0,);
        elements[--numElements].~value_type();
    }
}

template <typename ElementType, size_t preSize>
void SmallVector<ElementType, preSize>::clear() noexcept
{
    for (size_type i = 0; i < numElements; ++i)
        elements[i].~value_type();

    numElements = 0;
    freeHeapAndResetToInternalStorage();
}

template <typename ElementType, size_t preSize>
void SmallVector<ElementType, preSize>::resize (size_type newSize)
{
    if (newSize > numElements)
    {
        reserve (newSize);

        while (numElements < newSize)
            new (elements + numElements++) value_type (value_type());
    }
    else
    {
        shrink (newSize);
    }
}

template <typename ElementType, size_t preSize>
void SmallVector<ElementType, preSize>::shrink (size_type newSize)
{
    if (newSize == 0)
        return clear();

    DISTRHO_SAFE_ASSERT_RETURN (newSize <= numElements,);

    while (newSize < numElements && numElements > 0)
        elements[--numElements].~value_type();
}

template <typename ElementType, size_t preSize>
void SmallVector<ElementType, preSize>::reserve (size_type requiredNumElements)
{
    if (requiredNumElements > numAllocated)
    {
        requiredNumElements = static_cast<size_type> ((requiredNumElements + 15u) & ~(size_type) 15u);

        if (requiredNumElements > preSize)
        {
            auto* newBuffer = reinterpret_cast<value_type*> (new char[requiredNumElements * sizeof (value_type)]);

            for (size_type i = 0; i < numElements; ++i)
            {
                new (newBuffer + i) value_type (std::move (elements[i]));
                elements[i].~value_type();
            }

            freeHeapAndResetToInternalStorage();
            elements = newBuffer;
        }

        numAllocated = requiredNumElements;
    }
}

template <typename ElementType, size_t preSize>
void SmallVector<ElementType, preSize>::erase (iterator startElement)
{
    erase (startElement, startElement + 1);
}

template <typename ElementType, size_t preSize>
void SmallVector<ElementType, preSize>::erase (iterator startElement, iterator endElement)
{
    DISTRHO_SAFE_ASSERT_RETURN (startElement != nullptr && startElement >= begin() && startElement <= end(),);
    DISTRHO_SAFE_ASSERT_RETURN (endElement != nullptr && endElement >= begin() && endElement <= end(),);

    if (startElement != endElement)
    {
        DISTRHO_SAFE_ASSERT_RETURN (startElement < endElement,);

        if (endElement == end())
            return shrink (static_cast<size_type> (startElement - begin()));

        auto dest = startElement;

        for (auto src = endElement; src < end(); ++dest, ++src)
            *dest = std::move (*src);

        shrink (size() - static_cast<size_type> (endElement - startElement));
    }
}

}

#endif // CHOC_SMALLVECTOR_HEADER_INCLUDED
