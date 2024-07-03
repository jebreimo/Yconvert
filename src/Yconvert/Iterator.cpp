//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-07-02.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Iterator.hpp"
#include <array>
#include <istream>
#include <span>
#include <variant>

#include "MakeEncodersAndDecoders.hpp"

namespace Yconvert
{
    using Source = std::variant<std::istream*, std::span<const char>>;

    struct Iterator::Data
    {
        Source source;
        char32_t buffer[256];
        size_t buffer_size = 0;
        std::unique_ptr<DecoderBase> decoder;
    };

    Iterator::Iterator(const void* buffer, size_t size, Encoding encoding)
        : data_(std::make_unique<Data>())
    {
        data_->source = std::span(static_cast<const char*>(buffer), size);
        data_->decoder = make_decoder(encoding);
    }

    Iterator::Iterator(std::istream& stream, Encoding encoding)
        : data_(std::make_unique<Data>())
    {
        data_->source = &stream;
        data_->decoder = make_decoder(encoding);
    }

    Iterator::Iterator(Iterator&& other) noexcept
        : chars_(other.chars_),
          i_(other.i_),
          data_(std::move(other.data_))
    {
        other.i_ = 0;
    }

    Iterator::~Iterator() = default;

    Iterator& Iterator::operator=(Iterator&& other) noexcept
    {
        chars_ = other.chars_;
        i_ = other.i_;
        data_ = std::move(other.data_);
        other.i_ = 0;
        return *this;
    }

    bool Iterator::fill_buffer()
    {
        auto& data = *data_;
        if (data.buffer_size == 0)
        {
            if (std::holds_alternative<std::istream*>(data.source))
            {
                auto& stream = *std::get<std::istream*>(data.source);
                if (!stream)
                    return false;
                stream.read(data.buffer, sizeof(data.buffer));
                data.buffer_size = stream.gcount();
            }
            else
            {
                auto& span = std::get<std::span<const char>>(data.source);
                if (span.empty())
                    return false;
                data.buffer_size = std::min(span.size(), sizeof(data.buffer));
                std::copy(span.begin(), span.begin() + data.buffer_size, data.buffer);
                span = span.subspan(data.buffer_size);
            }
        }

        auto [read, written] = data.decoder->decode(data.buffer, data.buffer_size,
                                                    data.buffer, sizeof(data.buffer));
        data.buffer_size = read;
        chars_ = std::u32string_view(data.buffer, written);
        i_ = 0;
        return true;
    }
}
