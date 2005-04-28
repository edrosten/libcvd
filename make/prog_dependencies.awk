BEGIN{
	RS="progs"
}

NR==1{
	for(i=1; i <= NF; i++)
		opts[$i]=1
	ORS=" "
	next
}

NF{
	for(i=2; i <= NF; i++)
		if(!($i in opts))
			next
	print "progs" $1
}
