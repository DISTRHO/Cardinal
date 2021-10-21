/*
  ==============================================================================

   This file is part of the Water library.
   Copyright (c) 2016 ROLI Ltd.
   Copyright (C) 2017 Filipe Coelho <falktx@falktx.com>

   Permission is granted to use this software under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license/

   Permission to use, copy, modify, and/or distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
   FITNESS. IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT,
   OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
   USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
   TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
   OF THIS SOFTWARE.

  ==============================================================================
*/

#ifndef WATER_ATOMIC_HPP_INCLUDED
#define WATER_ATOMIC_HPP_INCLUDED

#include "DistrhoUtils.hpp"

START_NAMESPACE_DISTRHO

//==============================================================================
/**
    Simple class to hold a primitive value and perform atomic operations on it.

    The type used must be a 32 or 64 bit primitive, like an int, pointer, etc.
    There are methods to perform most of the basic atomic operations.
*/
template <typename Type>
class Atomic
{
public:
    /** Creates a new value, initialised to zero. */
    inline Atomic() noexcept
        : value (0)
    {
    }

    /** Creates a new value, with a given initial value. */
    inline explicit Atomic (const Type initialValue) noexcept
        : value (initialValue)
    {
    }

    /** Copies another value (atomically). */
    inline Atomic (const Atomic& other) noexcept
        : value (other.get())
    {
    }

    /** Destructor. */
    inline ~Atomic() noexcept
    {
#ifdef DISTRHO_PROPER_CPP11_SUPPORT
        // This class can only be used for types which are 32 or 64 bits in size.
        static_assert (sizeof (Type) == 4 || sizeof (Type) == 8, "Only for 32 or 64 bits");
#endif
    }

    /** Atomically reads and returns the current value. */
    Type get() const noexcept;

    /** Copies another value onto this one (atomically). */
    inline Atomic& operator= (const Atomic& other) noexcept         { exchange (other.get()); return *this; }

    /** Copies another value onto this one (atomically). */
    inline Atomic& operator= (const Type newValue) noexcept         { exchange (newValue); return *this; }

    /** Atomically sets the current value. */
    void set (Type newValue) noexcept                               { exchange (newValue); }

    /** Atomically sets the current value, returning the value that was replaced. */
    Type exchange (Type value) noexcept;

    /** Atomically adds a number to this value, returning the new value. */
    Type operator+= (Type amountToAdd) noexcept;

    /** Atomically subtracts a number from this value, returning the new value. */
    Type operator-= (Type amountToSubtract) noexcept;

    /** Atomically increments this value, returning the new value. */
    Type operator++() noexcept;

    /** Atomically decrements this value, returning the new value. */
    Type operator--() noexcept;

    /** Atomically compares this value with a target value, and if it is equal, sets
        this to be equal to a new value.

        This operation is the atomic equivalent of doing this:
        @code
        bool compareAndSetBool (Type newValue, Type valueToCompare)
        {
            if (get() == valueToCompare)
            {
                set (newValue);
                return true;
            }

            return false;
        }
        @endcode

        @returns true if the comparison was true and the value was replaced; false if
                 the comparison failed and the value was left unchanged.
        @see compareAndSetValue
    */
    bool compareAndSetBool (Type newValue, Type valueToCompare) noexcept;

    /** Atomically compares this value with a target value, and if it is equal, sets
        this to be equal to a new value.

        This operation is the atomic equivalent of doing this:
        @code
        Type compareAndSetValue (Type newValue, Type valueToCompare)
        {
            Type oldValue = get();
            if (oldValue == valueToCompare)
                set (newValue);

            return oldValue;
        }
        @endcode

        @returns the old value before it was changed.
        @see compareAndSetBool
    */
    Type compareAndSetValue (Type newValue, Type valueToCompare) noexcept;

    /** Implements a memory read/write barrier. */
    static void memoryBarrier() noexcept;

    //==============================================================================
    /** The raw value that this class operates on.
        This is exposed publicly in case you need to manipulate it directly
        for performance reasons.
    */
   #if defined(__LP64__) || defined(_LP64) || defined(__arm64__) || defined(__aarch64__) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(_M_ARM64)
    __attribute__ ((aligned (8)))
   #else
    __attribute__ ((aligned (4)))
   #endif
    volatile Type value;

private:
    template <typename Dest, typename Source>
    static inline Dest castTo (Source value) noexcept         { union { Dest d; Source s; } u; u.s = value; return u.d; }

    static inline Type castFrom32Bit (int32_t value)  noexcept  { return castTo <Type, int32_t>  (value); }
    static inline Type castFrom64Bit (int64_t value)  noexcept  { return castTo <Type, int64_t>  (value); }
    static inline Type castFrom32Bit (uint32_t value) noexcept  { return castTo <Type, uint32_t> (value); }
    static inline Type castFrom64Bit (uint64_t value) noexcept  { return castTo <Type, uint64_t> (value); }
    static inline int32_t castTo32Bit (Type value) noexcept     { return castTo <int32_t, Type>  (value); }
    static inline int64_t castTo64Bit (Type value) noexcept     { return castTo <int64_t, Type>  (value); }

