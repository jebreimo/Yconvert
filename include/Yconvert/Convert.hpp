//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2016-04-23.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Converter.hpp"
#include "Encoding.hpp"
#include <string_view>
#include <string>

/** @file
  * @brief Defines functions that convert strings from one encoding
  *     to another.
  */

namespace Yconvert
{
    /**
     * @brief Converts the string @a source from @a sourceEncoding
     *  to @a destinationEncoding and writes the result to @a destination.
     *
     * @returns The number of bytes read from source and the number of bytes
     *  written to destination.
     */
    std::pair<size_t, size_t>
    convert(const void* source, size_t sourceSize,
            Encoding sourceEncoding,
            void* destination, size_t destinationSize,
            Encoding destinationEncoding,
            ErrorPolicy errorPolicy);

    /**
     * @brief Converts the string @a source from @a sourceEncoding
     *  to @a destinationEncoding and writes the result to @a destination.
     *
     * @returns The number of values read from source and the number of
     *  bytes written to destination.
     */
    template <typename Char1T>
    std::pair<size_t, size_t>
    convert(const std::basic_string_view<Char1T>& source,
            Encoding sourceEncoding,
            void* destination, size_t destinationSize,
            Encoding destinationEncoding,
            ErrorPolicy errorPolicy)
    {
        Converter converter(sourceEncoding, destinationEncoding);
        converter.setErrorHandlingPolicy(errorPolicy);
        using SrcString = std::basic_string_view<Char1T>;
        auto srcSize = source.size() * sizeof(typename SrcString::value_type);
        auto n = converter.convert(source.data(), srcSize,
                                   destination, destinationSize);
        return {n.first / sizeof(typename SrcString::value_type),
                n.second};
    }

    /**
     * @brief Converts the string @a source from @a sourceEncoding
     *  to @a destinationEncoding and writes the result to @a destination.
     */
    template <typename Char1T>
    void convert(const std::basic_string_view<Char1T>& source,
                 Encoding sourceEncoding,
                 std::string& destination,
                 Encoding destinationEncoding,
                 ErrorPolicy errorPolicy = ErrorPolicy::REPLACE)
    {
        Converter converter(sourceEncoding, destinationEncoding);
        converter.setErrorHandlingPolicy(errorPolicy);
        using SrcString = std::basic_string_view<Char1T>;
        auto srcSize = source.size() * sizeof(typename SrcString::value_type);
        converter.convert(source.data(), srcSize, destination);
    }

    /**
     * @brief Converts the string @a source from @a sourceEncoding
     *  to @a destinationEncoding and writes the result to @a destination.
     */
    template <typename Char1T, typename Char2T>
    void convert(const std::basic_string_view<Char1T>& source,
                 Encoding sourceEncoding,
                 std::basic_string<Char2T>& destination,
                 Encoding destinationEncoding,
                 ErrorPolicy errorPolicy = ErrorPolicy::REPLACE)
    {
        Converter converter(sourceEncoding, destinationEncoding);
        converter.setErrorHandlingPolicy(errorPolicy);
        using SrcString = std::basic_string_view<Char1T>;
        auto srcSize = source.size() * sizeof(typename SrcString::value_type);
        auto dstSize = converter.getEncodedSize(source.data(), srcSize);
        destination.resize(dstSize);
        converter.convert(source.data(), srcSize,
                          destination.data(), destination.size());
    }

    /**
     * @brief Converts the string @a source from @a sourceEncoding
     *  to @a destinationEncoding and returns the result to @a destination.
     *
     * The first template parameter is the result's string type and must be
     * specified explicitly.
     */
    template <typename StringT, typename CharT>
    StringT convert(const std::basic_string_view<CharT>& source,
                    Encoding sourceEncoding,
                    Encoding resultEncoding,
                    ErrorPolicy errorPolicy = ErrorPolicy::REPLACE)
    {
        StringT result;
        convert(source, sourceEncoding, result, resultEncoding, errorPolicy);
        return result;
    }
}
