/^have/{
	what=substr($1, 6)
	have[what]= ($2 == "yes")
	next
}

/^#/{next}
NF==0{next}

!selected{
	
	for(i=2; i <= NF; i++)
		if($i != "END" && !have[$i])
			next

	selected= ($NF != "END")
	
	if($1 != "nothing")
	{
		nd = split($1, dependees, ",")
		
		for(i=1; i <= nd; i++)
			print dependees[i]
	}
}

$NF=="END"{
	selected=0
}