    Type operator++ (int); // better to just use pre-increment with atomics..
    Type operator-- (int);

    /** This templated negate function will negate pointers as well as integers */
    template <typename ValueType>
    inline ValueType negateValue (ValueType n) noexcept
    {
        return sizeof (ValueType) == 1 ? (ValueType) -(signed char) n
            : (sizeof (ValueType) == 2 ? (ValueType) -(short) n
            : (sizeof (ValueType) == 4 ? (ValueType) -(int) n
            : ((ValueType) -(int64_t) n)));
    }

    /** This templated negate function will negate pointers as well as integers */
    template <typename PointerType>
    inline PointerType* negateValue (PointerType* n) noexcept
    {
        return reinterpret_cast<PointerType*> (-reinterpret_cast<intptr_t> (n));
    }
};

//==============================================================================
template<>
inline int32_t Atomic<int32_t>::get() const noexcept
{
#ifdef DISTRHO_PROPER_CPP11_SUPPORT
    static_assert (sizeof (int32_t) == 4, "int32_t must be size 4");
#endif
    return castFrom32Bit ((int32_t) __sync_add_and_fetch (const_cast<volatile int32_t*>(&value), 0));
}

template<>
inline int64_t Atomic<int64_t>::get() const noexcept
{
#ifdef DISTRHO_PROPER_CPP11_SUPPORT
    static_assert (sizeof (int64_t) == 8, "int64_t must be size 8");
#endif
    return castFrom64Bit ((int64_t) __sync_add_and_fetch (const_cast<volatile int64_t*>(&value), 0));
}

template<>
inline uint32_t Atomic<uint32_t>::get() const noexcept
{
#ifdef DISTRHO_PROPER_CPP11_SUPPORT
    static_assert (sizeof (uint32_t) == 4, "uint32_t must be size 4");
#endif
    return castFrom32Bit ((uint32_t) __sync_add_and_fetch (const_cast<volatile uint32_t*>(&value), 0));
}

template<>
inline uint64_t Atomic<uint64_t>::get() const noexcept
{
#ifdef DISTRHO_PROPER_CPP11_SUPPORT
    static_assert (sizeof (uint64_t) == 8, "uint64_t must be size 8");
#endif
    return castFrom64Bit ((uint64_t) __sync_add_and_fetch (const_cast<volatile uint64_t*>(&value), 0));
}

template <typename Type>
inline Type Atomic<Type>::exchange (const Type newValue) noexcept
{
    Type currentVal = value;
    while (! compareAndSetBool (newValue, currentVal)) { currentVal = value; }
    return currentVal;
}

template <typename Type>
inline Type Atomic<Type>::operator+= (const Type amountToAdd) noexcept
{
    return (Type) __sync_add_and_fetch (&value, amountToAdd);
}

template <typename Type>
inline Type Atomic<Type>::operator-= (const Type amountToSubtract) noexcept
{
    return operator+= (negateValue (amountToSubtract));
}

template <typename Type>
inline Type Atomic<Type>::operator++() noexcept
{
    return sizeof (Type) == 4 ? (Type) __sync_add_and_fetch (&value, (Type) 1)
                              : (Type) __sync_add_and_fetch ((volatile int64_t*) &value, 1);
}

template <typename Type>
inline Type Atomic<Type>::operator--() noexcept
{
    return sizeof (Type) == 4 ? (Type) __sync_add_and_fetch (&value, (Type) -1)
                              : (Type) __sync_add_and_fetch ((volatile int64_t*) &value, -1);
}

template <typename Type>
inline bool Atomic<Type>::compareAndSetBool (const Type newValue, const Type valueToCompare) noexcept
{
    return sizeof (Type) == 4 ? __sync_bool_compare_and_swap ((volatile int32_t*) &value, castTo32Bit (valueToCompare), castTo32Bit (newValue))
                              : __sync_bool_compare_and_swap ((volatile int64_t*) &value, castTo64Bit (valueToCompare), castTo64Bit (newValue));
}

template <typename Type>
inline Type Atomic<Type>::compareAndSetValue (const Type newValue, const Type valueToCompare) noexcept
{
    return sizeof (Type) == 4 ? castFrom32Bit ((int32_t) __sync_val_compare_and_swap ((volatile int32_t*) &value, castTo32Bit (valueToCompare), castTo32Bit (newValue)))
                              : castFrom64Bit ((int64_t) __sync_val_compare_and_swap ((volatile int64_t*) &value, castTo64Bit (valueToCompare), castTo64Bit (newValue)));
}

template <typename Type>
inline void Atomic<Type>::memoryBarrier() noexcept
{
    __sync_synchronize();
}

END_NAMESPACE_DISTRHO

#endif // WATER_ATOMIC_HPP_INCLUDED
