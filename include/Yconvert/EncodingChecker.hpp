//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-02-16.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <memory>
#include "Encoding.hpp"

namespace Yconvert
{
    class Decoder;

    class YCONVERT_API EncodingChecker
    {
    public:
        EncodingChecker();

        explicit EncodingChecker(Encoding encoding);

        EncodingChecker(const EncodingChecker&);

        EncodingChecker(EncodingChecker&&) noexcept;

        ~EncodingChecker();

        EncodingChecker& operator=(const EncodingChecker&);

        EncodingChecker& operator=(EncodingChecker&&) noexcept;

        [[nodiscard]]
        Encoding encoding() const;

        [[nodiscard]]
        std::pair<size_t, size_t>
        count_valid_codepoints(const void* buffer, size_t length) const;

        [[nodiscard]]
        bool check_encoding(const void* buffer, size_t length) const;
    private:
        std::unique_ptr<Decoder> decoder_;
    };
}
