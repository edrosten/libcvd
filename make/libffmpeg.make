ffmpeg_objs=
ifeq ($(shell $(CC) make/test.c -Dtest_ffmpeg -o tmp -L$(OTHER_L) -I$(OTHER_I) -lavformat -lavcodec -lm -lz 2>/dev/null;echo $$?),0)
	ffmpeg_objs=cvd_src/videofilebuffer.o
	options+=ffmpeg
	TESTLIB+=-L$(OTHER_L) -lavformat -lavcodec -lm -lz
endif
