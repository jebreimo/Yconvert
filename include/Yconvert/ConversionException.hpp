//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-07-15.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "YconvertException.hpp"

/** @file
  * @brief Defines the exception thrown by the Converter class and the convert
  *     functions.
  */

namespace Yconvert
{
    /**
     * @brief Exception class that includes the offset of the character in
     *  the input where the conversion failed.
     */
    class ConversionException : public YconvertException
    {
    public:
        ConversionException(const std::string& message,
                            size_t codepoint_index) noexcept
            : YconvertException(message),
              codepoint_offset(codepoint_index)
        {}

        size_t codepoint_offset;
    };
}
