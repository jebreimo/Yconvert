//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-02-16.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <memory>
#include "DecoderBase.hpp"
#include "EncoderBase.hpp"

namespace Yconvert
{
    std::unique_ptr<DecoderBase> make_decoder(Encoding encoding);

    std::unique_ptr<EncoderBase> make_encoder(Encoding encoding);
}