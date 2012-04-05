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
	#define debugPrintString(dbColumn, length)

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
	#define debugPrintString(dbColumn, length) _debugEcho(__FILE__, __LINE__, text)

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

	/**
	* Writes what's in the dbColumn string, character by character,
	* both with charachter, number and memory adress.
	*
	*	@access public
	*/
	void debugPrintString(char * file, int line, char const * const dbColumn, int length)
	{
		unsigned char * bytePos;
		unsigned short * shortPos;
		unsigned int * intPos;
		_debugEcho(file, line,"---------- foFunction - Debug string -------------");
		for (unsigned short i=0; i<length ;i++)
		{
			bytePos = (unsigned char*)(dbColumn+i);
			shortPos = (unsigned short*)(dbColumn+i);
			intPos = (unsigned int*)(dbColumn+i);
			_debugEchoPrefix(file, line);
			fprintf
			(
				stderr,"Adr(%d), Pos(%d), Chr(%c), UByte(%d), UShort(%d), UInt(%d)\n",
				bytePos,
				i,
				*bytePos,
				abs(*bytePos),
				abs(*shortPos),
				abs(*intPos)
			);
		}
		_debugEcho(file, line,"--------------------------------------------------");
	}

#endif
