tiff_objs=
ifeq ($(shell $(CC) make/test.c -Dtest_tiff -o tmp -lm -ltiff 2>/dev/null;echo $$?),0)
	tiff_objs=pnm_src/tiff.o
	options += tiff
	images+=tiff
	TESTLIB+=-ltiff
endif

