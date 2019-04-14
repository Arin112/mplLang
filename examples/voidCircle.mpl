$radius:20

main()inline{
	once{
		[x0, y0] = CurrentCoords();
		[x, y, err] = radius, 0, 0;
		
		while (x >= y){
			SetVoid(x0 + x, y0 + y);
			SetVoid(x0 + y, y0 + x);
			SetVoid(x0 - y, y0 + x);
			SetVoid(x0 - x, y0 + y);
			SetVoid(x0 - x, y0 - y);
			SetVoid(x0 - y, y0 - x);
			SetVoid(x0 + y, y0 - x);
			SetVoid(x0 + x, y0 - y);
			
			if (err <= 0){
				y += 1;
				err += 2*y + 1;
			}

			if (err > 0){
				x -= 1;
				err -= 2*x + 1;
			}
		}
	}
}