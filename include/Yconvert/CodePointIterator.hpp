//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-07-02.
//
// This file is distributed under the BSD License.
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
     * for (char32_t c : CodePointIterator(text, Encoding::UTF8))
     * {
     *    // Do something with c...
     * }
     * @endcode
     */
    class YCONVERT_API CodePointIterator
    {
    public:
        CodePointIterator();

        CodePointIterator(const void* buffer, size_t size,
                          Encoding encoding,
                          ErrorPolicy error_policy = ErrorPolicy::REPLACE);

        template <typename CharType>
        CodePointIterator(std::basic_string_view<CharType> str,
                          Encoding encoding,
                          ErrorPolicy error_policy = ErrorPolicy::REPLACE)
            : CodePointIterator(str.data(), str.size() * sizeof(CharType),
                                encoding,
                                error_policy)
        {}

        template <typename CharType>
        CodePointIterator(std::span<CharType> str,
                          Encoding encoding,
                          ErrorPolicy error_policy = ErrorPolicy::REPLACE)
            : CodePointIterator(str.data(), str.size() * sizeof(CharType),
                                encoding,
                                error_policy)
        {}

        CodePointIterator(std::istream& stream,
                          Encoding encoding,
                          ErrorPolicy error_policy = ErrorPolicy::REPLACE);

        CodePointIterator(const CodePointIterator&) = delete;

        CodePointIterator(CodePointIterator&&) noexcept;

        ~CodePointIterator();

        CodePointIterator& operator=(const CodePointIterator&) = delete;

        CodePointIterator& operator=(CodePointIterator&&) noexcept;

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
     * @brief Adapts a CodePointIterator to be used in range-based for loops.
     *
     * This class is not intended to be used directly, use the begin() and
     * end() functions on CodePointIterator instead.
     */
    class Utf32InputIteratorAdapter
    {
    public:
        typedef char32_t value_type;
        typedef std::input_iterator_tag iterator_category;
        typedef void difference_type;
        typedef const value_type* pointer;
        typedef const value_type& reference;

        Utf32InputIteratorAdapter() = default;

        explicit Utf32InputIteratorAdapter(CodePointIterator& iterator)
            : iterator_(&iterator),
              is_end_(!iterator_->next(&character_))
        {}

        Utf32InputIteratorAdapter(const Utf32InputIteratorAdapter&) = delete;

        Utf32InputIteratorAdapter& operator++(int)
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
        friend bool operator==(const Utf32InputIteratorAdapter& lhs,
                               const Utf32InputIteratorAdapter& rhs);

        CodePointIterator* iterator_ = nullptr;
        char32_t character_ = {};
        bool is_end_ = true;
    };

    inline bool operator==(const Utf32InputIteratorAdapter& lhs,
                           const Utf32InputIteratorAdapter& rhs)
    {
        return lhs.iterator_ == rhs.iterator_ || lhs.is_end_ == rhs.is_end_;
    }

    inline bool operator!=(const Utf32InputIteratorAdapter& lhs,
                           const Utf32InputIteratorAdapter& rhs)
    {
        return !(lhs == rhs);
    }

    inline auto begin(CodePointIterator& iterator)
    {
        return Utf32InputIteratorAdapter(iterator);
    }

    inline auto end(const CodePointIterator&)
    {
        return Utf32InputIteratorAdapter();
    }
}
