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

namespace Yconvert
{
    class YCONVERT_API Iterator
    {
    public:
        Iterator() = default;

        Iterator(const void* buffer, size_t size, Encoding encoding);

        template <typename CharType>
        Iterator(const std::basic_string_view<CharType>& str, Encoding encoding)
            : Iterator(str.data(), str.size() * sizeof(CharType), encoding)
        {}

        Iterator(std::istream& stream, Encoding encoding);

        Iterator(const Iterator&) = delete;

        Iterator(Iterator&&) noexcept;

        ~Iterator();

        Iterator& operator=(const Iterator&) = delete;

        Iterator& operator=(Iterator&&) noexcept;

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
}
