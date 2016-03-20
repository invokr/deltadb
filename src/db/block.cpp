/**
 * @file block.cpp
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

#include <sys/stat.h>

#include <cassert>
#include <cstdio>
#include <cstdint>

#include "block.hpp"

namespace deltadb {
    block* block_read(const char* db, uint32_t num) {
        assert(num != 0);
        block* ret = new block();

        FILE* fp = fopen(db, "rb");
        if (!fp)
            return nullptr; // unkown db?

        fseek(fp, sizeof(block) * (num-1), SEEK_SET);
        assert(1 == fread(ret, sizeof(block), 1, fp));

        fclose(fp);
        return ret;
    }

    void block_write(const char* db, block* b, bool overwrite) {
        assert(b);

        FILE* fp = fopen(db, "rb+");
        assert(fp);

        fseek(fp, overwrite ? -sizeof(block) : 0, SEEK_END);
        assert(1 == fwrite(b, sizeof(block), 1, fp));
        fclose(fp);
    }

    uint32_t block_num(const char* db) {
        struct stat st;
        stat(db, &st);
        return st.st_size / sizeof(block);
    }
 } /* deltadb */
