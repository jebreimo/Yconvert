//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-07-14.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Encoder.hpp"

#include <vector>
#include "CodePageDefinitions.hpp"

namespace Yconvert
{
    struct CodePointMapRange
    {
        char32_t codepoint;
        uint8_t index;
        uint8_t length;
    };

    class CodePageEncoder : public Encoder
    {
    public:
        CodePageEncoder(Encoding encoding,
                        const CodePageRange* ranges,
                        size_t ranges_size);

        void set_replacement_character(char32_t value) override;

        [[nodiscard]]
        size_t get_encoded_size(const char32_t* src, size_t src_size) override;

        std::pair<size_t, size_t>
        encode(const char32_t* src, size_t src_size,
               void* dst, size_t dst_size) override;

        void encode(const char32_t* src, size_t src_size,
                    std::string& dst) override;

        void encode(const char32_t* src, size_t src_size, std::ostream& dst) override;

    private:
        template <typename FwdIt>
        void encode_impl(const char32_t* src, FwdIt dst, size_t count);

        std::vector<CodePointMapRange> ranges_;
    };
}
