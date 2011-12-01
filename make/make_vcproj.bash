#
#
#

if [ "$#" != "3" ]
then
	echo Please specify configuration name, a list of debug libs and a list of release libs
    echo Please use a configuration like this:
	echo ./configure --without-lapack --without-png --without-tiff  --without-dc1394v1 --without-dc1394v2 --without-assembler --without-inline_asm --host x86_64-windows
	exit
fi


#A repeatable PRNG which works by recursively summung the seed
#This is used to generate UUIDs
#libcvd's unique key generated as follows:
#unique=$(dd if=$devrandom count=1 bs=8 2> /dev/null | sha1sum  | awk '{print $1}')
unique=da39a3ee5e6b4b0d3255bfef95601890afd80709

#Get the initial state by concatenating the key and the arguments
rng_state="$unique $#"

function rng()
{
	rng_state=$(echo -n "$rng_state" | sha1sum | awk '{print $1}')	
	uuid=$(echo $rng_state | sed -e's/\(........\)\(....\)\(...\)\(...\)\(............\).*/\1-\2-4\3-b\4-\5/' | awk '{print toupper($0)}')
}

rng


echo ... $*

n="$1"
debuglibs="$2"
releaselibs="$3"

make clean

sed -e"s!XXXDEBUGLIBSXXX!$debuglibs!" -e"s!XXXRELEASELIBSXXX!$releaselibs!"  make/libcvd.vcproj.header \
                                         > "build/vc2008/libcvd-$n.vcproj"
bash make/make_vcproj_source.bash       >> "build/vc2008/libcvd-$n.vcproj"
bash make/make_vcproj_headers.bash "$n" >> "build/vc2008/libcvd-$n.vcproj"
cat make/libcvd.vcproj.footer           >> "build/vc2008/libcvd-$n.vcproj"
cp cvd/config.h                            "build/vc2008/config-$n.h"


####Make the .sln file
projsln=$uuid
rng
libcvd=$uuid
rng
install=$uuid
{
cat << FOO

Microsoft Visual Studio Solution File, Format Version 10.00
# Visual Studio 2008
Project("{$projsln}") = "libcvd", "libcvd-$n.vcproj", "{$libcvd}"
EndProject
Project("{$projsln}") = "install", "install.vcproj", "{$install}"
	ProjectSection(ProjectDependencies) = postProject
		{$libcvd} = {$libcvd}
	EndProjectSection
EndProject
Global
	GlobalSection(SolutionConfigurationPlatforms) = preSolution
		Debug|Win32 = Debug|Win32
		Release|Win32 = Release|Win32
	EndGlobalSection
	GlobalSection(ProjectConfigurationPlatforms) = postSolution
		{$libcvd}.Debug|Win32.ActiveCfg = Debug|Win32
		{$libcvd}.Debug|Win32.Build.0 = Debug|Win32
		{$libcvd}.Release|Win32.ActiveCfg = Release|Win32
		{$libcvd}.Release|Win32.Build.0 = Release|Win32
		{$install}.Debug|Win32.ActiveCfg = Debug|Win32
		{$install}.Debug|Win32.Build.0 = Debug|Win32
		{$install}.Release|Win32.ActiveCfg = Release|Win32
		{$install}.Release|Win32.Build.0 = Release|Win32
	EndGlobalSection
	GlobalSection(SolutionProperties) = preSolution
		HideSolutionNode = FALSE
	EndGlobalSection
EndGlobal
FOO
} | awk '{ print $0"\r"}'  > build/vc2008/libcvd-$n.sln

