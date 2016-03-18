/**
 * @file table_col.cpp
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

#include <cstring>

#include "../internal/bitstream.hpp"
#include "table_col.hpp"

namespace deltadb {
    col* col_read(bitstream& b) {
        col* ret = new col();

        // read type
        ret->m_data = b.read(8);

        // read name
        b.read_string(31, ret->m_name);

        // read comment if applicable
        if (b.read_bool()) {
            b.read_string(127, ret->m_comment);
        } else {
            ret->m_comment[0] = '\0';
        }

        return ret;
    }

    void col_write(bitstream& b, col* c) {
        // write type
        b.write(8, c->m_data);

        // write name
        b.write_bytes(c->m_name, strlen(c->m_name)+1);

        // write comment if applicable
        if (c->m_comment[0] != '\0') {
            b.write(1, 1);
            b.write_bytes(c->m_comment, strlen(c->m_comment)+1);
        } else {
            b.write(0, 0);
        }
    }
}
