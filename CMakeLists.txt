#------------------------------------------------------------
# Standard CMake Stuff
#------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED ( VERSION 2.8 )
PROJECT ( deltadb )

OPTION( CONFDIR "Directory which includes configuration files" "/etc/deltadb/" )
OPTION( DATADIR "Path to database files" "/var/db/deltadb/" )

#------------------------------------------------------------
# Compiler Setup
#------------------------------------------------------------

SET ( CMAKE_CXX_FLAGS "-O3 -g --std=c++14 -Wall" )

CONFIGURE_FILE (
    ${CMAKE_SOURCE_DIR}/src/config.hpp.in
    ${CMAKE_SOURCE_DIR}/src/config.hpp
)

#------------------------------------------------------------
# Include and configure all libraries
#------------------------------------------------------------

FIND_PACKAGE(Boost REQUIRED filesystem program_options system)

INCLUDE_DIRECTORIES (
    /usr/include
    /usr/local/include
    ${CMAKE_SOURCE_DIR}/vendor/variant/include
)

LINK_DIRECTORIES (
    /usr/lib
    /usr/lib64
    /usr/local/lib
    /usr/local/lib64
)

#------------------------------------------------------------
# Build server
#------------------------------------------------------------

ADD_EXECUTABLE ( deltadbd
    ${CMAKE_SOURCE_DIR}/src/console/console.cpp
    ${CMAKE_SOURCE_DIR}/src/db/block.cpp
    ${CMAKE_SOURCE_DIR}/src/db/database.cpp
    ${CMAKE_SOURCE_DIR}/src/db/table.cpp
    ${CMAKE_SOURCE_DIR}/src/db/table_col.cpp
    ${CMAKE_SOURCE_DIR}/src/db/table_row.cpp
    ${CMAKE_SOURCE_DIR}/src/server.cpp
)

TARGET_LINK_LIBRARIES( deltadbd
    ${Boost_LIBRARIES}
)
