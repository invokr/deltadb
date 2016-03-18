/**
 * @file filesystem.hpp
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

#ifndef DELTADB_INTERNAL_FILESYSTEM_HPP
#define DELTADB_INTERNAL_FILESYSTEM_HPP

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>

#include <boost/noncopyable.hpp>

namespace deltadb {
    /** Returns true if file exists and is not a directory */
    inline bool file_exists(const char* path) {
        struct stat sb;

        if (stat(path, &sb) == -1) {
            return false;
        }

        return sb.st_mode & S_IFREG;
    }

    /** Lockfile */
    struct filelock : private boost::noncopyable {
        /** Constructor, takes lockfile */
        filelock(const char* file) : m_file(file), m_fd(-1) {}

        /** Destructor, releases lock if aquired */
        ~filelock() {
            release();
        }

        /** Aquire file lock */
        bool aquire() {
            mode_t m = umask(0);
            m_fd = open(m_file, O_RDWR|O_CREAT, 0666);
            umask(m);

            if( m_fd >= 0 && flock(m_fd, LOCK_EX|LOCK_NB) < 0 ) {
                close( m_fd );
                m_fd = -1;
                return false;
            }

            return true;
        }

        /** Release file lock */
        void release() {
            if( m_fd < 0 )
                return;

            remove(m_file);
            close(m_fd);
        }

        /** File to lock */
        const char* m_file;
        /** File descriptor */
        int m_fd;
    };
}

#endif /* DELTADB_INTERNAL_FILESYSTEM_HPP */
