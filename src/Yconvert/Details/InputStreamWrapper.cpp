//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-03-16.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Details/InputStreamWrapper.hpp"
#include "Yconvert/YconvertThrow.hpp"
#include <istream>

namespace Yconvert::Details
{
    InputStreamWrapper::InputStreamWrapper(std::istream& stream) // NOLINT(*-pro-type-member-init)
        : stream_(&stream)
    {}

    bool InputStreamWrapper::fill()
    {
        if (offset_ != 0 && size_ != 0)
        {
            std::copy(buffer_ + ptrdiff_t(offset_),
                      buffer_ + ptrdiff_t(offset_ + size_),
                      buffer_);
        }
        offset_ = 0;
        stream_->read(buffer_ + size_,
                      std::streamsize(sizeof(buffer_) - size_));
        size_ += stream_->gcount();
        return size_ != 0;
    }

    void InputStreamWrapper::drain(size_t n)
    {
        if (n > size_)
        {
            YCONVERT_THROW("Trying to drain more bytes (" + std::to_string(n)
                           + ") than are available (" + std::to_string(size_)
                           + ").");
        }

        offset_ += n;
        size_ -= n;
    }

    const char* InputStreamWrapper::data() const
    {
        return buffer_ + offset_;
    }

    size_t InputStreamWrapper::size() const
    {
        return size_;
    }

    bool InputStreamWrapper::eof() const
    {
        return stream_->eof();
    }
}
