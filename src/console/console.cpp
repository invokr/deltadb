/**
 * @file console.cpp
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

#include "ascii.hpp"
#include "../db/table_col.hpp"
#include "../db/table_row.hpp"
namespace deltadb {
    /** Print table layout */
    void print_frm(std::vector<col*>& cols) {
        auto t2s = [](uint8_t t) {
            switch (t) {
            case 0:
                return "int8";
            case 1:
                return "int16";
            case 2:
                return "int32";
            case 3:
                return "int64";
            case 4:
                return "bool";
            case 5:
                return "float";
            case 6:
                return "double";
            case 7:
                return "string";
            case 8:
                return "bytes";
            default:
                return "Unkown";
            }
        };

        ascii_table a;

        a.append("Name", "Type", "Comment");

        for (auto c : cols) {
            a.append(std::string(c->m_name), t2s(c->type()), std::string(c->m_comment));
        }

        a.print({true, true, true});
    }

    /** Print given rows */
    void print_rows(std::vector<col*>& cols, std::vector<row*>& rows) {

    }
} /* deltadb */
