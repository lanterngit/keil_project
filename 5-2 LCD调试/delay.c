void delay(time)		//@11.0592MHz
{
	int a;
	for(a = 0;a<time;a++)
	{
		unsigned char i, j;
		i = 2;
		j = 199;
		do
		{
			while (--j);
		} while (--i);
//		i = 43;
//		while (--i);
	}
}