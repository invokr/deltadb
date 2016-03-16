/**
 * @file bitstream.hpp
 * @author Robin Dietrich <me (at) invokr (dot) org>
 *
 * @par License
 *   This file is part of deltadb.
 *
 *   Foobar is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Foobar is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DELTADB_INTERNAL_BITSTREAM_HPP
#define DELTADB_INTERNAL_BITSTREAM_HPP

#include <string>

#include <cassert>
#include <cstdint>
#include <cstddef>
#include <cstring>

#define BTYPE uint32_t

namespace deltadb {
    /** Pre-computed bitmasks */
    static constexpr uint64_t masks[64] = {
        0x0,             0x1,              0x3,              0x7,
        0xf,             0x1f,             0x3f,             0x7f,
        0xff,            0x1ff,            0x3ff,            0x7ff,
        0xfff,           0x1fff,           0x3fff,           0x7fff,
        0xffff,          0x1ffff,          0x3ffff,          0x7ffff,
        0xfffff,         0x1fffff,         0x3fffff,         0x7fffff,
        0xffffff,        0x1ffffff,        0x3ffffff,        0x7ffffff,
        0xfffffff,       0x1fffffff,       0x3fffffff,       0x7fffffff,
        0xffffffff,      0x1ffffffff,      0x3ffffffff,      0x7ffffffff,
        0xfffffffff,     0x1fffffffff,     0x3fffffffff,     0x7fffffffff,
        0xffffffffff,    0x1ffffffffff,    0x3ffffffffff,    0x7ffffffffff,
        0xfffffffffff,   0x1fffffffffff,   0x3fffffffffff,   0x7fffffffffff,
        0xffffffffffff,  0x1ffffffffffff,  0x3ffffffffffff,  0x7ffffffffffff,
        0xfffffffffffff, 0x1fffffffffffff, 0x3fffffffffffff, 0x7fffffffffffff
    };

    /** This class provides functions to read and write data as a stream of bits. */
    class bitstream {
    public:
        /** Current I/O mode */
        enum class mode {
            io_unset  = 0,
            io_reader = 1,
            io_writer = 2
        };

        /** Different kinds of possible bitstream errors */
        enum class error {
            none  = 0, // Nothing wrong
            redef = 1, // Trying to redefine buffer or type
            size  = 2  // Buffer size would overflow
        };

        /** Underlying word type */
        typedef BTYPE word_t;

    // Constructors / Destructors
    public:
        /**
         * Constructs an empty bitstream.
         * You are required to invoke set_buffer before calling any other functions
         */
        bitstream()
          : m_error(error::none), m_mode(mode::io_unset), m_buffer(nullptr), m_buffer_bytes(0),
            m_buffer_bits(0), m_pos(0), m_owns_buffer(false) { }

        /** Constructs bitstream from a buffer, defaults to reading mode. */
        bitstream(word_t* buffer, size_t size, mode m = mode::io_reader)
          : m_error(error::none), m_mode(m), m_buffer(buffer), m_buffer_bytes(size), m_buffer_bits(size*8),
            m_pos(0), m_owns_buffer(false)
        {
            if (!verify_size(size)) {
                m_error = error::size;
                m_buffer = nullptr;
                m_buffer_bytes = 0;
                m_buffer_bits = 0;
            }
        }

        /** Constructs bitstream from std::string, read-only */
        bitstream(const std::string& data)
          : m_error(error::none), m_mode(mode::io_reader), m_buffer(nullptr), m_buffer_bytes(0),
            m_buffer_bits(0), m_pos(0), m_owns_buffer(true)
        {
            if (!verify_size(data.size())) {
                m_error = error::size;
                return;
            }

            m_buffer_bytes = (data.size() + 3) / 4 + 1;
            m_buffer_bits = m_buffer_bytes * 8;
            m_buffer = new word_t[m_buffer_bytes];
            memcpy(&m_buffer[0], data.c_str(), data.size());
        }

        /** Constuct bitstream with given size in writing mode. */
        bitstream(const uint32_t size)
          : m_error(error::none), m_mode(mode::io_writer), m_buffer(new word_t[size]), m_buffer_bytes(size),
            m_buffer_bits(size * 8), m_pos(0), m_owns_buffer(true)
        {
            if (!verify_size(size))
                m_error = error::size;
        }

        /** @todo Implement copy constructor and refcount for owning buffer */
        bitstream(const bitstream& b) = delete;

        /** @todo Implement */
        bitstream& operator=(const bitstream&) = delete;

        /** @todo Implement */
        bitstream(bitstream&& b) = default;

        /** @todo Implement */
        bitstream& operator=(bitstream&&) = default;

        /** Destructor, frees buffer if owned */
        ~bitstream() {
            if (m_owns_buffer)
                delete[] m_buffer;
        }

        /** Swap bitstream with given one */
        void swap(bitstream &b) {
            std::swap(m_error, b.m_error);
            std::swap(m_mode, b.m_mode);
            std::swap(m_buffer, b.m_buffer);
            std::swap(m_buffer_bytes, b.m_buffer_bytes);
            std::swap(m_buffer_bits, b.m_buffer_bits);
            std::swap(m_pos, b.m_pos);
            std::swap(m_owns_buffer, b.m_owns_buffer);
        }

        /** Resets bitstream as if you used the default constructor */
        void reset() {
            if (m_owns_buffer)
                delete[] m_buffer;

            // Default values
            m_error = error::none;
            m_mode = mode::io_unset;
            m_buffer = nullptr;
            m_buffer_bytes = 0;
            m_buffer_bits = 0;
            m_pos = 0;
            m_owns_buffer = false;
        }

    // Shared functions and methods
    public:
        /** Whether the bitstream is in a valid state */
        bool valid() {
            return (m_error == error::none && m_mode != mode::io_unset);
        }

        /** Returns error if any */
        error error() {
            return m_error;
        }

        /** Sets I/O mode for this stream. */
        void set_mode(mode io_mode) {
            if (m_mode != mode::io_unset) {
                m_error = error::redef;
                return;
            }

            this->m_mode = io_mode;
        }

        /** Returns current I/O mode */
        mode mode() {
            return m_mode;
        }

        /** Returns whether this bitstream is read only */
        bool is_reader() {
            return m_mode == mode::io_reader;
        }

        /** Returns whether this bitstream is write only */
        bool is_writer() {
            return m_mode == mode::io_writer;
        }

        /** Sets buffer used for reading / writing. */
        void set_buffer(word_t* buffer, uint32_t size) {
            if (m_buffer) {
                m_error = error::redef;
                return;
            }

            m_buffer = buffer;
            m_buffer_bytes = size;
            m_buffer_bits = size*8;
            m_pos = 0;
        }

        /** Returns size in bits */
        uint32_t size() {
            return m_buffer_bits;
        }

        /** Returns size in bytes */
        uint32_t size_bytes() {
            return m_buffer_bytes;
        }

        /** Returns current position in bits */
        uint32_t position() {
            return m_pos;
        }

        /** Returns number of bits left in the stream */
        uint32_t left() {
            return m_buffer_bits - m_pos;
        }

        /** Returns pointer to buffer */
        word_t* buffer() {
            return m_buffer;
        }

        /**
         * Seek to a specific bit.
         *
         * Does a boundary check.
         */
        void seek(uint32_t position) {
            assert(position < m_buffer_bits);
            m_pos = position;
        }

        /** Force stream to align on byte boundary */
        void align() {
            m_pos = ((m_pos >> 3) + 1) << 3;
        }

    // Write only functions
    public:
        /**
         * Writes up to 32 bits from data to the buffer.
         *
         * If bits is not 32, only writes the lower bits.
         */
        void write(uint8_t bits, uint32_t data) {
            assert(m_error == error::none);
            assert(m_mode == mode::io_writer);
            assert(bits <= 32);

            static constexpr uint8_t bitSize = sizeof(word_t) << 3;  // size of a single chunk in bits
            const uint32_t start = m_pos / bitSize;                  // active chunk
            const uint32_t end = (m_pos + bits - 1) / bitSize;       // last chunk
            const uint32_t shift = m_pos % bitSize;                  // shift amount

            if (start == end) {
                m_buffer[start] = (m_buffer[start] & masks[shift]) | (data << shift);
            } else {
                m_buffer[start] = (m_buffer[start] & masks[shift]) | (data << shift);
                m_buffer[end] = (data >> (bitSize - shift)) & masks[bits - (bitSize - shift)];
            }

            m_pos += bits;
        }

        /** Writes specified number of bytes read from data. */
        void write_bytes(const char* data, uint32_t size) {
            if ((m_pos & 7) == 0) {
                memcpy(&(reinterpret_cast<char*>(m_buffer)[m_pos >> 3]), data, size);
            } else {
                for (uint32_t i = 0; i < size; ++i) {
                    write(8, data[i]);
                }
            }
        }

    // Read only functions
    public:
        /** Reads up to 32 bits from the stream */
        uint32_t read(uint8_t bits) {
            assert(m_error == error::none);
            assert(m_mode == mode::io_reader);
            assert(bits <= 32);

            static constexpr uint8_t bitSize = sizeof(word_t) << 3;  // size of a single chunk in bits
            const uint32_t start = m_pos / bitSize;                // current active chunk
            const uint32_t end   = (m_pos + bits - 1) / bitSize;   // next chunk in case of wrapping
            const uint32_t shift = m_pos % bitSize;                // shift amount

            uint32_t ret; // return value
            if (start == end) {
                ret = (m_buffer[start] >> shift) & masks[bits];
            } else {
                ret = ((m_buffer[start] >> shift) | (m_buffer[end] << (bitSize - shift))) & masks[bits];
            }

            m_pos += bits;
            return ret;
        }

        /** Reads number of bytes into buffer */
        void read_bytes(uint32_t bytes, char* dest) {
            if ((m_pos & 7) == 0) {
                memcpy(dest, &(reinterpret_cast<char*>(m_buffer)[m_pos >> 3]), bytes);
            } else {
                for (uint32_t i = 0; i < bytes; ++i) {
                    dest[i] = static_cast<int8_t>(read(8));
                }
            }
        }
    private:
        enum error m_error;
        enum mode m_mode;
        word_t* m_buffer;
        uint32_t m_buffer_bytes;
        uint32_t m_buffer_bits;
        uint32_t m_pos;
        bool m_owns_buffer;

        /** Verifies buffer size */
        bool verify_size(uint32_t size) {
            static size_t size_bits_max = (2^32) - 1;
            return (size * 8) + 1 > size_bits_max;
        }
    };
} /* deltadb */

#endif /* DELTADB_INTERNAL_BITSTREAM_HPP */
