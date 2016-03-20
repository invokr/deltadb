/**
 * @file block.hpp
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

#ifndef DELTADB_DB_BLOCK_HPP
#define DELTADB_DB_BLOCK_HPP

/** Block datasize */
#define BLOCK_DSIZE 131064

namespace deltadb {
    /** 128kb data blocks */
    struct block {
        /** Block crc */
        uint32_t crc;
        /** Last written pos */
        uint32_t pos;
        /** Block data */
        char data[BLOCK_DSIZE];

        block() : crc(0), pos(0) {}
    };

    /** Load block from data file */
    block* block_read(const char* db, uint32_t num);

    /** Write block to data file, optionally overwriting the last block */
    void block_write(const char* db, block* b, bool overwrite = false);

    /** Return number of blocks in file */
    uint32_t block_num(const char* db);
} /* deltadb */

 #endif /* DELTADB_DB_BLOCK_HPP */
