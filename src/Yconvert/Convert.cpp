//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-06-12.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Convert.hpp"

namespace Yconvert
{
    std::pair<size_t, size_t>
    convert(const void* source, size_t source_size,
            void* destination, size_t destination_size,
            Converter& converter)
    {
        return converter.convert(source, source_size,
                                 destination, destination_size, true);
    }

    std::pair<size_t, size_t>
    convert(const void* source, size_t source_size,
            Encoding source_encoding,
            void* destination, size_t destination_size,
            Encoding destination_encoding,
            ErrorPolicy error_policy)
    {
        Converter converter(source_encoding, destination_encoding);
        converter.set_error_policy(error_policy);
        return converter.convert(source, source_size,
                                 destination, destination_size, true);
    }

    void convert(const void* source, size_t source_size,
                 std::ostream& destination,
                 Converter& converter)
    {
        converter.convert(source, source_size, destination, true);
    }

    void convert(std::istream& source,
                 std::string& destination,
                 Converter& converter)
    {
        std::vector<char> buffer(BUFFER_SIZE);
        size_t source_offset = 0;
        for (;;)
        {
            source.read(buffer.data() + source_offset,
                        std::streamsize(buffer.size() - source_offset));
            auto bytes_read = source.gcount();
            auto buf_size = bytes_read + source_offset;
            if (buf_size == 0)
                break;
            bool src_is_final = source.eof();
            auto src_siz = converter.convert(buffer.data(), buf_size,
                                             destination, src_is_final);
            if (src_siz != buf_size)
            {
                std::copy(buffer.begin() + ptrdiff_t(src_siz), buffer.end(),
                          buffer.begin());
                source_offset = buffer.size() - src_siz;
            }
            else
            {
                source_offset = 0;
            }
        }
    }

    void convert(std::istream& source,
                 std::ostream& destination,
                 Converter& converter)
    {

    }

    void convert(std::istream& source, Encoding source_encoding,
                 std::ostream& destination, Encoding destination_encoding,
                 ErrorPolicy error_policy)
    {
        Converter converter(source_encoding, destination_encoding);
        converter.set_error_policy(error_policy);
        convert(source, destination, converter);
    }
}
