/**
 * @file table.hpp
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

#ifndef DELTADB_DB_TABLE_HPP
#define DELTADB_DB_TABLE_HPP

#include <string>
#include <cassert>

#include "../internal/filesystem.hpp"
#include "types.hpp"

namespace deltadb {
    class table {
    public:
        /** Constructor */
        table(std::string name) : m_name(name), m_types(nullptr), m_size(0) {
            assert(name.size() <= 32);
            auto frm = m_name+".tbl";

            if (file_exists(frm.c_str())) {
                from_file();
            } else {
                create();
            }
        }

        /** Destructor */
        ~table() {
            for (uint32_t i = 0; i < m_size; ++i) {
                delete m_types[i];
            }
        }

        /** Whether table is open */
        bool is_open() {
            return m_types != nullptr;
        }

        /** Set columns for newly created table */
        void set_columns(col** cols, uint8_t size) {
            if (m_types == nullptr) {
                m_types = cols;
                m_size = size;
                create();
            }
        }
    private:
        /** Table name */
        std::string m_name;
        /** Array of column types */
        col** m_types;
        /** Number of columns */
        uint8_t m_size;

        /** Read column data from file */
        void from_file();

        /** Create a table */
        void create();
    };
} /* deltadb */

 #endif /* DELTADB_DB_TABLE_HPP */
