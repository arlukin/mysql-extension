// Compile and run.
// gcc -Wall -fPIC -lstdc++ -o checkDataTypeSize.exe checkDataTypeSize.cpp
// ./checkDataTypeSize.exe
#include<stdio.h>
int main()
{
	printf("int   4 = %ud\n", sizeof(int));
	printf("long  8 = %ud\n", sizeof(long));
	printf("char  1 = %ud\n", sizeof(char));
	printf("short 2 = %ud\n", sizeof(short));

	printf("u int   = %ud\n", sizeof(unsigned int));
	printf("u long  = %ud\n", sizeof(unsigned long));
	printf("u char  = %ud\n", sizeof(unsigned char));
	printf("u short = %ud \n", sizeof(unsigned short));

	return 1;
}

