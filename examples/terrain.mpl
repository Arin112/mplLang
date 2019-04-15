main()inline{
	HideTraceLog();
	ClearTraceLog();
	once do(MapHeight()){
		do(MapWidth())
			SetTerrain(i, j, (((sin((i+j)/10.0)+1)/2.0)*5.0 * ((cos((i-j)/10.0)+1.0)/2.0)*5.0 )/25.0 * 9+1);
		delay(1);
	}
}

