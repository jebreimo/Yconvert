//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2016-04-23.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <istream>
#include <string>
#include <string_view>
#include "Converter.hpp"
#include "Encoding.hpp"
#include "Yconvert/Details/InputStreamWrapper.hpp"

/** @file
  * @brief Defines functions that convert strings from one encoding
  *     to another.
  */

namespace Yconvert
{
    /**
     * @brief Converts the string @a source with @a converter
     *  and writes the result to @a destination.
     *
     * @returns The number of bytes read from source and the number of bytes
     *  written to destination.
     */
    YCONVERT_API std::pair<size_t, size_t>
    convert(const void* source, size_t source_size,
            void* destination, size_t destination_size,
            Converter& converter);

    /**
     * @brief Converts the string @a source from @a source_encoding
     *  to @a destination_encoding and writes the result to @a destination.
     *
     * @returns The number of bytes read from source and the number of bytes
     *  written to destination.
     */
    YCONVERT_API std::pair<size_t, size_t>
    convert(const void* source, size_t source_size,
            Encoding source_encoding,
            void* destination, size_t destination_size,
            Encoding destination_encoding,
            ErrorPolicy error_policy = ErrorPolicy::REPLACE);

    /**
     * @brief Converts the string @a source with @a converter and writes
     *  the result to @a destination.
     *
     * @returns The number of values read from source and the number of
     *  bytes written to destination.
     */
    template <typename CharT>
    std::pair<size_t, size_t>
    convert(std::basic_string_view<CharT> source,
            void* destination, size_t destination_size,
            Converter& converter)
    {
        using SrcString = std::basic_string_view<CharT>;
        auto src_size = source.size() * sizeof(typename SrcString::value_type);
        auto n = converter.convert(source.data(), src_size,
                                   destination, destination_size);
        return {n.first / sizeof(typename SrcString::value_type),
                n.second};
    }

    /**
     * @brief Converts the string @a source from @a source_encoding
     *  to @a destination_encoding and writes the result to @a destination.
     *
     * @returns The number of values read from source and the number of
     *  bytes written to destination.
     */
    template <typename CharT>
    std::pair<size_t, size_t>
    convert(std::basic_string_view<CharT> source,
            Encoding source_encoding,
            void* destination, size_t destination_size,
            Encoding destination_encoding,
            ErrorPolicy error_policy = ErrorPolicy::REPLACE)
    {
        Converter converter(source_encoding, destination_encoding);
        converter.set_error_policy(error_policy);
        return convert(source, destination, destination_size, converter);
    }

    /**
     * @brief Converts the string @a source with @a converter and writes
     *  the result to @a destination.
     */
    template <typename CharT>
    void convert(std::basic_string_view<CharT> source,
                 std::string& destination,
                 Converter& converter)
    {
        using SrcString = std::basic_string_view<CharT>;
        auto src_size = source.size() * sizeof(typename SrcString::value_type);
        converter.convert(source.data(), src_size, destination);
    }

    /**
     * @brief Converts the string @a source from @a source_encoding
     *  to @a destination_encoding and writes the result to @a destination.
     */
    template <typename Char1T, typename Char2T>
    void convert(std::basic_string_view<Char1T> source,
                 Encoding source_encoding,
                 std::basic_string<Char2T>& destination,
                 Encoding destination_encoding,
                 ErrorPolicy error_policy = ErrorPolicy::REPLACE)
    {
        Converter converter(source_encoding, destination_encoding);
        converter.set_error_policy(error_policy);
        convert(source, destination, converter);
    }

    /**
     * @brief Converts the string @a source with @a converter and writes
     *  the result to @a destination.
     */
    template <typename Char1T, typename Char2T>
    void convert(std::basic_string_view<Char1T> source,
                 std::basic_string<Char2T>& destination,
                 Converter& converter)
    {
        auto src_size = source.size() * sizeof(Char1T);
        auto dst_size = converter.get_encoded_size(source.data(), src_size);
        destination.resize(dst_size / sizeof(Char2T));
        converter.convert(source.data(), src_size,
                          destination.data(), dst_size);
    }

    YCONVERT_API void convert(const void* source, size_t source_size,
                              std::ostream& destination,
                              Converter& converter);

    template <typename CharT>
    void convert(std::basic_string_view<CharT> source,
                 std::ostream& destination,
                 Converter& converter)
    {
        convert(source.data(), source.size() * sizeof(CharT),
                       destination, converter);
    }

