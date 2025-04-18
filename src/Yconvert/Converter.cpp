//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-10-23.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Converter.hpp"

#include <cassert>
#include <cstring>
#include <ostream>
#include <vector>
#include "Yconvert/ConversionException.hpp"
#include "MakeEncodersAndDecoders.hpp"

namespace Yconvert
{
    namespace
    {
        size_t find_nth_code_point(Decoder& decoder,
                                   const char* src, size_t src_size,
                                   std::vector<char32_t>& buf, size_t n)
        {
            assert(buf.size() >= n);
            if (n == 0)
                return 0;
            auto& info = get_info(decoder.encoding());
            if (info.max_units == 1)
                return n * info.unit_size;
            return decoder.decode(src, src_size, buf.data(), n).first;
        }

        template <unsigned UNIT_SIZE>
        size_t endian_copy(const void* src, size_t srcSize,
                           void* dst, size_t dstSize)
        {
            static_assert((UNIT_SIZE & ~UNIT_SIZE) == 0);
            size_t size = std::min(srcSize, dstSize);
            size -= size % UNIT_SIZE;
            auto c_src = static_cast<const char*>(src);
            auto cdst = static_cast<char*>(dst);
            if constexpr (UNIT_SIZE == 1)
            {
                memcpy(dst, src, size);
            }
            else
            {
                for (size_t i = 0; i < size; i += UNIT_SIZE)
                {
                    for (size_t j = 0; j < UNIT_SIZE; ++j)
                        cdst[i + j] = c_src[i + UNIT_SIZE - 1 - j];
                }
            }
            return size;
        }
    }

    Converter::Converter(Encoding src_encoding, Encoding dst_encoding)
        : decoder_(make_decoder(src_encoding)),
          encoder_(make_encoder(dst_encoding)),
          conversion_type_(get_conversion_type(src_encoding, dst_encoding))
    {}

    Converter::Converter(Converter&&) noexcept = default;

    Converter::~Converter() = default;

    Converter& Converter::operator=(Converter&&) noexcept = default;

    size_t Converter::buffer_size() const
    {
        return buffer_.empty() ? BUFFER_SIZE : buffer_.size();
    }

    void Converter::set_buffer_size(size_t size)
    {
        buffer_.resize(size);
    }

    void Converter::set_error_policy(ErrorPolicy policy)
    {
        decoder_->set_error_policy(policy);
        encoder_->set_error_policy(policy);
    }

    ErrorPolicy Converter::error_policy() const
    {
        return decoder_->error_handling_policy();
    }

    char32_t Converter::replacement_character() const
    {
        return encoder_->replacement_character();
    }

    void Converter::set_replacement_character(char32_t value)
    {
        encoder_->set_replacement_character(value);
    }

    Encoding Converter::source_encoding() const
    {
        return decoder_->encoding();
    }

    Encoding Converter::destination_encoding() const
    {
        return encoder_->encoding();
    }

    size_t Converter::get_encoded_size(const void* src, size_t src_size)
    {
        if (conversion_type_ != ConversionType::CONVERT)
            return src_size;

        if (buffer_.empty())
            buffer_.resize(BUFFER_SIZE);

        const auto& dec = get_info(decoder_->encoding());
        const auto& enc = get_info(encoder_->encoding());
        if (dec.max_units == 1 && enc.max_units == 1)
            return (src_size / dec.unit_size) * enc.unit_size;
        auto bytes = static_cast<const char*>(src);
        size_t size = 0;
        size_t offset = 0;
        for (;;)
        {
            auto [n, m] = decoder_->decode(bytes + offset, src_size - offset,
                                           buffer_.data(), buffer_.size());
            if (n == 0)
                break;
            offset += n;
            size += encoder_->get_encoded_size(buffer_.data(), m);
        }
        return size;
    }

    size_t Converter::convert(const void* src, size_t src_size,
                              std::string& dst,
                              bool src_is_final)
    {
        auto conversion_type = error_policy() == ErrorPolicy::IGNORE
            ? conversion_type_
            : ConversionType::CONVERT;
        switch (conversion_type)
        {
        case ConversionType::SWAP_ENDIANNESS:
            {
                auto old_size = dst.size();
                dst.resize(old_size + src_size);
                return copy_and_swap(src, src_size,
                                     dst.data() + old_size, src_size);
            }
        case ConversionType::COPY:
            {
                auto old_size = dst.size();
                dst.resize(old_size + src_size);
                return copy(src, src_size, dst.data() + old_size, src_size);
            }
        case ConversionType::CONVERT:
            return do_convert(src, src_size, dst, src_is_final);
        default:
            return 0;
        }
    }

    std::pair<size_t, size_t>
    Converter::convert(const void* src, size_t src_size,
                       void* dst, size_t dst_size,
                       bool src_is_final)
    {
        auto conversion_type = error_policy() == ErrorPolicy::IGNORE
                               ? conversion_type_
                               : ConversionType::CONVERT;
        switch (conversion_type)
        {
        case ConversionType::SWAP_ENDIANNESS:
        {
            auto n = copy_and_swap(src, src_size, dst, dst_size);
            return {n, n};
        }
        case ConversionType::COPY:
        {
            auto n = copy(src, src_size, dst, dst_size);
            return {n, n};
        }
        case ConversionType::CONVERT:
            return do_convert(src, src_size, dst, dst_size, src_is_final);
        default:
            return {0, 0};
        }
    }

