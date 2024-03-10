//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-07-24.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Convert.hpp"

#include <sstream>
#include <catch2/catch_test_macros.hpp>
#include "U8Adapter.hpp"

using namespace Yconvert;

namespace
{
    std::wstring LONG_WSTRING = {
        L"Property(Name = D.01_Prosess_1, Value = 74.4191, Utlegging av matjord (A-sjikt))Property(Name ="
        L" D.02_Prosess_2, Value = 74.4191, Utlegging av matjord (B-sjikt))Property(Name ="
        L" A.01_Objektnavn, Value = )Property(Name = E.01_Overflatebehandling, Value = Matjorda skal"
        L" legges ut løst med ujevn overflate på ruglete/løs/ujevn undergrunnsmasse. Matjorda skal ikke"
        L" komprimeres.)Property(Name = E.02_Sjikt_01, Value = Matjord, A-sjikt, t= avhengig av avtak,"
        L" min. 50 cm)Property(Name = E.03_Sjikt_02, Value = Matjord, B-sjikt, t= avhengig av avtak, ca"
        L" 20-40 cm)Property(Name = E.04_Sjikt_03, Value = Usoldet jord fra linja, til traubunn/fjell)"
        L"Property(Name = A.02_Objektkode_tekst, Value = Matjord)Property(Name = A.03_Objektkode_nummer,"
        L" Value = 744190)Property(Name = A.04_Beskrivelse, Value = Mellomlagret matjord tilbakeført fra"
        L" ranker på samme eiendom.)Property(Name = B.01_Parsell, Value = )Property(Name = B.02_Område,"
        L" Value = )Property(Name = C.01_MMI, Value = )Property(Name = D.03_Prosess_3, Value = 25.223,"
        L" Usoldet jord fra linja)Property(Name = F.01_Merknad, Value = Omfatter også nødvendig"
        L" utsortering av stubber, røtter, kvist og stein/klumper med tykkelse 50-100mm. All"
        L" tilbakeføring skal skje med beltegående maskin.)Property(Name = G.01_Vertikalnivå, Value = 1)"
    };

    constexpr char8_t LONG_STRING[] = {
        u8"Property(Name = D.01_Prosess_1, Value = 74.4191, Utlegging av matjord (A-sjikt))Property(Name ="
        u8" D.02_Prosess_2, Value = 74.4191, Utlegging av matjord (B-sjikt))Property(Name ="
        u8" A.01_Objektnavn, Value = )Property(Name = E.01_Overflatebehandling, Value = Matjorda skal"
        u8" legges ut løst med ujevn overflate på ruglete/løs/ujevn undergrunnsmasse. Matjorda skal ikke"
        u8" komprimeres.)Property(Name = E.02_Sjikt_01, Value = Matjord, A-sjikt, t= avhengig av avtak,"
        u8" min. 50 cm)Property(Name = E.03_Sjikt_02, Value = Matjord, B-sjikt, t= avhengig av avtak, ca"
        u8" 20-40 cm)Property(Name = E.04_Sjikt_03, Value = Usoldet jord fra linja, til traubunn/fjell)"
        u8"Property(Name = A.02_Objektkode_tekst, Value = Matjord)Property(Name = A.03_Objektkode_nummer,"
        u8" Value = 744190)Property(Name = A.04_Beskrivelse, Value = Mellomlagret matjord tilbakeført fra"
        u8" ranker på samme eiendom.)Property(Name = B.01_Parsell, Value = )Property(Name = B.02_Område,"
        u8" Value = )Property(Name = C.01_MMI, Value = )Property(Name = D.03_Prosess_3, Value = 25.223,"
        u8" Usoldet jord fra linja)Property(Name = F.01_Merknad, Value = Omfatter også nødvendig"
        u8" utsortering av stubber, røtter, kvist og stein/klumper med tykkelse 50-100mm. All"
        u8" tilbakeføring skal skje med beltegående maskin.)Property(Name = G.01_Vertikalnivå, Value = 1)"
    };
}

TEST_CASE("Convert empty UTF-8 to UTF-32")
{
    std::string_view s;
    REQUIRE(convert_to<std::u32string>(s, Encoding::UTF_8, Encoding::UTF_32_NATIVE).empty());
}

TEST_CASE("Convert ISO 8859-10 -> UTF-8")
{
    std::string s = "AB\xE7\xF1";
    REQUIRE(convert_to<std::string>(s, Encoding::ISO_8859_10, Encoding::UTF_8) == U8("ABįņ"));
}

