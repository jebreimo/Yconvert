//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-07-02.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <iosfwd>
#include <string>

namespace Yconvert
{
    class BufferIterator
    {
    public:
        BufferIterator(const void* buffer, size_t size);

        bool next(char32_t* c);
    private:
        const char* m_Buffer;
        size_t m_Size;
    };

    class StreamIterator
    {
    public:
        StreamIterator(std::istream& stream);

        bool next(char32_t* c);
    };

    template <typename CharType>
    class StringIterator
    {
    public:
        StringIterator(const std::basic_string<CharType>& str);

        bool next(char32_t* c);
    };
}