./configure --host x86_64-windows --without-lapack --without-png --without-tiff --without-dc1394v1 --without-dc1394v2 --without-inline_asm --without-assembler
bash make/make_vcproj.bash TooN-JPEG-pthread jpeg.lib pthreadVCE2.lib

./configure --host x86_64-windows --without-lapack --without-png --without-tiff --without-dc1394v1 --without-dc1394v2 --without-inline_asm --without-assembler --without-jpeg 
bash make/make_vcproj.bash TooN  ""

