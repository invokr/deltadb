/**
 * @file console.hpp
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

#ifndef DELTADB_CONSOLE_CONSOLE_HPP
#define DELTADB_CONSOLE_CONSOLE_HPP

#include <vector>

namespace deltadb {
    // forward decls
    struct row;
    struct col;

    /** Print table layout */
    void print_frm(std::vector<col*>& cols);

    /** Print given rows */
    void print_rows(std::vector<col*>& cols, std::vector<row*>& rows);
} /* deltadb */

#endif /* DELTADB_CONSOLE_CONSOLE_HPP */
