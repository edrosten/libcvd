import os, glob
from os.path import join
import sys

env = Environment(ENV = os.environ)
env.Append(CPPPATH=['.', 'cvd', '/opt/local/include', '/usr/X11R6/include'])
env.Append(LIBPATH=['.', '/opt/local/lib', '/usr/X11R6/lib'])
env.Append(CCFLAGS=['-ggdb','-fPIC', '-mmmx', '-msse', '-msse', '-msse2', '-msse3'])
env.Append(LIBS=['GL', 'X11'])

#env.Append(FRAMEWORKS=['OpenGL', 'Accelerate'])
#
#env.Append(DEFINES=['_GLIBCXX_DEBUG', 'CVD_IMAGE_DEBUG'])
#env.Append(LIBS=['avcodec'])

# core cvd source code
cvd_srcs  = ['cvd_src/diskbuffer2.cc', 'cvd_src/deinterlacebuffer.cc', 'cvd_src/exceptions.cc', 'cvd_src/image_io.cc', 'cvd_src/bayer.cxx', 'cvd_src/colourspace_convert.cxx', 'cvd_src/half_sample.cc', 'cvd_src/draw.cc', 'cvd_src/yuv422.cpp', 'cvd_src/yuv420.cpp', 'pnm_src/pnm_grok.cxx', 'pnm_src/bmp.cxx', 'pnm_src/fits.cc', 'pnm_src/fitswrite.cc', 'pnm_src/save_postscript.cxx', 'pnm_src/text_write.cc', 'pnm_src/text.cxx', 'cvd_src/fast_corner.cxx', 'cvd_src/convolution.cc', 'cvd_src/nonmax_suppression.cxx', 'cvd_src/timeddiskbuffer.cc', 'cvd_src/videosource.cpp', 'cvd_src/connected_components.cc']
cvd_srcs += ['pnm_src/cvdimage.cxx']

def CheckPThreads(context):
    context.Message( 'Checking for pthreads ...' )
    result = context.TryLink("""
                          #include <pthread.h>
                          void* foo(void*) {}
                          int main(int argc, char** argv){
                             pthread_create(0,0,foo,0);
                             return 0;
                          }""",'.cc')
    if result:
        context.env.Append(CXXFLAGS='-pthread')
    context.Result( result )
    return result

def CheckSharedPtr(context):
    context.Message( 'Checking for tr1_shared_ptr...' )
    result = context.TryLink("""
                         #include <tr1/memory>
	                 int main(){
                              std::tr1::shared_ptr<int> i; 
                              return  0;
                         }""",'.cc')
    context.Result( result )
    return result

if sys.platform == 'darwin':
    env.Append(CPPDEFINES=['DARWIN'])
elif sys.platform.startswith('linux'):
    env.Append(CPPDEFINES=['LINUX'])

conf = Configure(env, custom_tests = { 'CheckPThreads':CheckPThreads, 'CheckSharedPtr':CheckSharedPtr })
# test for pthreads
if conf.CheckPThreads():
    print "We have pthreads"   
    cvd_srcs += ['cvd_src/synchronized.cpp', 'cvd_src/eventobject.cpp', 'cvd_src/thread.cpp']
    env.Append(CPPDEFINES=['CVD_HAVE_PTHREAD'])
else:
    print "We don't have pthreads"

if conf.CheckPThreads() and conf.CheckSharedPtr():
    cvd_srcs += ['cvd_src/thread/runnable_batch.cc']

if not conf.CheckSharedPtr():
    cvd_srcs += ['cvd_src/nothread/runnable_batch.cc']

# test for support for dc1394v2
if conf.CheckHeader('dc1394/dc1394.h') and conf.CheckLib('dc1394'):
    print "got dc1394"
    cvd_srcs += ['cvd_src/Linux/dvbuffer3_dc1394v2.cc', 'cvd_src/videosource_dvbuffer.cc']
    env.Append(CPPDEFINES=['CVD_HAVE_DVBUFFER3'])
else:
    print "not got dc1394"
    cvd_srcs += ['cvd_src/videosource_nodvbuffer.cc']

# take care of Quicktime
cvd_srcs += ['cvd_src/OSX/videosource_noqtbuffer.cc']
if False:
    if True:
        cvd_srcs += ['cvd_src/OSX/videosource_qtbuffer.cc', 'cvd_src/OSX/qtbuffer.cpp']
        env.Append(FRAMEWORKS=['QuickTime', 'Carbon'])
    else:
        cvd_srcs += ['cvd_src/OSX/videosource_noqtbuffer.cc']

# take care of other sources
cvd_srcs += ['cvd_src/videosource_novideofilebuffer.cc', 'cvd_src/Linux/videosource_nov4lbuffer.cc', 'cvd_src/Linux/videosource_nov4l1buffer.cc']

# add support for gl
cvd_srcs += ['cvd_src/glwindow.cc', 'cvd_src/gltext.cpp']

conf.Finish()
# test for SSE2 support
if False:
    print "building in support for SSE2"
    cvd_srcs += ['cvd_src/i686/yuv422_wrapper.cc']
else:
    cvd_srcs += ['cvd_src/yuv411_to_stuff.cxx', 'cvd_src/noarch/yuv422_wrapper.cc', 'cvd_src/noarch/default_memalign.cpp']

# add timer functionality
cvd_srcs += ['cvd_src/cvd_timer.cc']

# add support for globbing
env.Append(CPPDEFINES=['CVD_HAVE_GLOB'])
cvd_srcs += ['cvd_src/globlist.cxx']

# Image conversion
cvd_srcs += ['cvd_src/noarch/convert_rgb_to_y.cc']

env.SharedLibrary(target='libcvd', source=cvd_srcs)
#env.Command('cvd/config.h', None, [Touch('$TARGET')])

progs_env = env.Clone()
progs_env.Append(LIBS=['libcvd'])
progs_env.Program(source='progs/video_play_source.cc')#, LIBS='libcvd')

qtkit_env = progs_env.Clone()
qtkit_env.Append(FRAMEWORKS=['Foundation', 'QTKit', 'Cocoa', 'Quartz'])
qtkit_env.Program(target='progs/testvideo', source=['progs/video_capture.m', 'progs/testvideo.cpp'])

qtice_env = qtkit_env .Clone()
qtice_env.Append(LIBS=['ZeroCIce', 'Ice', 'IceUtil', 'IceSSL'])
qtice_env.Program(target='progs/qtkit_ice_source', source=['progs/qtkit_ice_source.cxx', 'progs/video_capture.m'])



#progs_env.Program(source='test/qtkit_test.cc', FRAMEWORKS=['QTKit'])
