/**
 * @file bitfield.hpp
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

#ifndef DELTADB_INTERNAL_BITFIELD_HPP
#define DELTADB_INTERNAL_BITFIELD_HPP

#include <cassert>
#include <cstdint>

namespace deltadb {
    /** Bitfield */
    class bitfield {
    public:
        /** Constructor */
        bitfield() : m_storage(0), m_size(0) {};

        /** Constructor, sets size */
        bitfield(uint8_t size) : m_storage(0), m_size(size) {
            assert(size <= 64);
        }

        /** Resize bitfield */
        void resize(uint8_t size) {
            assert(size <= 64);
            m_size = size;
        }

        /** Return bit at index */
        bool at(uint8_t idx) {
            assert(idx < m_size);
            return m_storage & bit_at(idx);
        }

        /** Set bit at index */
        void set(uint8_t idx, bool value) {
            assert(idx < m_size);
            m_storage |= bit_at(idx);
        }

        /** Return bitfield size */
        uint8_t size() {
            return m_size;
        }

        /** Return bytes require to store */
        constexpr uint8_t width() {
            return ((m_size + 7) & ~7) >> 3;
        }

        /** Return all bits */
        uint64_t storage() {
            return m_storage;
        }
    private:
        uint64_t m_storage;
        uint8_t m_size;

        constexpr uint64_t bit_at(uint8_t idx) {
            return static_cast<uint64_t>(1) << static_cast<uint64_t>(idx);
        }
    };
}

#endif /* DELTADB_INTERNAL_BITFIELD_HPP */
