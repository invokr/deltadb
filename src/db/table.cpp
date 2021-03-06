/**
 * @file table.cpp
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

#include <string>
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <cstring>

#include "../internal/bitfield.hpp"
#include "../internal/bitstream.hpp"
#include "table_col.hpp"
#include "table_row.hpp"
#include "table.hpp"

namespace deltadb {
    table::~table() {
        for (uint32_t i = 0; i < m_types.size(); ++i) {
            delete m_types[i];
        }

        // write last block
        if (m_tainted) {
            std::string blk = m_name+".blk";
            block_write(blk.c_str(), m_block, true);
        }
    }

    void table::from_file() {
        std::string frm = m_name+".tbl";
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
        uint32_t size = b.read(8);
        m_types.resize(size);
        for (uint8_t i = 0; i < size; ++i) {
            m_types[i] = col_read(b);
        }

        // read blocks
        std::string blk = m_name+".blk";

        uint32_t blocks = block_num(blk.c_str());
        for (uint32_t i = 1; i < blocks; ++i) {
            m_cache.push_back(block_read(blk.c_str(), i));
        }

        m_tainted = true;
        if (blocks != 0) {
            m_block = block_read(blk.c_str(), blocks);
        } else {
            m_block = new block();
        }
    }

    void table::create() {
        std::string frm = m_name+".tbl";

        bitstream b(m_name.size() + 2 + m_types.size() * 161);
        b.write_bytes(&m_name[0], m_name.size());
        b.write(8, 0);
        b.write(8, m_types.size());

        for (int i = 0; i < m_types.size(); ++i) {
            col_write(b, m_types[i]);
        }

        FILE* fp = fopen(frm.c_str(), "wb");
        if (!fp) {
            perror("Unable to open table");
            return;
        }

        fwrite(b.buffer(), 1, b.width(), fp);
        fclose(fp);

        // create an empty block file
        std::string blk = m_name+".blk";
        FILE* fp2 = fopen(blk.c_str(), "wb");
        fclose(fp2);

        // set active block
        m_block = new block();
        m_tainted = false;
    }

    void table::write(row *r) {
        // @todo: this code is retarded

        auto rem = r->size() + m_block->pos;
        if (rem > BLOCK_DSIZE) {
            // @todo compute crc
            std::string blk = m_name+".blk";

            if (m_tainted) {
                block_write(blk.c_str(), m_block, true);
            } else {
                block_write(blk.c_str(), m_block);
            }

            m_cache.push_back(m_block);
            m_block = new block();
            m_tainted = false;
        }

        bitstream b(
            (bitstream::word_t*)(m_block->data + m_block->pos),
            BLOCK_DSIZE - rem, bitstream::mode::io_writer
        );

        row_write(b, r);
        m_block->pos += r->size();
    }
}
