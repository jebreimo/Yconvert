//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-07-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Utf32Iterator.hpp"

#include <sstream>
#include <vector>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Iterate over a buffer")
{
    std::string buffer = "Hello!";
    Yconvert::Utf32Iterator iter(buffer.data(), buffer.size(),
                                 Yconvert::Encoding::ASCII);
    char32_t c;
    REQUIRE(iter.next(&c));
    REQUIRE(c == 'H');
    REQUIRE(iter.next(&c));
    REQUIRE(c == 'e');
    REQUIRE(iter.next(&c));
    REQUIRE(c == 'l');
    REQUIRE(iter.next(&c));
    REQUIRE(c == 'l');
    REQUIRE(iter.next(&c));
    REQUIRE(c == 'o');
    REQUIRE(iter.next(&c));
    REQUIRE(c == '!');
    REQUIRE(!iter.next(&c));
}

TEST_CASE("Iterate over a stream")
{
    std::stringstream stream;
    // Write all 3-byte UTF-8 characters to the stream.
    for (unsigned i = 1 << 11; i < 1 << 16; ++i)
    {
        stream << static_cast<char>(0xE0 | (i >> 12))
               << static_cast<char>(0x80 | ((i >> 6) & 0x3F))
               << static_cast<char>(0x80 | (i & 0x3F));
    }

    stream.seekg(0);
    Yconvert::Utf32Iterator iter(stream, Yconvert::Encoding::UTF_8);
    unsigned n = 1 << 11;
    char32_t c;
    while (iter.next(&c))
    {
        REQUIRE(c == n++);
    }
    REQUIRE(n == 1 << 16);
}

TEST_CASE("Range-based for loop")
{
    std::vector<char16_t> buffer = {'A', 0xD900, 0xDD00, 'B'};
    std::u32string_view expected = U"A\U00050100B";

    for (auto c: Yconvert::Utf32Iterator(std::span(buffer),
                                         Yconvert::Encoding::UTF_16_NATIVE))
    {
        REQUIRE(!expected.empty());
        REQUIRE(c == expected.front());
        expected.remove_prefix(1);
    }
}
