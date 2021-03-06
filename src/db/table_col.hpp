/**
 * @file table_col.hpp
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

#ifndef DELTADB_DB_TABLE_COL_HPP
#define DELTADB_DB_TABLE_COL_HPP

#include "../internal/platform.hpp"

namespace deltadb {
    // forward decl
    class bitstream;

    /** Different possible column types */
    enum col_type {
        col_int8   = 0, /// Encodes as 1 byte
        col_int16  = 1, /// Encodes as 2 bytes
        col_int32  = 2, /// Encodes as 4 bytes
        col_int64  = 3, /// Encodes as 8 bytes
        col_bool   = 4, /// Encodes as 1 bit (grouped) to 1 byte (solo)
        col_float  = 5, /// Encodes as 4 bytes ( IEEE 754 )
        col_double = 6, /// Encodes as 8 bytes ( IEEE 754 )
        col_string = 7, /// Encodes as 1 byte (len) + len bytes. Max 255
        col_bytes  = 8  /// Encodes as 2 bytes (len) + len bytes. Max (65.535)
    };

    /** Type / Column flags */
    enum col_flags {
        col_unsigned = (1 << 5), /// Encode as unsigned
        col_indexed  = (1 << 6), /// Keep column indexed
        col_sparse   = (1 << 7)  /// Encode as list of types, not the types themself
    };

    /** Single table column */
    struct col {
        /** Returns type kind */
        uint8_t type() {
            // First 4 bytes are type
            return static_cast<col_type>(m_data & 15);
        }

        /** Whether type is unsigned */
        bool is_unsigned() {
            return m_data & col_unsigned;
        }

        /** Whether type is indexed */
        bool is_indexed() {
            return m_data & col_indexed;
        }

        /** Whether type is sparse */
        bool is_sparse() {
            return m_data & col_sparse;
        }

        /** Type data */
        uint8_t m_data;
        /** Name */
        char m_name[32];
        /** Commentary */
        char m_comment[128];
    };

    /** Read column from bitstream */
    col* col_read(bitstream& b);

    /** Write column from bitstream */
    void col_write(bitstream& b, col* c);
} /* deltadb */

#endif /* DELTADB_DB_TABLE_COL_HPP */
