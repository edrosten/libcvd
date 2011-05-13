#
#
#
if [ "$1" == "" ]
then
	echo Please specify configuration name
    echo Please use a configuration like this:
	echo ./configure --without-lapack --without-png --without-tiff  --without-dc1394v1 --without-dc1394v2 --without-assembler --without-inline_asm --host x86_64-windows
	exit
fi

echo ... $*

n="$1"
shift

if [ "$#" == 0 ]
then
	echo Please specify the libraries that visual studio will use
	exit
fi

make clean

sed -e"s!XXXLIBSXXX!$*!"  make/libcvd.vcproj.header   \
                                         > "build/vc2008/libcvd-$n.vcproj"
bash make/make_vcproj_source.bash       >> "build/vc2008/libcvd-$n.vcproj"
bash make/make_vcproj_headers.bash "$n" >> "build/vc2008/libcvd-$n.vcproj"
cat make/libcvd.vcproj.footer           >> "build/vc2008/libcvd-$n.vcproj"

cp cvd/config.h                            "build/vc2008/config-$n.h"
