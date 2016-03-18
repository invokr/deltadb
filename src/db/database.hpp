/**
 * @file database.hpp
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

#ifndef DELTADB_DB_DATATBASE_HPP
#define DELTADB_DB_DATATBASE_HPP

#include <vector>
#include <boost/noncopyable.hpp>

#include "../internal/filesystem.hpp"

namespace deltadb {
    // forward decl
    class table;

    class database : private boost::noncopyable {
    public:
        /** Constructor */
        database() : m_lock("db.lock") {}

        /** Destructor */
        ~database() {
            close();
        }

        /** Open the database */
        bool open();

        /** Close database */
        void close();
    private:
        /** Database lock */
        filelock m_lock;
        /** List of tables */
        std::vector<table*> m_tables;
    };
} /* deltadb */

#endif /* DELTADB_DB_DATATBASE_HPP */
