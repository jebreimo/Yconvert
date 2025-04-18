//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-07-22.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Encoding.hpp"
#include "U8Adapter.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace Yconvert;

void test_determine_encoding(const char* s, size_t len,
                             Encoding expected_enc,
                             ptrdiff_t expected_offset = 0)
{
    std::string str(s, len);
    CAPTURE(str, len, expected_enc, expected_offset);
    auto enc = determine_encoding(s, len);
    REQUIRE(enc.first == expected_enc);
    REQUIRE(enc.second == expected_offset);
}

TEST_CASE("Determine encoding from first character")
{
    test_determine_encoding("(abc)", 5, Encoding::UTF_8);
    test_determine_encoding(U8("Å"), 2, Encoding::UTF_8);
    test_determine_encoding("\0(\xAA\xAA", 4, Encoding::UTF_16_BE);
    test_determine_encoding("\xAA\0", 2, Encoding::UTF_16_LE);
    test_determine_encoding("\0\0\xAA\0", 4, Encoding::UTF_32_BE);
    test_determine_encoding("QZ\0\0", 4, Encoding::UTF_32_LE);
    test_determine_encoding("QVZ\0", 4, Encoding::UNKNOWN);
    test_determine_encoding("\0QV", 3, Encoding::UNKNOWN);
    test_determine_encoding("\xEFQ", 2, Encoding::UNKNOWN);
    test_determine_encoding("\xCF", 1, Encoding::UNKNOWN);
    test_determine_encoding("\0", 1, Encoding::UNKNOWN);
    test_determine_encoding("", 0, Encoding::UNKNOWN);
    test_determine_encoding(nullptr, 0, Encoding::UNKNOWN);
}

TEST_CASE("Ambiguous UTF-8")
{
    // Adding this test to highlight a potential issue: is an incomplete
    // sequence of bytes that looks like it could be UTF-8 actually UTF-8?
    // I'm not sure if I want this to succeed.
    test_determine_encoding("Q\xC0", 2, Encoding::UTF_8);
}

TEST_CASE("Determine encoding from BOM")
{
    test_determine_encoding("\xFE\xFFQWER", 6, Encoding::UTF_16_BE, 2);
    test_determine_encoding("\xFF\xFE\0\0QWER", 8, Encoding::UTF_32_LE, 4);
    test_determine_encoding("\xEF\xBB\xBFQWER", 7, Encoding::UTF_8, 3);
}

TEST_CASE("Count valid code points in an invalid UTF-16BE string")
{
    char u16be[] = "\0A\0B\xD8\x00\xDC\x00\xD8\x00\0A";
    auto [cp, bytes] = Yconvert::count_valid_codepoints(
        u16be, sizeof(u16be) - 1, Encoding::UTF_16_BE);
    REQUIRE(cp == 3);
    REQUIRE(bytes == 8);
    REQUIRE(!Yconvert::check_encoding(u16be, sizeof(u16be) - 1, Encoding::UTF_16_BE));
}

TEST_CASE("Count valid code points in a valid UTF-16LE string")
{
    char u16le[] = "Q\0R\0S\0\x00\xD8\x00\xDCT\0";
    auto [cp, bytes] = Yconvert::count_valid_codepoints(
        u16le, sizeof(u16le) - 1, Encoding::UTF_16_LE);
    REQUIRE(cp == 5);
    REQUIRE(bytes == sizeof(u16le) - 1);
    REQUIRE(Yconvert::check_encoding(u16le, sizeof(u16le) - 1, Encoding::UTF_16_LE));
}

TEST_CASE("Count valid code points in an invalid UTF-8 string")
{
    char u8[] = "Q\xC3\xA5R\xF0\x9F\x98\x80S\xE2\x98T";
    auto [cp, bytes] = Yconvert::count_valid_codepoints(
        u8, sizeof(u8) - 1, Encoding::UTF_8);
    REQUIRE(cp == 5);
    REQUIRE(bytes == 9);
    REQUIRE(!Yconvert::check_encoding(u8, sizeof(u8) - 1, Encoding::UTF_8));
}

TEST_CASE("Count valid code points in a zero-terminated UTF-8 string.")
{
    char u8[] = "ABCDEFGHIJ\0KLM";
    auto [cp, bytes] = Yconvert::count_valid_codepoints(
        u8, sizeof(u8), Encoding::UTF_8);
    REQUIRE(cp == 10);
    REQUIRE(bytes == 10);
    REQUIRE(Yconvert::check_encoding(u8, sizeof(u8), Encoding::UTF_8));
}

TEST_CASE("Count valid code points in an invalid ASCII string")
{
    char ascii[] = "ABCDEFGHIJ\x80";
    auto [cp, bytes] = Yconvert::count_valid_codepoints(
        ascii, sizeof(ascii), Encoding::ASCII);
    REQUIRE(cp == 10);
    REQUIRE(bytes == 10);
    REQUIRE(!Yconvert::check_encoding(ascii, sizeof(ascii), Encoding::ASCII));
}

TEST_CASE("Count valid code points in a zero-terminated ASCII string")
{
    char ascii[] = "ABCDEFGHIJ\0KL";
    auto [cp, bytes] = Yconvert::count_valid_codepoints(
        ascii, sizeof(ascii), Encoding::ASCII);
    REQUIRE(cp == 10);
    REQUIRE(bytes == 10);
    REQUIRE(Yconvert::check_encoding(ascii, sizeof(ascii), Encoding::ASCII));
}

TEST_CASE("Count valid code points in zero-terminated UTF32 string")
{
    char32_t u32[] = U"ABCDEFGHIJ\0KLM";
    auto [cp, bytes] = Yconvert::count_valid_codepoints(
        u32, sizeof(u32), Encoding::UTF_32_NATIVE);
    REQUIRE(cp == 10);
    REQUIRE(bytes == 40);
    REQUIRE(Yconvert::check_encoding(u32, sizeof(u32), Encoding::UTF_32_NATIVE));
}