TEST_CASE("Convert ISO 8859-10 -> UTF-16")
{
    std::string s = "AB\xE7\xF1";
    REQUIRE(convert_to<std::u16string>(s, Encoding::ISO_8859_10, Encoding::UTF_16_NATIVE) == u"ABįņ");
}

TEST_CASE("Convert UTF-32 -> UTF-16")
{
    std::u32string s = U"ABįņ";
    REQUIRE(
        convert_to<std::u16string>(s, Encoding::UTF_32_NATIVE, Encoding::UTF_16_NATIVE) == u"ABįņ");
}

TEST_CASE("Convert long wstring")
{
    std::wstring w = LONG_WSTRING;
    std::string s(reinterpret_cast<const char*>(LONG_STRING), sizeof(LONG_STRING) - 1);
    REQUIRE(convert_to<std::string>(w, Encoding::WSTRING_NATIVE, Encoding::UTF_8) == s);
}

TEST_CASE("Use convert to repair an invalid UTF-8 string")
{
    const char u8[] = "Q\xC3\xA5R\xF0\x9F\x98\x80S\xE2\x98T";
    auto result = convert_to<std::string>(std::string_view(u8),
                                          Encoding::UTF_8,
                                          Encoding::UTF_8,
                                          ErrorPolicy::REPLACE);
    REQUIRE(result == "Q\xC3\xA5R\xF0\x9F\x98\x80S\xEF\xBF\xBDT");
}

TEST_CASE("Convert UTF-16 stream to a UTF-8 string")
{
    std::stringstream ss;
    ss.write("\xFF\xFE""A\0""B\0", 6);
    ss.seekg(0, std::ios::beg);
    auto result = convert_to<std::u8string>(ss, Encoding::UTF_16_LE, Encoding::UTF_8);
    REQUIRE(result == u8"\uFEFFAB");
}

TEST_CASE("Convert an invalid UTF-8 stream to a UTF-16 string")
{
    std::stringstream ss;
    ss.write("ABC_\xE2\x9C_STU", 10);
    ss.seekg(0, std::ios::beg);
    auto result = convert_to<std::u16string>(ss, Encoding::UTF_8, Encoding::UTF_16_NATIVE);
    REQUIRE(result == u"ABC_\uFFFD_STU");
}

TEST_CASE("Convert a multi-buffer UTF-16 stream to UTF-8")
{
    std::stringstream ss;
    constexpr auto N = Yconvert::BUFFER_SIZE / 2 + 3;
    for (size_t i = 0; i < N - 4; ++i)
        ss.write("\0A", 2);
    ss.write("\xD8\x00\xDC\x00", 4);
    ss.seekg(0, std::ios::beg);
    auto result = convert_to<std::string>(ss, Encoding::UTF_16_BE, Encoding::UTF_8);
    REQUIRE(result.size() == N);
    REQUIRE(result[N - 4] == '\xF0');
    REQUIRE(result[N - 3] == '\x90');
    REQUIRE(result[N - 2] == '\x80');
    REQUIRE(result[N - 1] == '\x80');
}

TEST_CASE("Convert a UTF-16 stream ending on a incomplete surrogate pair to UTF-8")
{
    std::stringstream ss;
    constexpr auto N = Yconvert::BUFFER_SIZE / 2 + 2;
    for (size_t i = 0; i < N - 3; ++i)
        ss.write("\0A", 2);
    ss.write("\xD8\x00", 2);
    ss.seekg(0, std::ios::beg);
    auto result = convert_to<std::string>(ss, Encoding::UTF_16_BE, Encoding::UTF_8);
    REQUIRE(result.size() == N);
    REQUIRE(result[N - 3] == '\xEF');
    REQUIRE(result[N - 2] == '\xBF');
    REQUIRE(result[N - 1] == '\xBD');
}

TEST_CASE("Convert a UTF-8 string to a UTF-16 encoded stream")
{
    std::string s = "AB\xC3\xA7\xC3\xB1";
    std::ostringstream ss;
    Yconvert::convert_string(std::string_view(s), Encoding::UTF_8,
                             ss, Encoding::UTF_16_LE);
    std::string expected("A\0B\0\xE7\0\xF1\0", 8);
    REQUIRE(ss.str() == expected);
}
