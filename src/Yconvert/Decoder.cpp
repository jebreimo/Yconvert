//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-10-26
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Decoder.hpp"

#include "Yconvert/ConversionException.hpp"

namespace Yconvert
{
    Decoder::Decoder(Encoding encoding)
        : encoding_(encoding),
          error_policy_()
    {}

    Encoding Decoder::encoding() const
    {
        return encoding_;
    }

    ErrorPolicy Decoder::error_handling_policy() const
    {
        return error_policy_;
    }

    void Decoder::set_error_policy(ErrorPolicy policy)
    {
        error_policy_ = policy;
    }

    std::pair<size_t, size_t>
    Decoder::decode(const void* src, size_t src_size,
                    char32_t* dst, size_t dst_size,
                    bool src_is_final) const
    {
        size_t i_src = 0, i_dst = 0;
        auto bytes = static_cast<const char*>(src);
        while (true)
        {
            auto size = do_decode(bytes + i_src, src_size - i_src,
                                  dst + i_dst, dst_size - i_dst);
            i_src += size.first;
            i_dst += size.second;
            if (i_src == src_size || i_dst == dst_size)
                return {i_src, i_dst};

            if (!src_is_final)
            {
                auto next = skip_codepoint(bytes + i_src, src_size - i_src);
                if (i_src + next == src_size)
                    return {i_src, i_dst};
            }

            switch (error_policy_)
            {
            case ErrorPolicy::REPLACE:
                dst[i_dst++] = REPLACEMENT_CHARACTER;
                i_src += skip_codepoint(bytes + i_src, src_size - i_src);
                break;
            case ErrorPolicy::THROW:
                throw ConversionException("Invalid character in input.", i_dst);
            case ErrorPolicy::SKIP:
            case ErrorPolicy::IGNORE:
                i_src += skip_codepoint(bytes, src_size);
                break;
            }
        }
    }
}
