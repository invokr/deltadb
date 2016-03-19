/**
 * @file table_row.cpp
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

#include <cstdint>
#include <iostream>

#include "../internal/bitstream.hpp"
#include "table_col.hpp"
#include "table_row.hpp"

namespace deltadb {
    row* row_read(std::vector<col*> c, bitstream& b) {
        row* ret = new row();
        ret->m_fields = ((uint64_t)b.read(32) << 32) | b.read(32);
        uint64_t fields = ret->m_fields;

        for (uint8_t i = 0; i < c.size(); ++i) {
            if (!(fields & bit_at(i)))
                continue;

            row_value v;
            v.m_size = 0;
            v.m_type = c[i]->type();

            switch (v.m_type) {
            case col_int8:
                v.m_value.v_u8 = b.read(8);
                break;
            case col_int16:
                v.m_value.v_u16 = b.read(16);
                break;
            case col_int32:
            case col_float:
                v.m_value.v_u32 = b.read(32);
                break;
            case col_int64:
            case col_double:
                v.m_value.v_u64 = ((uint64_t)b.read(32) << 32) | b.read(32);
                break;
            case col_bool:
                v.m_value.v_bool = b.read(8);
                break;
            case col_string:
                v.m_value.v_bytes = new char[256];
                b.read_string(255, v.m_value.v_bytes);
                break;
            case col_bytes:
                v.m_size = b.read(16);
                v.m_value.v_bytes = new char[v.m_size];
                b.read_bytes(v.m_size, v.m_value.v_bytes);
                break;
            }

            ret->m_data.push_back(v);
        }

        return ret;
    }

    void row_write(bitstream& b, row* r) {
        b.write(32, (uint32_t)(r->m_fields >> 32));
        b.write(32, (uint32_t)(r->m_fields));

        for (auto &v : r->m_data) {
            switch (v.m_type) {
            case col_int8:
                b.write(8, v.m_value.v_u8);
                break;
            case col_int16:
                b.write(16, v.m_value.v_u16);
                break;
            case col_int32:
            case col_float:
                b.write(32, v.m_value.v_u32);
                break;
            case col_int64:
            case col_double:
                b.write(32, (uint32_t)(v.m_value.v_u64 >> 32));
                b.write(32, (uint32_t)(v.m_value.v_u64));
                break;
            case col_bool:
                b.write(8, v.m_value.v_bool);
                break;
            case col_string:
                b.write_bytes(v.m_value.v_bytes, strlen(v.m_value.v_bytes)+1);
                break;
            case col_bytes:
                b.write(16, v.m_size);
                b.write_bytes(v.m_value.v_bytes, v.m_size);
                break;
            }
        }
    }
} /* deltadb */
