//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-02-16.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <memory>
#include "Decoder.hpp"
#include "Encoder.hpp"

namespace Yconvert
{
    std::unique_ptr<Decoder> make_decoder(Encoding encoding);

    std::unique_ptr<Encoder> make_encoder(Encoding encoding);
}
