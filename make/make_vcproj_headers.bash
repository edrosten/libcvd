function process()
{
	#Sort the files by filename ignoring the directories, to match the VS default
	awk -F/ '{print $(NF),$0}' | sort | awk -vd=$1 -vconfig="$config" '

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
}

config='			<File
				RelativePath="config-'"$1"'.h"
				>
				<FileConfiguration
					Name="Debug|Win32"
					>
					<Tool
						Name="VCCustomBuildTool"
						CommandLine="copy &quot;$(InputPath)&quot; &quot;$(InputDir)..\\..\\cvd\\config.h&quot;&#x0D;&#x0A;"
						Outputs="&quot;$(InputDir)..\\..\\cvd\\config.h&quot;"
					/>
				</FileConfiguration>
				<FileConfiguration
					Name="Release|Win32"
					>
					<Tool
						Name="VCCustomBuildTool"
						CommandLine="copy &quot;$(InputPath)&quot; &quot;$(InputDir)..\\..\\cvd\\config.h&quot;&#x0D;&#x0A;"
						Outputs="&quot;$(InputDir)..\\..\\cvd\\config.h&quot;"
					/>
				</FileConfiguration>
			</File>'



cat << FOO
		<Filter
			Name="Header Files"
			Filter="h;hpp;hxx;hm;inl;inc;xsd"
			UniqueIdentifier="{93995380-89BD-4b04-88EB-625FBE52EBFB}"
			>
FOO

find cvd -name *.h | grep -v -e internal -e Linux -e IRIX -e OSX -e python | process 0

cat << FOO
			<Filter
				Name="internal"
				>
FOO

find cvd/internal -name *.h | process 1

cat  <<FOO
			</Filter>
		</Filter>
FOO
