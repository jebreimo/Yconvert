//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-06-29.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Encoder.hpp"

#include <iterator>
#include <ostream>

namespace Yconvert
{
    namespace Detail
    {
        template <bool SWAP_BYTES, typename OutIt>
        void add_bytes(char16_t c, OutIt& out)
        {
            union {char16_t c; char b[2];} u = {swap_endianness<SWAP_BYTES>(c)};
            *out++ = u.b[0];
            *out++ = u.b[1];
        }

        template <bool SWAP_BYTES, typename OutIt>
        void add_utf16_bytes(char32_t codepoint, OutIt out)
        {
            if (codepoint <= 0xFFFF)
            {
                Detail::add_bytes<SWAP_BYTES>(char16_t(codepoint), out);
            }
            else if (codepoint <= UNICODE_MAX)
            {
                codepoint -= 0x10000;
                auto word1 = char16_t(0xD800u | (codepoint >> 10u));
                auto word2 = char16_t(0xDC00u | (codepoint & 0x3FFu));
                Detail::add_bytes<SWAP_BYTES>(word1, out);
                Detail::add_bytes<SWAP_BYTES>(word2, out);
            }
        }

        template <bool SWAP_BYTES, typename T>
        size_t encode_utf16(char32_t codepoint, T* data, size_t n)
        {
            if (codepoint <= 0xFFFF)
            {
                if (n < 2)
                    return 0;
                Detail::add_bytes<SWAP_BYTES>(char16_t(codepoint), data);
                return 2;
            }
            else if (codepoint <= UNICODE_MAX)
            {
                if (n < 4)
                    return 0;
                codepoint -= 0x10000;
                auto word1 = char16_t(0xD800u | (codepoint >> 10u));
                auto word2 = char16_t(0xDC00u | (codepoint & 0x3FFu));
                Detail::add_bytes<SWAP_BYTES>(word1, data);
                Detail::add_bytes<SWAP_BYTES>(word2, data);
                return 4;
            }
            return 0;
        }
    }

    template <bool SWAP_BYTES>
    class Utf16Encoder : public Encoder
    {
    public:
        Utf16Encoder()
            : Encoder(IS_BIG_ENDIAN == SWAP_BYTES
                          ? Encoding::UTF_16_LE
                          : Encoding::UTF_16_BE)
        {}

        size_t get_encoded_size(const char32_t* src, size_t src_size) override
        {
            size_t result = 0;
            for (size_t i = 0; i < src_size; ++i)
            {
                if (src[i] <= 0xFFFF)
                    result +=  2;
                else if (src[i] <= UNICODE_MAX)
                    result += 4;
            }
            return result;
        }

        std::pair<size_t, size_t>
        encode(const char32_t* src, size_t src_size,
               void* dst, size_t dst_size) override
        {
            auto cdst = static_cast<char*>(dst);
            size_t bytes_written = 0;
            for (size_t i = 0; i < src_size; ++i)
            {
                auto n = Detail::encode_utf16<SWAP_BYTES>(
                    src[i], cdst + bytes_written, dst_size - bytes_written);
                if (n == 0 && src[i] <= UNICODE_MAX)
                    return {i, bytes_written};
                bytes_written += n;
            }
            return {src_size, bytes_written};
        }

        void encode(const char32_t* src, size_t src_size,
                    std::string& dst) override
        {
            auto out = back_inserter(dst);
            for (size_t i = 0; i < src_size; ++i)
                Detail::add_utf16_bytes<SWAP_BYTES>(src[i], out);
        }

        void encode(const char32_t* src, size_t src_size,
                    std::ostream& dst) override
        {
            auto out = std::ostreambuf_iterator(dst);
            for (size_t i = 0; i < src_size; ++i)
                Detail::add_utf16_bytes<SWAP_BYTES>(src[i], out);
        }
    };

    using Utf16BEEncoder = Utf16Encoder<IS_LITTLE_ENDIAN>;
    using Utf16LEEncoder = Utf16Encoder<IS_BIG_ENDIAN>;
}
