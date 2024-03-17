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
    InputStreamWrapper::InputStreamWrapper(std::istream& stream)
        : stream_(&stream),
          buffer_(Yconvert::BUFFER_SIZE)
    {}

    bool InputStreamWrapper::fill()
    {
        stream_->read(buffer_.data() + buffer_size_,
                      std::streamsize(buffer_.size() - buffer_size_));
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
            std::copy(buffer_.begin() + ptrdiff_t(n), buffer_.end(),
                      buffer_.begin());
            buffer_size_ -= n;
        }
    }

    const char* InputStreamWrapper::data() const
    {
        return buffer_.data();
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
