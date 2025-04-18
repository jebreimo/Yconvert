//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-11-10
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

/** @file
  * @brief Defines the error handling policies available to the Conversion
  *     class.
  */

namespace Yconvert
{
    /** @brief The available error handling policies for encoding conversion.
      */
    enum class ErrorPolicy
    {
        /**
         * @brief Replace invalid characters with a replacement character.
         */
        REPLACE,
        /**
         * @brief Throw an exception if an invalid character is encountered.
         */
        THROW,
        /**
         * @brief Skip invalid characters as if they don't exist.
         */
        SKIP,
        /**
         * @brief Assume there are no invalid characters.
         * This only works if the source and destination encodings are the same,
         * or sufficiently similar (i.e. big endian and little endian versions
         * of the same encoding). `SKIP` is used ff they are not similar enough.
         */
        IGNORE
    };
}
