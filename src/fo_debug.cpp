/***************************************************************************
*	Debug functions.
*
*	Functions that will print debug information to the mysql error file.
*
*	The define FO_LANGUGE_DEBUG_MODE, needs to be true, to get the
*	debug information.
*
*	@author Daniel Lindh <daniel@fareoffice.com>
*	@package FoFunctions
*	@subpackage FoDebug
*	@filesource
***************************************************************************/


// Enable/Disable debug mode!
//#define FO_LANGUGE_DEBUG_MODE

#ifndef FO_LANGUGE_DEBUG_MODE

	#define debugBegin(level1, level2)
	#define debugEchoPrefix()
	#define debugEcho(text)
	#define debugEchoEX(text, value)
	#define debugPrintString(dbColumn, length)
	#define debugCheckVariableTypeSize()

#else
	//
	// Those global variables may cause some errors.
	// Becuase the mysql module needs to be build
	// thread safe, with no global variables.
	//
	// But it looks to work for singel user mode.
	//
	int debugCounter1 = 0;
	int debugCounter2 = 0;

	#define debugBegin(level1, level2) _debugBegin(level1, level2)
	#define debugEchoPrefix() _debugEchoPrefix(__FILE__, __LINE__)
	#define debugEcho(text) _debugEcho(__FILE__, __LINE__, text)
    #define debugEchoEX(text, value) _debugEcho(__FILE__, __LINE__, text, value)
	#define debugPrintString(dbColumn, length) _debugPrintString(__FILE__, __LINE__, dbColumn, length)
	#define debugCheckVariableTypeSize() _debugCheckVariableTypeSize()

	/**
	* Handles prefix counter for _debugEchoPrefix, used
	* for grouping many debugEcho() output to one section.
	*
	*	@access public
	*/
	void _debugBegin(bool level1 = true, bool level2=false)
	{
		if (level1)
		{
			debugCounter1++;
			debugCounter2 = 0;
		}

		if (level2)
		{
			debugCounter2++;
		}
	}

	/**
	* Writes the debug prefix to the mysql error file.
	* Should be used with fprintf(stderr, ...) to get the right
	* format on the stderr output.
	*
	*	@access public
	*/
	void _debugEchoPrefix(char * file, int line)
	{
		fprintf(stderr, "%d-%d (%s - %d) ", debugCounter1, debugCounter2,  file, line);
	}

	/**
	* Writes a text to the mysql error file, with a prefix.
	*
	*
	*	@access public
	*/
	void _debugEcho(char * file, int line, char const * const text)
	{
		_debugEchoPrefix(file, line);
		fprintf(stderr, "%s\n", text);
	}

	void _debugEcho(char * file, int line, char const * const text, unsigned short *value)
	{
		_debugEchoPrefix(file, line);
		fprintf(stderr, "%s%u\n", text, *value);
	}

	void _debugEcho(char * file, int line, char const * const text, char *value)
	{
		_debugEchoPrefix(file, line);
		fprintf(stderr, "%s%s\n", text, value);
	}

	void _debugEcho(char * file, int line, char const * const text, long unsigned int value)
	{
		_debugEchoPrefix(file, line);
		fprintf(stderr, "%s%ld\n", text, value);
	}
	/**
	* Writes what's in the dbColumn string, character by character,
	* both with charachter, number and memory adress.
	*
	*	@access public
	*/

	void _debugPrintString(char * file, int line, char const * const dbColumn, int length)
	{
		unsigned char * bytePos;
		unsigned short * shortPos;
		unsigned int * intPos;
			fprintf
			(
				stderr,
				"bytePos(%lu)\n",
				sizeof(bytePos)
			);

		_debugEcho(file, line,"---------- foFunction - Debug string -------------");
		for (unsigned short i=0; i<length ;i++)
		{
			bytePos = (unsigned char*)(dbColumn+i);
			shortPos = (unsigned short*)(dbColumn+i);
			intPos = (unsigned int*)(dbColumn+i);
			_debugEchoPrefix(file, line);
			fprintf
			(
				stderr,
				"Pos(%2d), Adr(%lu), UByte(%3d), UShort(%5d), UInt(%10d), Chr(%c)\n",
				i,
				(unsigned long)bytePos,
				abs(*bytePos),
				abs(*shortPos),
				abs(*intPos),
				*bytePos
			);
		}
		_debugEcho(file, line,"--------------------------------------------------");
	}

	void _debugCheckVariableTypeSize()
	{
		debugEcho("---- Size of variables ----");
		debugEchoEX("int   4 = ", sizeof(int));
		debugEchoEX("long  8 = ", sizeof(long));
		debugEchoEX("char  1 = ", sizeof(char));
		debugEchoEX("short 2 = ", sizeof(short));

		debugEchoEX("u int = ", sizeof(unsigned int));
		debugEchoEX("u long = ", sizeof(unsigned long));
		debugEchoEX("u char= ", sizeof(unsigned char));
		debugEchoEX("u short = ", sizeof(unsigned short));
	}

#endif

void checkVariableTypeCompatiblity()
{
	if
	(
		sizeof(char) != 1 ||
		sizeof(short) != 2 ||
		sizeof(int) != 4 ||
		sizeof(long) != 8 ||
		sizeof(unsigned char) != 1 ||
		sizeof(unsigned short) != 2 ||
		sizeof(unsigned int) != 4||
		sizeof(unsigned long) != 8
	)
	{
	    fprintf(stderr, "\nERROR: foFunctions.so and foLanguage.so\n");
		fprintf(stderr, "ERROR:    The Fareoffice MYSQL module has c variable type incompatibility\n");

		fprintf(stderr, "char  1 = %ld\n", sizeof(char));
		fprintf(stderr, "short 2 = %ld\n", sizeof(short));
		fprintf(stderr, "int   4 = %ld\n", sizeof(int));
		fprintf(stderr, "long  8 = %ld\n", sizeof(long));

		fprintf(stderr, "u char  1 = %ld\n", sizeof(unsigned char));
		fprintf(stderr, "u short 2 = %ld\n", sizeof(unsigned short));
		fprintf(stderr, "u int   4 = %ld\n", sizeof(unsigned int));
		fprintf(stderr, "u long  8 = %ld\n", sizeof(unsigned long));
	}
}
