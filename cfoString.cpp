/*
#include "cfoString.h"
#include <string.h>


//

cfoString::cfoString(int length)
{
	strLength = 0;
	str = NULL;
	allocateBuffer(length);
}

//

cfoString::~cfoString()
{
	delete str;
}

//

void cfoString::allocateBuffer(int length)
{
	if (strLength < length)
	{
		strLength = length;
		delete str;
		str = new char[strLength];
	}
}

int cfoString::getLength()
{
	return strLength;
}

char* cfoString::getString()
{
	return str;
}

void cfoString::setString(char *string, int length)
{
	allocateBuffer(length);
	memcpy(str, string, length);
}
*/