//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-07-07.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Utf32Decoder.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test UTF-32 decoder")
{
    std::string s("\0\0\0A\0\0\0B", 8);
    Yconvert::Utf32BEDecoder decoder;
    std::vector<char32_t> u(2);
    auto[m, n] = decoder.decode(s.data(), s.size(), u.data(), u.size());
    REQUIRE(m == 8);
    REQUIRE(n == 2);
    REQUIRE(u[0] == 'A');
    REQUIRE(u[1] == 'B');
}
