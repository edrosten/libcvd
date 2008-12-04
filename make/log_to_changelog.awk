BEGIN{
	revision_key=0
}

function gline()
{
	if(!getline)
		error("Unexpected EOF")
	return 1
}

function error(s)
{
	print s > "/dev/stderr"
	exit(1)
}

/^?/{next}

#Process a file entry
/^Working file:/{

	#Get the filename
	filename=$3
	
	#Find the tags
	while(gline() && !/^symbolic names:/);

	#Extract tags
	delete revision_to_tag
	while(gline() && ! /^keyword substitution:/)
	{
		tagname=substr($1, 1, length($1)-1)
		revision_to_tag[$2]=tagname
	}
	
	#Skip over the rest until the revisions
	while(gline() && !/^----------------/);
	
	#There is always at least one revision
	do
	{
		#The revision number
		gline()
		revision=$2

		#The commit line
		gline()
		author=substr($6, 1, length($6)-1)
		
		#Remove the variable line count.
		if($9 == "lines:")
		{
			$10 = ""
			$11 = ""
		}
		$0=$0
		commit_line=$0
		
		#Store the date* 10 for extra precision
		date = $2" "$3
		gsub(/[^0-9]+/, " ", date)
		date=mktime(date)*10
		
		#The commit message
		message=""
		while(gline() && !/^[-=]+$/)
			message = message $0 "\n"

		#The commit line is the primary key.
		
		#Keep track of commits
		date_to_line[date]=commit_line
		line_to_message[commit_line]=message
		line_to_author[commit_line]=author
		line_to_files[commit_line]=line_to_files[commit_line] " " filename
		line_to_revision[commit_line]=line_to_revision[commit_line] " " revision
		
		#The last file commit which has a certain tag 
		#happens just before the tag.
		if(tag_to_date[revision_to_tag[revision]] < date)
			tag_to_date[revision_to_tag[revision]] = date
	}
	while(!/^=+$/)
}


END{
	#Delete the null tag for untagged revisions
	delete tag_to_date[""]

	#The last
	for(i in tag_to_date)
	{	
		#Put the tag in just after the last commit
		date = tag_to_date[i] + 1
		date_to_line[date]="hello"
		date_to_tag[date]=i
	}


	#Sort commits by date
	num_dates=asorti(date_to_line, sorted_dates)
	
	#Iterate through all commits, newest first
	for(i=num_dates; i > 0; i--)
	{
		date = sorted_dates[i]

		if(date in date_to_tag)
		{
			print "=========================================="
			print "Repository tagged as " date_to_tag[date]
			print "=========================================="
		}
		else
		{
			line = date_to_line[date]

			author=line_to_author[line]
			message=line_to_message[line]

			file_list=line_to_files[line]
			rev_list=line_to_revision[line]
			
			print ""
			#Long commits can get spread over several seconds.
			#Try to concatenate entries which differ by a few 
			#seconds, but have the same commit messsage and 
			#author
			#We allow 20 seconds
			concatenation=""
			num_concat=0
			for(j=i-1; j>0 && 
					   author==line_to_author[date_to_line[sorted_dates[j]]] &&
					   message==line_to_message[date_to_line[sorted_dates[j]]] &&
					   date - sorted_dates[j] < 1000; j--)
			{
				#Reset i, so that the fragment appears once only.
				i=j
				
				#Append to the file and revision list
				file_list=file_list " " line_to_files[date_to_line[sorted_dates[j]]];
				rev_list=rev_list " " line_to_revision[date_to_line[sorted_dates[j]]];

				#Make a note of teh concatenations
				concatenation=concatenation ", " (date - sorted_dates[j])/10 "s"
				num_concat++
			}

			#Create a message based on concatenation
			if(num_concat)
				concatenation = " (Concatenated " num_concat+1 " entries at " substr(concatenation, 3) ")"
			
			#Leader line
			print strftime("%a %e %b, %Y %H:%M:%S", date/10) " author: " author concatenation
			
			#List files commit happened on
			numfiles=split(file_list, files);
			split(rev_list, revisions);

			for(j=1; j <= numfiles; j++)
				print "    File: " files[j] " (Rev. " revisions[j] ")"
			
			#Print the commit message
			print "\n" message "\n--------------"
		}
	}
}
