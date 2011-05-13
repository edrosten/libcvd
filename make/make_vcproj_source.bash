function process()
{
	d=0
	if [ "$1" != "" ]
	then
		d=1
cat << FOO
			<Filter
				Name="$1"
				>
FOO
	fi

	#Sort the files by filename ignoring the directories, to match the VS default
	awk -F/ '{print $(NF),$0}' | sort | awk -vd=$d -vconfig="$config" '

	BEGIN{
		prefix="\"..\\..\\"
		tabs=""
		for(i=0; i < d; i++)
			tabs = tabs "	"
	}
	/config.h/{
		print config
		next
	}
	{
		gsub(/\//, "\\")
		print tabs "			<File"
		print tabs "				RelativePath=" prefix $2 "\""
		print tabs "				>"
		print tabs "			</File>"
	}'

	if [ "$1" != "" ]
	then
		echo '			</Filter>'
	fi
}


cat << FOO
		<Filter
			Name="Source Files"
			Filter="cpp;c;cc;cxx;def;odl;idl;hpj;bat;asm;asmx"
			UniqueIdentifier="{4FC737F1-C7A5-4376-A066-2A32D752A2FF}"
			>
FOO

make -n libcvd.a | grep g++ | grep -v -e Linux -e SSE -e OSX -e Win32 -e noarch -e pnm_src -e /fast/ -e i686 | awk '{print $(NF-2)}' | process

make -n libcvd.a | grep g++ | grep  /fast/ | awk '{print $(NF-2)}' | process fast
make -n libcvd.a | grep g++ | grep  i686   | awk '{print $(NF-2)}' | process i686
make -n libcvd.a | grep g++ | grep  noarch | awk '{print $(NF-2)}' | process noarch
make -n libcvd.a | grep g++ | grep  pnm_src | awk '{print $(NF-2)}' | process pnm_src
make -n libcvd.a | grep g++ | grep  Win32 | awk '{print $(NF-2)}' | process Win32
make -n libcvd.a | grep g++ | grep  Linux | awk '{print $(NF-2)}' | process Linux
make -n libcvd.a | grep g++ | grep  OSX | awk '{print $(NF-2)}' | process OSX
make -n libcvd.a | grep g++ | grep  SSE | awk '{print $(NF-2)}' | process SSE


cat << FOO
		</Filter>
FOO

