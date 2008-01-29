BEGIN{
	key=0
}




#A log entry
/RCS file:/{
	getline
	sub(/^Working file: /,"")
	filename=$0

	do
		getline
	while(!/symbolic names/)

	delete sym

	getline
	for(;!/keyword substitution/;getline)
	{
		sub(/:/,"",$1)
		sym[$2]=$1
	}


	while(!/----------/)
		getline

	while(1)
	{
		getline
		revision=$2
		getline
		date=$2" "$3

		datestamp=date
		gsub(/[^0-9]+/, " ", datestamp)

		epoch=mktime(datestamp)
		date = strftime("%a %e %b, %Y %H:%M:%S", epoch)


		author = $6
		sub(/;$/, "", author)

		#Try to build a checkin stamp
			
		tag=sym[revision]
	
		#Find the most recent date for the tag (if present)
		if(tag != "" && alltags[tag] < datestamp)
			alltags[tag] = datestamp

		text=""
		#Get text
		while(getline && !/-----/ && !/======/)
			text=text (text == "" ? "" : "\n") $0

		#Now we have: filename revision date datestamp author tag text key
		authors[key] = author
		revisions[key] = revision
		tags[key] =  tag
		texts[key] = text
		files[key] = filename
	
		dates[datestamp] = date

		entries[datestamp] = entries[datestamp] " " key

		key++

		if(/========/)
			break
	}
}



END{
	#Put tags in to entries
	for(i in alltags)
		entries[alltags[i]] = "tag " i


	n = asorti(entries, se)

	for(i=n; i >0; i--)
	{
		ds = se[i]
		keylist = entries[ds]
		nk = split(keylist, keys)

		#Try to concatenate non-atomic commits
		concat=""
		nc=1
		for(;;)
		{
			ds1 = se[i-1]
			kl1 = entries[ds1]
			nk1 = split(kl1, k1)
			if(authors[keys[1]] == authors[k1[1]] && texts[keys[1]] == texts[k1[1]])
			{
				keylist = keylist " " kl1
				nk = split(keylist, keys)
				i--
				concat = concat ", "(ds1 - ds) "s"
				nc++
			}
			else
				break
		}

		if(concat != "")
			concat = "(Concatenated " nc " entries at 0s" concat ")"


		

		if(keys[1] == "tag")
		{
			print "====================================================================="
			print "Repository tagged as " keys[2]
			print "====================================================================="
			print ""
			print ""
		}
		else
		{
			print dates[ds] " author: " authors[keys[1]] " " concat

			for(j=1; j <= nk; j++)
			{
				k = keys[j]
				print "   File: " files[k] " (Rev. " revisions[k] ")" 
			}

			print ""
			print texts[keys[1]]
			print ""
			print "-----------------------------------"
			print ""
		}
	}

}
