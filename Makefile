UNAME=$(shell uname)

################################################################################
#
# External library locations
#

NUMERICS=$(HOME)/code/TooN
X11_I=/usr/X11R6/include
X11_L=/usr/X11R6/lib



FORCE_GCC=0

PREFIX=$(HOME)/usr/noarch
EXEC_PREFIX=$(HOME)/usr/arch/$(UNAME)



#Options: 0 | full
OPTIMIZE=full
#Options: 0 | 1 
DEBUG=0
#Profile
PROFILE=0

MAJOR_VER=0
MINOR_VER=6



################################################################################
#
# Configure CPU architecture specific stuff
#

include make/arch.make



################################################################################
#
# Configure OS specific stuff
#

ifeq ($(UNAME),Linux)
	include make/Linux.make
else
ifeq ($(UNAME),IRIX)
	include make/IRIX.make
else
	include make/generic.make
endif
endif

################################################################################
#
# Configure library specific stuff
#
include make/x11.make

#Images
images=

include make/libjpeg.make


#List of all possible options
options_libs=videodisplay jpeg


OFLAGS=$(OFLAGS_$(OPTIMIZE))
DFLAGS=$(DFLAGS_$(DEBUG))
PFLAGS=$(PFLAGS_$(PROFILE))

CXX_FLAGS=$(CXXFLAGS) -I. -I $(NUMERICS) -I$(X11_I) $(INCLUDE)  $(DFLAGS) $(OFLAGS) $(WFLAGS) $(MISCFLAGS) $(PFLAGS)
CXX_FLAGS_no_opt=$(CXXFLAGS) -I. -I $(NUMERICS) -I$(X11_I)  $(INCLUDE)  $(DFLAGS) $(WFLAGS) $(MISCFLAGS) $(PFLAGS)


################################################################################
#
# Object files


CVD_OBJS=	cvd_src/se3.o 								\
			cvd_src/so3.o 								\
			cvd_src/cvd_timer.o 						\
			cvd_src/brezenham.o							\
			cvd_src/globlist.o							\
			cvd_src/diskbuffer2.o						\
			cvd_src/deinterlacebuffer.o					\
			cvd_src/exceptions.o						\
			cvd_src/image_io.o							\
 			pnm_src/instantiations.o					\
			pnm_src/pnm_grok.o							\

#Optional library support
OBJS_OPT_LIBS=$(jpeg_objs) $(x11_objs)

#Arch specific object files
OBJS_arch=$(yuv411_objs)

#Aggregte all object file
OBJS=$(CVD_OBJS) $(OBJS_$(UNAME)) $(OBJS_arch) $(OBJS_OPT_LIBS)

################################################################################
#
# Programs to be installed
#
PROGS=progs/se3_exp progs/se3_ln  progs/se3_pre_mul progs/se3_post_mul progs/img_play progs/img_play_bw progs/img_stream_play progs/se3_inv progs/img_play_deinterlace

################################################################################
#
# Required headers

HEADERS=cvd/arch.h cvd/version.h 


################################################################################
#
# Test programs
TEST_all= test/test_images.test 
TEST= $(TEST_all) $(TEST_$(UNAME))


LIBMAJ=libcvd.so.$(MAJOR_VER)
LIBMIN=$(LIBMAJ).$(MINOR_VER)


libcvd.a: configuration $(OBJS)
	$(AR) libcvd.a $(OBJS)
	ranlib libcvd.a

libcvd.so: libcvd.a
	echo ********** ERROR! **********************
	echo *ERROR* this breaks exceptions!!!!!!!!
	foofoofoof
	ld -shared -soname $@ -o $(LIBMIN) -lc $(OBJS)
	#strip $(LIBMIN)
	rm -f $(LIBMAJ)
	ln -s $(LIBMIN) $(LIBMAJ)
	rm -f libcvd.so
	ln -s $(LIBMIN) libcvd.so
	echo ********** ERROR! **********************