    size_t Converter::convert(const void* src, size_t src_size,
                              std::ostream& dst,
                              bool src_is_final)
    {
        auto conversion_type = error_policy() == ErrorPolicy::IGNORE
                               ? conversion_type_
                               : ConversionType::CONVERT;
        switch (conversion_type)
        {
        case ConversionType::SWAP_ENDIANNESS:
        {
            auto n = copy_and_swap(src, src_size, dst);
            return n;
        }
        case ConversionType::COPY:
        {
            auto unit_size = get_info(decoder_->encoding()).unit_size;
            auto count = std::streamsize(src_size - src_size % unit_size);
            dst.write(static_cast<const char*>(src), count);
            return count;
        }
        case ConversionType::CONVERT:
            return do_convert(src, src_size, dst, src_is_final);
        default:
            return 0;
        }
    }

    Converter::ConversionType Converter::get_conversion_type(
            Encoding src, Encoding dst)
    {
        if (src == dst)
            return ConversionType::COPY;
        if ((src == Encoding::UTF_16_LE && dst == Encoding::UTF_16_BE)
            || (src == Encoding::UTF_16_BE && dst == Encoding::UTF_16_LE)
            || (src == Encoding::UTF_32_LE && dst == Encoding::UTF_32_BE)
            || (src == Encoding::UTF_32_BE && dst == Encoding::UTF_32_LE))
        {
            return ConversionType::SWAP_ENDIANNESS;
        }
        return ConversionType::CONVERT;
    }

    size_t Converter::copy(const void* src, size_t src_size,
                           void* dst, size_t dst_size)
    {
        auto unit_size = get_info(encoder_->encoding()).unit_size;
        auto min_size = std::min(src_size, dst_size);
        min_size -= min_size % unit_size;
        memcpy(dst, src, min_size);
        return min_size;
    }

    size_t Converter::copy_and_swap(const void* src, size_t src_size,
                                    void* dst, size_t dst_size)
    {
        auto unit_size = get_info(decoder_->encoding()).unit_size;
        if (unit_size == 2)
            return endian_copy<2>(src, src_size, dst, dst_size);
        else if (unit_size == 4)
            return endian_copy<4>(src, src_size, dst, dst_size);
        return 0;
    }

    size_t Converter::copy_and_swap(const void* src, size_t src_size,
                                    std::ostream& dst)
    {
        if (buffer_.empty())
            buffer_.resize(BUFFER_SIZE);

        auto buf = reinterpret_cast<char*>(buffer_.data());
        auto buf_size = buffer_.size() * sizeof(char32_t);

        for (size_t i = 0; i < src_size; i += buf_size)
        {
            auto n = std::min(src_size - i, buf_size);
            auto m = copy_and_swap(static_cast<const char*>(src) + i, n,
                                   buf, buf_size);
            dst.write(buf, std::streamsize(m));
        }
        return src_size;
    }

    size_t Converter::do_convert(const void* src, size_t src_size,
                                 std::string& dst, bool src_is_final)
    {
        if (buffer_.empty())
            buffer_.resize(BUFFER_SIZE);

        auto original_size = src_size;
        auto c_src = static_cast<const char*>(src);
        while (src_size != 0)
        {
            auto [dec_in, dec_out] = decoder_->decode(c_src, src_size,
                                                      buffer_.data(), buffer_.size(),
                                                      src_is_final);
            if (dec_in == 0)
                break;
            encoder_->encode(buffer_.data(), dec_out, dst);
            c_src += dec_in;
            src_size -= dec_in;
        }
        return original_size - src_size;
    }

    std::pair<size_t, size_t>
    Converter::do_convert(const void* src, size_t src_size,
                          void* dst, size_t dst_size,
                          bool src_is_final)
    {
        if (buffer_.empty())
            buffer_.resize(BUFFER_SIZE);

        auto src_size_0 = src_size;
        auto dst_size_0 = dst_size;
        auto c_src = static_cast<const char*>(src);
        auto cdst = static_cast<char*>(dst);
        size_t code_point_offset = 0;
        try
        {
            while (src_size != 0)
            {
                auto[dec_in, dec_out] = decoder_->decode(c_src, src_size,
                                                         buffer_.data(), buffer_.size(),
                                                         src_is_final);
                if (dec_in == 0)
                    break;
                auto [enc_in, enc_out] = encoder_->encode(buffer_.data(), dec_out,
                                                          cdst, dst_size);
                if (dec_out != enc_in)
                {
                    // We've reached the end of the output buffer, but have
                    // not encoded all codepoints in the input buffer. We need
                    // to rewind the input buffer to the end of the last code
                    // point that was encoded.
                    src_size -= find_nth_code_point(*decoder_, c_src, src_size,
                                                    buffer_, enc_in);
                    break;
                }
                c_src += dec_in;
                src_size -= dec_in;
                cdst += enc_out;
                dst_size -= enc_out;
                code_point_offset += dec_out;
            }
        }
        catch (ConversionException& ex)
        {
            ex.codepoint_offset += code_point_offset;
            throw;
        }
        return {src_size_0 - src_size, dst_size_0 - dst_size};
    }

    size_t Converter::do_convert(const void* src, size_t src_size,
                                 std::ostream& dst,
                                 bool src_is_final)
    {
        if (buffer_.empty())
            buffer_.resize(BUFFER_SIZE);

        auto original_size = src_size;
        auto c_src = static_cast<const char*>(src);
        while (src_size != 0)
        {
            auto [dec_in, dec_out] = decoder_->decode(c_src, src_size,
                                                      buffer_.data(),
                                                      buffer_.size(),
                                                      src_is_final);
            if (dec_in == 0)
                break;
            encoder_->encode(buffer_.data(), dec_out, dst);
            c_src += dec_in;
            src_size -= dec_in;
        }
        return original_size - src_size;
    }
}
