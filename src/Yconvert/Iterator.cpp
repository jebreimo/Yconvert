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
#include "Yconvert/Details/InputStreamWrapper.hpp"
#include "MakeEncodersAndDecoders.hpp"

namespace Yconvert
{
    namespace
    {
        constexpr size_t CHAR32_BUFFER_SIZE = 256;

        struct StreamReader
        {
            explicit StreamReader(std::istream& stream)
                : input_(stream)
            {}

            size_t read(char32_t* buffer, size_t size, const DecoderBase& decoder)
            {
                auto [read, written] = decoder.decode(input_.data(), input_.size(),
                                                      buffer, size);
                input_.drain(read);
                if (written < size && !input_.eof())
                {
                    read = input_.fill();
                    if (read == 0)
                        return 0;
                    std::tie(read, written) = decoder.decode(input_.data(), input_.size(),
                                                             buffer + written, size - written);
                    input_.drain(read);
                }
                return written;
            }

            Details::InputStreamWrapper input_;
        };

        struct BufferReader
        {
            explicit BufferReader(std::span<const char> buffer)
                : input_(buffer)
            {}

            size_t read(char32_t* buffer, size_t size, const DecoderBase& decoder)
            {
                auto [read, written] = decoder.decode(input_.data(), input_.size(),
                                                      buffer, size);
                input_ = input_.subspan(read);
                return written;
            }

            std::span<const char> input_;
        };
        using Source = std::variant<StreamReader, BufferReader>;
    }

    struct Iterator::Data
    {
        explicit Data(std::span<const char> buffer) // NOLINT(*-pro-type-member-init)
            : source(BufferReader(buffer))
        {}

        explicit Data(std::istream& stream) // NOLINT(*-pro-type-member-init)
            : source(StreamReader(stream))
        {}

        Source source;
        char32_t buffer[CHAR32_BUFFER_SIZE];
        std::unique_ptr<DecoderBase> decoder;
    };

    Iterator::Iterator(const void* buffer, size_t size, Encoding encoding)
        : data_(std::make_unique<Data>(std::span{static_cast<const char*>(buffer), size}))
    {
        data_->decoder = make_decoder(encoding);
    }

    Iterator::Iterator(std::istream& stream, Encoding encoding)
        : data_(std::make_unique<Data>(stream))
    {
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
        struct Visitor
        {
            size_t operator()(StreamReader& reader) const
            {
                return reader.read(data.buffer, std::size(data.buffer), *data.decoder);
            }

            size_t operator()(BufferReader& reader) const
            {
                return reader.read(data.buffer, std::size(data.buffer), *data.decoder);
            }

            Iterator::Data& data;
        };

        auto size = std::visit(Visitor{*data_}, data_->source);
        chars_ = std::u32string_view(data_->buffer, size);
        return size != 0;
    }
}
