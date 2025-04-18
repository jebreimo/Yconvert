//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-02-16.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/EncodingChecker.hpp"
#include "MakeEncodersAndDecoders.hpp"

namespace Yconvert
{
    EncodingChecker::EncodingChecker() = default;

    EncodingChecker::EncodingChecker(Encoding encoding)
        : decoder_(make_decoder(encoding))
    {}

    EncodingChecker::EncodingChecker(const EncodingChecker& other)
        : decoder_(other.decoder_ ? make_decoder(other.decoder_->encoding())
                                  : nullptr)
    {}

    EncodingChecker::EncodingChecker(EncodingChecker&& other) noexcept
        : decoder_(std::move(other.decoder_))
    {}

    EncodingChecker::~EncodingChecker() = default;

    EncodingChecker& EncodingChecker::operator=(const EncodingChecker& other)
    {
        if (this != &other)
            decoder_ = other.decoder_ ? make_decoder(other.decoder_->encoding())
                                      : nullptr;
        return *this;
    }

    EncodingChecker& EncodingChecker::operator=(EncodingChecker&& other) noexcept
    {
        if (this != &other)
            decoder_ = std::move(other.decoder_);
        return *this;
    }

    Encoding EncodingChecker::encoding() const
    {
        if (!decoder_)
            return Encoding::UNKNOWN;
        return decoder_->encoding();
    }

    std::pair<size_t, size_t>
    EncodingChecker::count_valid_codepoints(const void* buffer, size_t length) const
    {
        if (!decoder_)
            return {0, 0};
        return decoder_->count_valid_codepoints(buffer, length);
    }

    bool EncodingChecker::check_encoding(const void* buffer, size_t length) const
    {
        if (!decoder_)
            return false;
        auto bytes = count_valid_codepoints(buffer, length).second;
        if (bytes == length)
            return true;

        char32_t chr;
        auto count = decoder_->decode(
            static_cast<const char*>(buffer) + bytes,
            length - bytes, &chr, 1).second;
        return count == 1 && chr == 0;
    }
}
