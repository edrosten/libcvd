echo=/bin/echo -e
CXX=g++
LD=g++
CC=g++

INCLUDE=
WFLAGS=-Wall

OFLAGS_full=-O3
OFLAGS_2=-O
DFLAGS_1= -g -ggdb -fno-inline
DFLAGS_0= -DNDEBUG

TESTLIB=-L /usr/X11R6/lib -lX11 -lGL -ljpeg
TESTLIB=-L /usr/X11R6/lib -lX11 -lGL -ljpeg

MISCFLAGS=-fPIC -pthread 
AR = ar crv

