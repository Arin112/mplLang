main()inline{
	Trace3(i, j, k);
	do(7){
		Trace3(i, j, k);
		do(3){
			Trace3(i, j, k);
			do(5) Trace3(i, j, k);
		}
	}
	i = j < k ? j : k;
}