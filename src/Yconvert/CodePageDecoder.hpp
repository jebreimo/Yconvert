//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-11-21
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Decoder.hpp"

#include "CodePageDefinitions.hpp"

namespace Yconvert
{
    class CodePageDecoder : public Decoder
    {
    public:
        CodePageDecoder(Encoding encoding,
                        const CodePageRange* ranges,
                        size_t ranges_size);
    protected:
        size_t skip_codepoint(const void* src, size_t src_size) const override;

        std::pair<size_t, size_t>
        do_decode(const void* src, size_t src_size,
                  char32_t* dst, size_t dst_size) const override;

        std::pair<size_t, size_t>
        count_valid_codepoints(const void *src, size_t src_size) const override;

    private:
        char32_t chars_[256];
    };
}
