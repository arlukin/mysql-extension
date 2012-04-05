// Globala variabler för debug funktionerna.



//
// Must build thread safe, no global variables..
//
//

void debugPrintString(char const * const dbColumn, int length)
{
	unsigned char * bytePos;
	unsigned short * shortPos;
	unsigned int * intPos;
	fprintf(stderr,"---------- foFunction - Debug string -------------\n");
	for (int i=0; i<length ;i++)
	{
		bytePos = (unsigned char*)(dbColumn+i);
		shortPos = (unsigned short*)(dbColumn+i);
		intPos = (unsigned int*)(dbColumn+i);
		fprintf
		(
			stderr,"Adr(%d), Pos(%d), Chr(%c), Byte(%d), Short(%d), Int(%d)\n",
			bytePos,
			i,
			*bytePos,
			abs(*bytePos),
			abs(*shortPos),
			abs(*intPos)
		);
	}
	fprintf(stderr,"--------------------------------------------------\n");
}




char *gResultPointer;
char *gResult;
unsigned long *gLength;













void debugStart(char *result, unsigned long *length)
{
	gLength = length;
	gResult = result;
	gResultPointer = result;

	memcpy(gResultPointer, "Debug output: ", 14);
	gResultPointer+=14;
	*gLength=14;
}

//

char *debugStr(char* cText, char *caption = NULL)
{
	int textLen = strlen(cText);

	*gResultPointer ='['; gResultPointer++;

	if (caption)
	{
		int captionLen = strlen(caption);
		memcpy(gResultPointer, caption, captionLen);
		gResultPointer+=captionLen;

		*gResultPointer =':'; gResultPointer++;
		*gResultPointer =' '; gResultPointer++;

		*gLength+=captionLen+2;
	}

	memcpy(gResultPointer, cText, textLen);
	gResultPointer+=textLen;
	*gResultPointer =']'; gResultPointer++;
	*gLength+=textLen+2;

	return gResult;
}

//

char *debugStr(char cChar, char *caption = NULL)
{
	char cText[2];
	cText[0] = cChar;
	cText[1] = 0;


	return debugStr((char*)cText, caption);
}
//

char *debugInt(int number, char *caption = NULL)
{
				// @todo: my_itoa finns inte i den mysql vi compilerat på senare
        //char cText[20];
        //my_itoa(number, cText, 10);

        //return debugStr(cText, caption);
        return "";
}

char *debugDouble(double number, char *caption = NULL)
{
	char cText[20];
  sprintf(cText, "%f\0", number);
  return debugStr(cText, caption);
}



