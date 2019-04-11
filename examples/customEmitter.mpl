#include "utils.mpl" // you can include mpl files
$creepAmt:10 // just like normal CRPL
$addPerTime:1
$Delay:15
$maxAmt:1000
$incrementInterval:10

amt()inline, CRPL{<-creepAmt 1 add} // also you can create CRPL functions

main()inline{ // the function that calls every tick
	/*
	recommended to be inline.
	all inline function will be implaced to their calls.
	*/
	once{
		RemoveImages(Self());
		SetImage(Self(), "main", "CustomEmitter");
		SetTextSize(0.6);
		SetTimer1(incrementInterval + 1);
	}
	if(GetTimer1() == 0){
		SetText("Amt " $ creepAmt $ endl() $ "Interval: " $ round(Delay/30.0, 2));
		SetCreeper(CurrentCoords(), amt());
		SetTimer1(Delay);
	}
	if (GetTimer0()==1 && creepAmt < maxAmt){
		creepAmt = creepAmt + addPerTime;
		SetTimer0(incrementInterval + 1);
	}
	if (GetTimer2() == 0){
		f(RandCoords());
		SetTimer2(60);
	}
	else if(creepAmt > maxAmt) once creepAmt = maxAmt;
}
