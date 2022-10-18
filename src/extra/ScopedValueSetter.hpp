/*
 * Scope value setter, taken from JUCE v4
 * Copyright (C) 2013 Raw Material Software Ltd.
 * Copyright (c) 2016 ROLI Ltd.
 * Copyright (C) 2013-2020 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the doc/GPL.txt file.
 */

#pragma once

#include "DistrhoUtils.hpp"

START_NAMESPACE_DISTRHO

//=====================================================================================================================
/**
    Helper class providing an RAII-based mechanism for temporarily setting and
    then re-setting a value.

    E.g. @code
    int x = 1;

    {
        ScopedValueSetter setter (x, 2);

        // x is now 2
    }

    // x is now 1 again

    {
        ScopedValueSetter setter (x, 3, 4);

        // x is now 3
    }

    // x is now 4
    @endcode
*/
template <typename ValueType>
class ScopedValueSetter
{
public:
    /** Creates a ScopedValueSetter that will immediately change the specified value to the
        given new value, and will then reset it to its original value when this object is deleted.
        Must be used only for 'noexcept' compatible types.
    */
    ScopedValueSetter(ValueType& valueToSet, ValueType newValue) noexcept
        : value(valueToSet),
          originalValue(valueToSet)
    {
        valueToSet = newValue;
    }

    /** Creates a ScopedValueSetter that will immediately change the specified value to the
        given new value, and will then reset it to be valueWhenDeleted when this object is deleted.
    */
    ScopedValueSetter(ValueType& valueToSet, ValueType newValue, ValueType valueWhenDeleted) noexcept
        : value(valueToSet),
          originalValue(valueWhenDeleted)
    {
        valueToSet = newValue;
    }

    ~ScopedValueSetter() noexcept
    {
        value = originalValue;
    }

private:
    //=================================================================================================================
    ValueType& value;
    const ValueType originalValue;

    DISTRHO_DECLARE_NON_COPYABLE(ScopedValueSetter)
    DISTRHO_PREVENT_HEAP_ALLOCATION
};

END_NAMESPACE_DISTRHO
