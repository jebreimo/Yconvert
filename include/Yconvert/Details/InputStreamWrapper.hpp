//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-03-16.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <iosfwd>
#include <vector>
#include "Yconvert/YconvertDefinitions.hpp"

namespace Yconvert::Details
{
    /**
     * @brief A wrapper for std::istream that allows reading from the stream
     *     in small chunks.
     */
    class YCONVERT_API InputStreamWrapper
    {
    public:
        explicit InputStreamWrapper(std::istream& stream);

        /**
         * @brief Fills the buffer with data from the stream.
         * @return True if the buffer has contents, false if the stream is at
         *     the end and the buffer is empty.
         */
        bool fill();

        /**
         * @brief Removes the first n bytes from the buffer.
         * @param n The number of bytes to remove.
         */
        void drain(size_t n);

        /**
         * @brief Returns a pointer to the first byte in the buffer.
         */
        [[nodiscard]]
        const char* data() const;

        /**
         * @brief Returns the number of bytes in the buffer.
         */
        [[nodiscard]]
        size_t size() const;

        /**
         * @brief Returns true if the stream is at the end.
         */
        [[nodiscard]]
        bool eof() const;
    private:
        std::istream* stream_;
        char buffer_[Yconvert::BUFFER_SIZE];
        size_t size_ = 0;
        size_t offset_ = 0;
    };
}
