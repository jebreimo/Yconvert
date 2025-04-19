//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-07-02.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/CodepointIterator.hpp"
#include <istream>
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

            size_t read(char32_t* buffer, size_t size, const Decoder& decoder)
            {
                auto [read, written] = decoder.decode(input_.data(), input_.size(),
                                                      buffer, size, input_.eof());
                input_.drain(read);
                if (written < size && !input_.eof())
                {
                    if (!input_.fill())
                        return written;
                    auto [r, w] = decoder.decode(input_.data(), input_.size(),
                                                 buffer + written, size - written,
                                                 input_.eof());
                    written += w;
                    input_.drain(r);
                }
                return written;
            }

            [[nodiscard]] bool is_complete() const
            {
                return input_.eof();
            }

            Details::InputStreamWrapper input_;
        };

        struct BufferReader
        {
            explicit BufferReader(std::span<const char> buffer)
                : input_(buffer)
            {}

            size_t read(char32_t* buffer, size_t size, const Decoder& decoder)
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

    struct CodepointIterator::Data
    {
        explicit Data(std::span<const char> buffer) // NOLINT(*-pro-type-member-init)
            : source(BufferReader(buffer))
        {}

        explicit Data(std::istream& stream) // NOLINT(*-pro-type-member-init)
            : source(StreamReader(stream))
        {}

        Source source;
        char32_t buffer[CHAR32_BUFFER_SIZE];
        std::unique_ptr<Decoder> decoder;
    };

    CodepointIterator::CodepointIterator() = default;

    CodepointIterator::CodepointIterator(const void* buffer, size_t size,
                                         Encoding encoding,
                                         ErrorPolicy error_policy)
        : data_(std::make_unique<Data>(std::span{static_cast<const char*>(buffer), size}))
    {
        data_->decoder = make_decoder(encoding);
        data_->decoder->set_error_policy(error_policy);
    }

    CodepointIterator::CodepointIterator(std::istream& stream,
                                         Encoding encoding,
                                         ErrorPolicy error_policy)
        : data_(std::make_unique<Data>(stream))
    {
        data_->decoder = make_decoder(encoding);
        data_->decoder->set_error_policy(error_policy);
    }

    CodepointIterator::CodepointIterator(CodepointIterator&& other) noexcept
        : chars_(other.chars_),
          i_(other.i_),
          data_(std::move(other.data_))
    {
        other.chars_ = {};
        other.i_ = 0;
    }

    CodepointIterator::~CodepointIterator() = default;

    CodepointIterator& CodepointIterator::operator=(CodepointIterator&& other) noexcept
    {
        if (this == &other)
            return *this;

        chars_ = other.chars_;
        i_ = other.i_;
        data_ = std::move(other.data_);
        other.chars_ = {};
        other.i_ = 0;
        return *this;
    }

    bool CodepointIterator::fill_buffer()
    {
        if (!data_)
            return false;

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

            CodepointIterator::Data& data;
        };

        auto size = std::visit(Visitor{*data_}, data_->source);
        chars_ = std::u32string_view(data_->buffer, size);
        i_ = 0;
        return size != 0;
    }
}
