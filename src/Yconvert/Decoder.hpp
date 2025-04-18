//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-10-26
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstdint>
#include <string>
#include "Yconvert/Encoding.hpp"
#include "Yconvert/ErrorPolicy.hpp"

namespace Yconvert
{
    class Decoder
    {
    public:
        virtual ~Decoder() = default;

        [[nodiscard]]
        Encoding encoding() const;

        [[nodiscard]]
        ErrorPolicy error_handling_policy() const;

        void set_error_policy(ErrorPolicy policy);

        /**
         * @brief Decodes a sequence of bytes.
         *
         * @param src The input sequence.
         * @param src_size The number of bytes in the input sequence.
         * @param dst The output buffer.
         * @param dst_size The size of the output buffer.
         * @param src_is_final True if the input sequence is the last part.
         * @return The number of bytes read from the input sequence and the
         *     number of characters written to the output buffer.
         */
        std::pair<size_t, size_t>
        decode(const void* src, size_t src_size,
               char32_t* dst, size_t dst_size,
               bool src_is_final = true) const;

        virtual std::pair<size_t, size_t>
        count_valid_codepoints(const void* src, size_t src_size) const = 0;
    protected:
        explicit Decoder(Encoding encoding);

        virtual size_t
        skip_codepoint(const void* src, size_t src_size) const = 0;

        virtual std::pair<size_t, size_t>
        do_decode(const void* src, size_t src_size,
                  char32_t* dst, size_t dst_size) const = 0;
    private:
        Encoding encoding_;
        ErrorPolicy error_policy_ = ErrorPolicy::THROW;
    };
}
