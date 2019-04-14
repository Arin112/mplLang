f1(){
	[x, y] = stack(), stack();
	x*7-y; // return value
}
f2()inline{
	[x, y] = stack(), stack();
	x*7-y; // return value
}
f3()inline, CRPL{swap 7 mul swap sub}
f4()CRPL{swap 7 mul swap sub}

main()inline{
	ShowTraceLog();
	ClearTraceLog();
	
	Trace("f1 " $ f1(1, 2));
	Trace("f2 " $ f2(1, 2));
	Trace("f3 " $ f3(1, 2));
	Trace("f4 " $ f4(1, 2));
}