int sum (int a, int b ) {
	int sum = a + b ;
	return sum ;
}

int sum_five_parameter ( int a, int b, int c, int d, int e ) {
	return a + b + c + d + e ;
}


void reset_handler(void)
{
	sum(1,2);
	sum_five_parameter(1,2,3,4,5);
}
