#------------------------------------------------------------
# Standard CMake Stuff
#------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED ( VERSION 2.8 )
PROJECT ( deltadb )

#------------------------------------------------------------
# Compiler Setup
#------------------------------------------------------------

SET ( CMAKE_CXX_FLAGS "-O3 -g --std=c++14 -Wall" )

#------------------------------------------------------------
# Include and configure all libraries
#------------------------------------------------------------

INCLUDE_DIRECTORIES (
    /usr/include
    /usr/local/include
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
    ${CMAKE_SOURCE_DIR}/src/server.cpp
)
