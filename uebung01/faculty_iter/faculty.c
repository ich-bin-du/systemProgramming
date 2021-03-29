/*
 * faculty.c
 *
 * Created: 15.03.2021 12:36:18
 * Author : Dominik Amthor
 */ 

#include <stdio.h>


int faculty_iter( int n )
{
	int result = 1;

	if ( n == 0 )
		return result;
	else if ( n < 0 )
		n = n * (-1);

	for ( int i = 1; i <= n; i++ )
		result = result * i;

	return result;
}

int faculty_rec( int n )
{
	if ( n == 0 )
		return 1;
	else if ( n > 0 )
		return n * faculty_rec( n - 1 );
}


int main( void )
{
	int n;
	printf( "Gib eine ganze Zahl ein: " );
	scanf( "%i", &n );

	printf( "Fakultät iterativ = %i\n", faculty_iter( n ) );
	printf( "Fakultät rekursiv = %i", faculty_rec( n ) );

	return 0;
}

