has_i686=$(shell uname -m | grep -q i686 && echo 1)

yuv411_objs=cvd_src/yuv411_to_stuff.o

ifeq ($(has_i686), 1)
	yuv411_objs=cvd_src/i686/yuv411_to_stuff_MMX.o
endif