testprogs:$(TEST)
test:testprogs
	mkdir -p testout/foo
	echo "**** TESTING image loading and saving:"
	test/test_images.test test/images/*.p?m test/images/*.jpg # > test_images.out.tmp
	echo "**** Converting to 8 bit (so xv will work for us...)"
	ls testout/*.pnm | sed -e's!testout/\(.*\)!pnmdepth 255 "&" > "testout/foo/\1"!' | sh
	echo "**** Check if images are OK:"
	echo "**** Warning: some versions of pnmdepth break on 16 bit files!"
	mv testout/*.jpg testout/foo/
	xv testout/*

	echo "**** TESTING video i/o: press a key to exit"
	#test/videoprog.test

	echo "**** All OK"


install: libcvd.a $(PROGS)
	cp -r cvd $(PREFIX)/include/
	cp libcvd.a $(EXEC_PREFIX)/lib/
	cp $(PROGS) $(EXEC_PREFIX)/bin/

cvd/arch.h: util/make_pnm_arch_h.util
	util/make_pnm_arch_h.util > cvd/arch.h

cvd/version.h:
	echo "#ifndef __CVD_VERSION_H" > cvd/version.h
	echo "#define __CVD_VERSION_H" >> cvd/version.h
	echo "  #define CVD_MAJOR_VERSION $(MAJOR_VER)" >> cvd/version.h
	echo "  #define CVD_MINOR_VERSION $(MINOR_VER)" >> cvd/version.h
	echo "#endif" >>cvd/version.h

cvd/internal/avaliable_images.hh:
	echo $(images) | awk -vRS='[[:space:]]' '$0{a=toupper($$1);print "#define CVD_IMAGE_HAS_"a" "a","}' > cvd/internal/avaliable_images.hh


configuration: cvd/arch.h cvd/internal/avaliable_images.hh
	$(echo) "CVD version $(MAJOR_VER).$(MINOR_VER)\n" > configuration
	$(echo) "Platform: $(UNAME)" >> configuration
	$(echo) "Compiler: $(CXX)"   >> configuration
	$(echo) "Debug level: $(DEBUG)   (DFLAGS=$(DFLAGS))" >> configuration
	$(echo) "Optimize: $(OPTIMIZE) (OFLAGS=$(OFLAGS))" >> configuration
	$(echo) '$(config_$(UNAME))' >> configuration
	$(echo) -n "Arch specific stuff: " >> configuration
	awk -vORS=" " '!/PNM/{print $$2}' cvd/arch.h 			>> configuration
	$(echo) "has_i686=$(has_i686)\n" >> configuration
	$(echo) "Options: $(options)\n" >> configuration
	$(echo) -n "Missing options for $(UNAME): " >> configuration
	echo $(options_libs) $(options_$(UNAME)) $(options) | awk 'BEGIN{RS=" "}{a[$$1]++}END{for(i in a)if(a[i]==1)printf(i" "); print""}' >> configuration

progs: libcvd.a $(PROGS)

progs/se3_exp: libcvd.a progs/se3_exp.o
	$(CXX) $^ -o $@ $(OFLAGS) -L. -lcvd 

progs/se3_ln: libcvd.a progs/se3_ln.o
	$(CXX) $^ -o $@ $(OFLAGS) -L. -lcvd 

progs/se3_inv: libcvd.a progs/se3_inv.o
	$(CXX) $^ -o $@ $(OFLAGS) -L. -lcvd 

progs/se3_pre_mul: libcvd.a progs/se3_pre_mul.o
	$(CXX) $^ -o $@ $(OFLAGS) -L. -lcvd 

progs/se3_post_mul: libcvd.a progs/se3_post_mul.o
	$(CXX) $^ -o $@ $(OFLAGS) -L. -lcvd

progs/img_stream_play: libcvd.a progs/img_stream_play.o
	$(CXX) $^ -o $@ $(OFLAGS) -L. -lcvd  $(TESTLIB)

progs/img_play: libcvd.a progs/img_play.o
	$(CXX) $^ -o $@ $(OFLAGS) -L. -lcvd  $(TESTLIB)

progs/img_play_deinterlace: libcvd.a progs/img_play_deinterlace.o
	$(CXX) $^ -o $@ $(OFLAGS) -L. -lcvd  $(TESTLIB)
	
progs/img_play_bw: libcvd.a  progs/img_play_bw.o
	$(CXX) $^ -o $@ $(OFLAGS) -L. -lcvd $(TESTLIB)

clean: 
	rm -f libcvd.a
	rm -f libcvd.s*
	find . -name '*.o' | xargs rm -f
	find . -name '*.test' | xargs rm -f
	find . -name '*.util' | xargs rm -f
	rm -f cvd/arch.h cvd/version.h cvd/internal/avaliable_images.hh
	find . -name 'ii_files' | xargs rm -rf
	rm -rf testout tmp *.tmp
	rm -f configuration
	rm -f cvd/version.h




################################################################################
#
# Generic rules
.SUFFIXES:
.SUFFIXES: .cc .C .o .cxx .test .util


.cxx.util:
	$(CXX) -o  $@ -L.  $(CXX_FLAGS_no_opt) $< $(UTILLIB)

%.test: %.C libcvd.a
	$(CXX) -o $@ -L.  $(CXX_FLAGS_no_opt)  $< -lcvd $(TESTLIB) -lm

%.test: %.cxx libcvd.a
	$(CXX) -o $@ -L.  $(CXX_FLAGS) $< -lcvd $(TESTLIB) -lm

%.o :  %.cc $(HEADERS)
	$(CXX) -o $@ -c $(CXX_FLAGS) $< 

%.o :  %.cxx $(HEADERS)
	$(CXX) -o $@ -c $(CXX_FLAGS) $<  

#These have asm in them. Do not optimize.
%.o :  %.C $(HEADERS)
	$(CXX) -o $@ -c $(CXX_FLAGS_no_opt) $< 

