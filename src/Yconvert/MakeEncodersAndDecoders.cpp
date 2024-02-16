//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-02-16.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "MakeEncodersAndDecoders.hpp"

#include "CodePageDecoder.hpp"
#include "CodePageEncoder.hpp"
#include "Utf8Decoder.hpp"
#include "Utf8Encoder.hpp"
#include "Utf16Decoder.hpp"
#include "Utf16Encoder.hpp"
#include "Utf32Decoder.hpp"
#include "Utf32Encoder.hpp"
#include "YconvertThrow.hpp"

namespace Yconvert
{
    namespace
    {
    #ifdef YCONVERT_ENABLE_CODE_PAGES

        inline std::pair<const CodePageRange*, size_t>
        get_code_page_ranges(Encoding encoding)
        {
            static const CodePageRange ASCII_CHARS = {0x0000, 0, 127};
            if (encoding == Encoding::ASCII)
                return {&ASCII_CHARS, 1};
            #ifdef YCONVERT_ENABLE_ISO_CODE_PAGES
            if ((unsigned(encoding) & unsigned(Encoding::ISO_8859_1)) != 0)
                return get_iso_code_page_ranges(encoding);
            #endif
            #ifdef YCONVERT_ENABLE_MAC_CODE_PAGES
            if ((unsigned(encoding) & unsigned(Encoding::MAC_CYRILLIC)) != 0)
                    return get_iso_code_page_ranges(encoding);
            #endif
            #ifdef YCONVERT_ENABLE_DOS_CODE_PAGES
            if ((unsigned(encoding) & unsigned(Encoding::DOS_CP437)) != 0)
                return get_iso_code_page_ranges(encoding);
            #endif
            #ifdef YCONVERT_ENABLE_WIN_CODE_PAGES
            if ((unsigned(encoding) & unsigned(Encoding::WIN_CP1250)) != 0)
                return get_iso_code_page_ranges(encoding);
            #endif
            return {nullptr, 0};
        }

        std::unique_ptr<DecoderBase> make_code_page_decoder(Encoding encoding)
        {
            auto [ranges, ranges_size] = get_code_page_ranges(encoding);
            if (ranges)
            {
                return std::unique_ptr<DecoderBase>(new CodePageDecoder(
                    encoding, ranges, ranges_size));
            }
            return {};
        }

        std::unique_ptr<EncoderBase> make_code_page_encoder(Encoding encoding)
        {
            auto [ranges, ranges_size] = get_code_page_ranges(encoding);
            if (ranges)
            {
                return std::unique_ptr<EncoderBase>(new CodePageEncoder(
                    encoding, ranges, ranges_size));
            }
            return {};
        }

    #else

        std::unique_ptr<DecoderBase> make_code_page_decoder(Encoding)
        {
            return {};
        }

        std::unique_ptr<EncoderBase> make_code_page_encoder(Encoding)
        {
            return {};
        }

    #endif
    }

    std::unique_ptr<DecoderBase> make_decoder(Encoding encoding)
    {
        switch (encoding)
        {
        case Encoding::UTF_8:
            return std::unique_ptr<DecoderBase>(new Utf8Decoder);
        case Encoding::UTF_16_BE:
            return std::unique_ptr<DecoderBase>(new Utf16BEDecoder);
        case Encoding::UTF_16_LE:
            return std::unique_ptr<DecoderBase>(new Utf16LEDecoder);
        case Encoding::UTF_32_BE:
            return std::unique_ptr<DecoderBase>(new Utf32BEDecoder);
        case Encoding::UTF_32_LE:
            return std::unique_ptr<DecoderBase>(new Utf32LEDecoder);
        default:
            if (auto decoder = make_code_page_decoder(encoding))
                return decoder;
            break;
        }

        const auto& info = get_encoding_info(encoding);
        YCONVERT_THROW("Unsupported decoder: " + std::string(info.name));
    }

    std::unique_ptr<EncoderBase> make_encoder(Encoding encoding)
    {
        switch (encoding)
        {
        case Encoding::UTF_8:
            return std::unique_ptr<EncoderBase>(new Utf8Encoder);
        case Encoding::UTF_16_BE:
            return std::unique_ptr<EncoderBase>(new Utf16BEEncoder);
        case Encoding::UTF_16_LE:
            return std::unique_ptr<EncoderBase>(new Utf16LEEncoder);
        case Encoding::UTF_32_BE:
            return std::unique_ptr<EncoderBase>(new Utf32BEEncoder);
        case Encoding::UTF_32_LE:
            return std::unique_ptr<EncoderBase>(new Utf32LEEncoder);
        default:
            if (auto encoder = make_code_page_encoder(encoding))
                return encoder;
            break;
        }

        const auto& info = get_encoding_info(encoding);
        YCONVERT_THROW("Unsupported encoder: " + std::string(info.name));
    }
}
