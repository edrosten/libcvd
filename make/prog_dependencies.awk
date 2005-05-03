BEGIN{
	RS=" "
	ORS=" "
	a = -1
}

$0~stub{
	print prev
	prev=$1
	next
}

!prev{
	opts[$1]=1
	next
}

!($1 in opts){prev=" "}

END{
	print prev
}



