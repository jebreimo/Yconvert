//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-07-02.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <iosfwd>
#include <memory>
#include <span>
#include <string_view>
#include "Yconvert/Encoding.hpp"
#include "ErrorPolicy.hpp"

namespace Yconvert
{
    /**
     * @brief An iterator that reads code points from a buffer or stream.
     *
     * The iterator reads code points from a buffer or stream and returns
     * them as 32-bit Unicode code points. The buffer or stream is assumed
     * to contain text encoded in a specific encoding. The iterator will
     * convert the text to Unicode code points as it reads it.
     *
     * The iterator can be used in range-based for loops, e.g.:
     * @code
     * std::string text = "Hello, world!";
     * for (char32_t c : CodepointIterator(text, Encoding::UTF8))
     * {
     *    // Do something with c...
     * }
     * @endcode
     *
     * @note This iterator does not support the range concept of std::ranges.
     */
    class YCONVERT_API CodepointIterator
    {
    public:
        CodepointIterator();

        CodepointIterator(const void* buffer, size_t size,
                          Encoding encoding,
                          ErrorPolicy error_policy = ErrorPolicy::REPLACE);

        template <typename CharType>
        CodepointIterator(std::basic_string_view<CharType> str,
                          Encoding encoding,
                          ErrorPolicy error_policy = ErrorPolicy::REPLACE)
            : CodepointIterator(str.data(), str.size() * sizeof(CharType),
                                encoding,
                                error_policy)
        {}

        template <typename CharType>
        CodepointIterator(std::span<CharType> str,
                          Encoding encoding,
                          ErrorPolicy error_policy = ErrorPolicy::REPLACE)
            : CodepointIterator(str.data(), str.size() * sizeof(CharType),
                                encoding,
                                error_policy)
        {}

        CodepointIterator(std::istream& stream,
                          Encoding encoding,
                          ErrorPolicy error_policy = ErrorPolicy::REPLACE);

        CodepointIterator(const CodepointIterator&) = delete;

        CodepointIterator(CodepointIterator&&) noexcept;

        ~CodepointIterator();

        CodepointIterator& operator=(const CodepointIterator&) = delete;

        CodepointIterator& operator=(CodepointIterator&&) noexcept;

        /**
         * @brief Returns the next code point in the buffer or stream.
         * @param c Receives the next code point.
         * @return True if a code point was read, false if the end of the
         *    buffer or stream was reached.
         */
        bool next(char32_t* c)
        {
            if (i_ == chars_.size() && !fill_buffer())
                return false;

            *c = chars_[i_++];
            return true;
        }
    private:
        bool fill_buffer();

        std::u32string_view chars_;
        size_t i_ = 0;
        struct Data;
        std::unique_ptr<Data> data_;
    };


    /**
     * @brief Adapts a CodepointIterator to be used in range-based for loops.
     *
     * This class is not intended to be used directly, use the begin() and
     * end() functions on CodepointIterator instead.
     */
    class CodepointInputIteratorAdapter
    {
    public:
        typedef char32_t value_type;
        typedef std::input_iterator_tag iterator_category;
        typedef void difference_type;
        typedef const value_type* pointer;
        typedef const value_type& reference;

        CodepointInputIteratorAdapter() = default;

        explicit CodepointInputIteratorAdapter(CodepointIterator& iterator)
            : iterator_(&iterator),
              is_end_(!iterator_->next(&character_))
        {}

        CodepointInputIteratorAdapter& operator++(int)
        {
            if (!is_end_)
                is_end_ = !iterator_->next(&character_);
            return *this;
        }

        void operator++()
        {
            if (!is_end_)
                is_end_ = !iterator_->next(&character_);
        }

        const char32_t& operator*() const
        {
            return character_;
        }

        const char32_t* operator->() const
        {
            return &character_;
        }
    private:
        friend bool operator==(const CodepointInputIteratorAdapter& lhs,
                               const CodepointInputIteratorAdapter& rhs);

        CodepointIterator* iterator_ = nullptr;
        char32_t character_ = INVALID_CHAR;
        bool is_end_ = true;
    };

    inline bool operator==(const CodepointInputIteratorAdapter& lhs,
                           const CodepointInputIteratorAdapter& rhs)
    {
        return lhs.iterator_ == rhs.iterator_ || lhs.is_end_ == rhs.is_end_;
    }

    inline bool operator!=(const CodepointInputIteratorAdapter& lhs,
                           const CodepointInputIteratorAdapter& rhs)
    {
        return !(lhs == rhs);
    }

    inline auto begin(CodepointIterator& iterator)
    {
        return CodepointInputIteratorAdapter(iterator);
    }

    inline auto end(const CodepointIterator&)
    {
        return CodepointInputIteratorAdapter();
    }
}
