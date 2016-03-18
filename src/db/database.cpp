/**
 * @file database.cpp
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
#include <vector>
#include <cstring>
#include <cstdio>

#include <dirent.h>
#include <unistd.h>

#include "../config.hpp"
#include "table.hpp"
#include "types.hpp"
#include "database.hpp"

namespace deltadb {
    bool database::open() {
        // Set cwd to data directory
        if (chdir(DELTADB_PATH_DATA) != 0) {
            perror("Unable to set cwd");
            return false;
        }

        // Aquire db lock
        if (!m_lock.aquire()) {
            perror("Unable to aquire database lock");
            return false;
        }

        // Read a list of tables
        DIR *dp;
        struct dirent *file;

        if((dp = opendir("./")) == NULL) {
            perror("Unable to list directory");
            return false;
        } else {
            while((file=readdir(dp)) != NULL) {
                if (strcmp(file->d_name+(strlen(file->d_name)-3), "tbl") == 0)
                    m_tables.push_back(new table(std::string(file->d_name, strlen(file->d_name)-4)));
            }

            closedir(dp);
        }

        return true;
    }

    void database::close() {
        for (auto tbl : m_tables) {
            delete tbl;
        }

        m_lock.release();
    }

    void database::create(const char* name, col** t, uint32_t len) {
        std::string frm = std::string(name)+".tbl";
        if (file_exists(frm.c_str())) {
            return; // @todo: error
        }

        table* t2 = new table(std::string(name));
        t2->set_columns(t, len);
        m_tables.push_back(t2);
    }
}