    template <typename CharT>
    void convert(std::basic_string_view<CharT> source,
                 Encoding source_encoding,
                 std::ostream& destination,
                 Encoding destination_encoding,
                 ErrorPolicy error_policy = ErrorPolicy::REPLACE)
    {
        Converter converter(source_encoding, destination_encoding);
        converter.set_error_policy(error_policy);
        convert(source, destination, converter);
    }

    template <typename CharT>
    void convert(std::istream& source,
                 std::basic_string<CharT>& destination,
                 Converter& converter)
    {
        Details::InputStreamWrapper input(source);
        while (input.fill())
        {
            auto result_offset = destination.size();
            auto raw_size = converter.get_encoded_size(input.data(), input.size());
            auto enc_size = raw_size / sizeof(CharT);
            destination.resize(result_offset + enc_size);
            auto sizes = converter.convert(input.data(), input.size(),
                                           destination.data() + result_offset,
                                           raw_size, input.eof());
            input.drain(sizes.first);
            destination.resize(result_offset + sizes.second / sizeof(CharT));
        }
    }

    /**
     * @brief Converts the stream @a source with @a converter and writes
     * the result to @a destination.
     */
    YCONVERT_API void convert(std::istream& source,
                              std::string& destination,
                              Converter& converter);

    /**
     * @brief Converts the stream @a source with @a converter and writes
     *  the result to @a destination.
     */
    YCONVERT_API void convert(std::istream& source,
                              std::ostream& destination,
                              Converter& converter);

    /**
     * @brief Converts the stream @a source from @a source_encoding
     *  to @a destination_encoding and writes the result to @a destination.
     */
    YCONVERT_API void convert(std::istream& source,
                              Encoding source_encoding,
                              std::ostream& destination,
                              Encoding destination_encoding,
                              ErrorPolicy error_policy = ErrorPolicy::REPLACE);

    /**
     * @brief Converts the string @a source with @a converter
     *  and returns the result.
     *
     * The first template parameter is the result's string type and must be
     * specified explicitly.
     */
    template <typename StringT, typename CharT>
    StringT convert_to(std::basic_string_view<CharT> source,
                       Converter& converter)
    {
        StringT result;
        convert<CharT, typename StringT::value_type>(source, result,
                                                            converter);
        return result;
    }

    /**
     * @brief Converts the string @a source with @a converter
     *  and returns the result.
     *
     * The first template parameter is the result's string type and must be
     * specified explicitly.
     */
    template <typename StringT, typename CharT>
    StringT convert_to(const std::basic_string<CharT>& source,
                       Converter& converter)
    {
        return convert_to<StringT>(std::basic_string_view<CharT>(source),
                                   converter);
    }

    /**
     * @brief Converts the string @a source from @a source_encoding
     *  to @a destination_encoding and returns the result.
     *
     * The first template parameter is the result's string type and must be
     * specified explicitly.
     */
    template <typename StringT, typename CharT>
    StringT convert_to(std::basic_string_view<CharT> source,
                       Encoding source_encoding,
                       Encoding result_encoding,
                       ErrorPolicy error_policy = ErrorPolicy::REPLACE)
    {
        StringT result;
        convert<CharT, typename StringT::value_type>(
            source, source_encoding, result, result_encoding, error_policy);
        return result;
    }

    /**
     * @brief Converts the string @a source from @a source_encoding
     *  to @a destination_encoding and returns the result.
     *
     * The first template parameter is the result's string type and must be
     * specified explicitly.
     */
    template <typename StringT, typename CharT>
    StringT convert_to(const std::basic_string<CharT>& source,
                       Encoding source_encoding,
                       Encoding result_encoding,
                       ErrorPolicy error_policy = ErrorPolicy::REPLACE)
    {
        return convert_to<StringT>(std::basic_string_view<CharT>(source),
                                   source_encoding,
                                   result_encoding,
                                   error_policy);
    }

    /**
     * @brief Reads the contents of @a source, converts it with @a converter
     *  and returns the result.
     */
    template <typename StringT>
    StringT convert_to(std::istream& source,
                       Converter& converter)
    {
        StringT result;
        convert(source, result, converter);
        return result;
    }

    /**
     * @brief Reads the contents of @a stream, converts it from
     *  @a source_encoding to @a destination_encoding and returns the result.
     */
    template <typename StringT>
    StringT convert_to(std::istream& stream,
                       Encoding source_encoding,
                       Encoding result_encoding,
                       ErrorPolicy error_policy = ErrorPolicy::REPLACE)
    {
        Converter converter(source_encoding, result_encoding);
        converter.set_error_policy(error_policy);
        return convert_to<StringT>(stream, converter);
    }
}
