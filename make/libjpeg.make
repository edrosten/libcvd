jpeg_objs=
ifeq ($(shell $(CC) make/test.c -Dtest_jpeg -o /dev/null -ljpeg 2>/dev/null;echo $$?),0)
	jpeg_objs=pnm_src/jpeg.o
	options += jpeg
	images+=jpeg
endif

