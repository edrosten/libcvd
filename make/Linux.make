echo=/bin/echo -e

CXX=g++-3
LD=g++-3
CC=gcc-3


KERNEL_MAJOR=$(shell uname -r | awk -F'[.-]' '{print $$1}')
KERNEL_MINOR=$(shell uname -r | awk -F'[.-]' '{print $$2}')
KERNEL_RELEASE=$(shell uname -r | awk -F'[.-]' '{print $$3}')

config_Linux+=Kernel version flags:
config_Linux+=KERNEL_MAJOR=$(KERNEL_MAJOR) KERNEL_MINOR=$(KERNEL_MINOR) KERNEL_RELEASE=$(KERNEL_RELEASE)


INCLUDE=
WFLAGS=-Wall

OFLAGS_full=-O3 -march=pentium3 -fexpensive-optimizations
OFLAGS_2=-O
DFLAGS_1= -g -ggdb -fno-inline
DFLAGS_0= -DNDEBUG
PFLAGS_0=
PFLAGS_1=-p -pg 

TESTLIB=

MISCFLAGS=-fPIC -pthread -DKERNEL_MAJOR=$(KERNEL_MAJOR) -DKERNEL_MINOR=$(KERNEL_MINOR) -DKERNEL_RELEASE=$(KERNEL_RELEASE)

AR = ar crv

TEST_Linux=
OBJS_Linux=

#Test for dvbuffer
ifeq ($(shell $(CC) make/test.c -Dtest_dv -o /dev/null -lraw1394 -ldc1394_control;echo $$?),0)
	options+=dvbuffer
	OBJS_Linux+=cvd_src/Linux/dvbuffer.o 
	TESTLIB+=-lraw1394 -ldc1394_control

	TEST_Linux+=test/dvbuffer_mono.test test/dvbuffer_rgb.test test/dvbuffer_controls.test 
	TEST_Linux+=test/diskbuffer2.test test/rawdcvideo_yuv_to_rgb.test 
	TEST_Linux+=test/rawdcvideo_yuv_to_y.test test/dvbuffer_yuvrgb.test test/v4l2buffer.test
endif


#Test for v4l2buffer
ifeq ($(shell $(CC) make/test.c -Dtest_v4l2 -o /dev/null -lraw1394 -ldc1394_control;echo $$?),0)
	options+=v4l2buffer
	OBJS_Linux+=cvd_src/Linux/v4l2buffer.o 
	TEST_Linux+=test/v4l2buffer.test 
endif


options_Linux=dvbuffer v4l2buffer
