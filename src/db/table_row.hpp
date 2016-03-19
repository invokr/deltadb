/**
 * @file table_row.hpp
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

#ifndef DELTADB_DB_TABLE_ROW_HPP
#define DELTADB_DB_TABLE_ROW_HPP

#include <vector>
#include <cstdint>

#include "../internal/bitstream.hpp"

namespace deltadb {
    // forward decl
    struct col;

    /** Single value */
    struct row_value {
        /** Value type */
        uint8_t m_type;

        /** Position */
        uint8_t m_pos;

        /** String size if applicable */
        uint16_t m_size;

        /** Actual value */
        union {
            int8_t   v_i8;
            int16_t  v_i16;
            int32_t  v_i32;
            int64_t  v_i64;
            uint8_t  v_u8;
            uint16_t v_u16;
            uint32_t v_u32;
            uint64_t v_u64;
            bool     v_bool;
            float    v_float;
            double   v_double;
            char*    v_bytes;
        } m_value;
    };

    /** Row data */
    struct row {
        /** Fields set */
        uint64_t m_fields;
        /** Data */
        std::vector<row_value> m_data;

        row() : m_fields(0) {}

        /** Check if row has given field */
        bool has(uint8_t field) {
            return m_fields & bit_at(field);
        }

        /** Returns row_value for field */
        row_value* get(uint8_t field) {
            assert(has(field));
            const uint8_t pos = __builtin_popcountll((m_fields & bits_until(field+1)));
            return &m_data[pos - 1];
        }

        /** Set field to given value, requires sort if used out of order */
        void set(uint8_t field, row_value v) {
            m_fields = bit_set(field, m_fields);
            v.m_pos = field;
            m_data.push_back(std::move(v));
        }

        /** Sort m_data */
        void sort() {
            std::sort(m_data.begin(), m_data.end(), [](const row_value& a, const row_value& b) {
                return a.m_pos > b.m_pos;
            });
        }

        /** Returns size in bytes */
        uint32_t size() {
            uint32_t ret = 8; // initial size for bitfield

            for (auto &v : m_data) {
                switch(v.m_type) {
                case col_bool:
                case col_int8:
                    ret += 1;
                    break;
                case col_int16:
                    ret += 2;
                    break;
                case col_float:
                case col_int32:
                    ret += 4;
                    break;
                case col_double:
                case col_int64:
                    ret += 8;
                    break;
                case col_string:
                    ret += strlen(v.m_value.v_bytes) + 1;
                    break;
                case col_bytes:
                    ret += 2 + v.m_size;
                    break;
                }
            }

            return ret;
        }
    };

    /** Read row from bitstream */
    row* row_read(std::vector<col*> c, bitstream& b);

    /** Write row to bitstream */
    void row_write(bitstream& b, row* r);
} /* deltadb */

#endif /* DELTADB_DB_TABLE_ROW_HPP */
