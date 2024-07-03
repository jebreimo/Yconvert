//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-03-16.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yconvert/Details/InputStreamWrapper.hpp"
#include <istream>

namespace Yconvert::Details
{
    InputStreamWrapper::InputStreamWrapper(std::istream& stream) // NOLINT(*-pro-type-member-init)
        : stream_(&stream)
    {}

    bool InputStreamWrapper::fill()
    {
        stream_->read(buffer_ + buffer_size_,
                      std::streamsize(sizeof(buffer_) - buffer_size_));
        buffer_size_ += stream_->gcount();
        return buffer_size_ != 0;
    }

    void InputStreamWrapper::drain(size_t n)
    {
        if (n == buffer_size_)
        {
            buffer_size_ = 0;
        }
        else if (n < buffer_size_)
        {
            std::copy(buffer_ + ptrdiff_t(n), std::end(buffer_),
                      buffer_);
            buffer_size_ -= n;
        }
    }

    const char* InputStreamWrapper::data() const
    {
        return buffer_;
    }

    size_t InputStreamWrapper::size() const
    {
        return buffer_size_;
    }

    bool InputStreamWrapper::eof() const
    {
        return stream_->eof();
    }
}
