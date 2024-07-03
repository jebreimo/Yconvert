//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-07-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Utf32Iterator.hpp"
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