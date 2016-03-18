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
#include <iostream>
#include <cassert>

#include "../internal/bitfield.hpp"
#include "../internal/bitstream.hpp"
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
        void from_file() {
            auto frm = m_name+".tbl";
            char l_name[32] = {'\0'};

            // read frm data
            FILE* fp = fopen(frm.c_str(), "rb");
            if (!fp) {
                perror("Unable to open table");
                return;
            }

            const size_t fstart = ftell(fp);
            fseek(fp, 0, SEEK_END);
            const size_t fend = ftell(fp) - fstart;
            fseek(fp, 0, SEEK_SET);

            char* frm_data = new char[fend+3];
            fread(frm_data, 1, fend, fp);
            fclose(fp);

            bitstream b((bitstream::word_t*)frm_data, fend);

            // verify table name
            b.read_string(32, l_name);
            if (strcmp(l_name, m_name.c_str()) != 0) {
                std::cerr << "Error verifying table name for " << l_name << std::endl;
                return;
            }

            // read fields
            m_size = b.read(8);
            m_types = (col**)malloc(m_size*sizeof(col*));
            for (uint8_t i = 0; i < m_size; ++i) {
                m_types[i] = new col();
                m_types[i]->m_data = b.read(8);

                b.read_string(31, &m_types[i]->m_name[0]);

                if (b.read_bool()) {
                    b.read_string(127, &m_types[i]->m_comment[0]);
                } else {
                    m_types[i]->m_comment[0] = '\0';
                }
            }
        }

        /** Create a table */
        void create() {
            auto frm = m_name+".tbl";

            bitstream b(m_name.size() + 2 + m_size * 161);
            b.write_bytes(&m_name[0], m_name.size());
            b.write(8, 0);
            b.write(8, m_size);

            for (int i = 0; i < m_size; ++i) {
                b.write(8, m_types[i]->m_data);
                b.write_bytes(m_types[i]->m_name, strlen(m_types[i]->m_name)+1);
                if (m_types[i]->m_comment[0] != '\0') {
                    b.write(1, 1);
                    b.write_bytes(m_types[i]->m_comment, strlen(m_types[i]->m_comment)+1);
                } else {
                    b.write(0, 0);
                }
            }

            FILE* fp = fopen(frm.c_str(), "wb");
            if (!fp) {
                perror("Unable to open table");
                return;
            }

            fwrite(b.buffer(), 1, b.width(), fp);
            fclose(fp);
        }
    };
} /* deltadb */

 #endif /* DELTADB_DB_TABLE_HPP */
