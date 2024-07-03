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
#include <string_view>
#include "Yconvert/Encoding.hpp"
#include "ErrorPolicy.hpp"

namespace Yconvert
{
    class YCONVERT_API Utf32Iterator
    {
    public:
        Utf32Iterator();

        Utf32Iterator(const void* buffer, size_t size,
                      Encoding encoding,
                      ErrorPolicy error_policy = ErrorPolicy::REPLACE);

        template <typename CharType>
        Utf32Iterator(const std::basic_string_view<CharType>& str,
                      Encoding encoding,
                      ErrorPolicy error_policy = ErrorPolicy::REPLACE)
            : Utf32Iterator(str.data(), str.size() * sizeof(CharType),
                            encoding,
                            error_policy)
        {}

        Utf32Iterator(std::istream& stream,
                      Encoding encoding,
                      ErrorPolicy error_policy);

        Utf32Iterator(const Utf32Iterator&) = delete;

        Utf32Iterator(Utf32Iterator&&) noexcept;

        ~Utf32Iterator();

        Utf32Iterator& operator=(const Utf32Iterator&) = delete;

        Utf32Iterator& operator=(Utf32Iterator&&) noexcept;

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
     * @brief Adapts a Utf32Iterator to be used in range-based for loops.
     *
     * This class is not intended to be used directly, use the begin() and
     * end() functions on Utf32Iterator instead.
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

        explicit Utf32InputIteratorAdapter(Utf32Iterator& iterator)
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

        Utf32Iterator* iterator_ = nullptr;
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

    inline auto begin(Utf32Iterator& iterator)
    {
        return Utf32InputIteratorAdapter(iterator);
    }

    inline auto end(const Utf32Iterator&)
    {
        return Utf32InputIteratorAdapter();
    }
}
