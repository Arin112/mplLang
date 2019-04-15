$Amt:10.0
$maxAmt:120.0
$multipler:1.002
$additional:0.03
$interval:15.0

main()inline{
	
	once{
		RemoveImages(Self());
		SetImage(Self(), "main", "CustomEmitter");
		SetTextSize(0.6);
		SetDigitalis(CurrentCoords(), 1);
		SetUnitAttribute(Self(), CONST_NULLIFIERDAMAGES, False);
	}
	
	if(GetDigitalis(CurrentCoords())!=1) Amt*=0.85;
	
	SetDigitalisGrowth(CurrentCoords(), True);
	SetDigitalis(CurrentCoords(), 1);
	
	if(Amt<5)Destroy(Self(), 2);
	
	SetPopupText("Amt: " $ round(Amt, 2) $ endl() $ "Interval: " $ round(interval/30.0, 2));
	SetImageScale(Self(), "main", dup(Amt/maxAmt*1.7+0.3));
	AddCreeper(CurrentCoords(), Amt);
	
	if(Amt>=50)once enable = True;
	
	if(enable){
		[x, y] = RandCoordsInRange(CurrentCoords(), 10);
		noEnemy=True;
		do(GetEnemyUnitsInRange(x, y, 8)){
			pop(); // remove uid from stack
			noEnemy=False;
		}
		if(GetCreeper(x, y)>1 && noEnemy){
			Amt/=1.75;
			uid = CreateUnit("CRPLCORE", x, y);
			AddScriptToUnit(uid, "emitter.crpl");
			if(Amt<maxAmt/2)enable = False;
		}
	}
	
	if(Amt < maxAmt) Amt = Amt*multipler + additional;
	else once Amt = maxAmt;
	
	Delay(interval);
}