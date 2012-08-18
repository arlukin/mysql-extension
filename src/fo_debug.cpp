/*

Functions that will print debug information to the mysql error file.

The define FO_LANGUGE_DEBUG_MODE, needs to be true, to get the
debug information.

__author__     = "daniel.lindh@cybercow.se"
__copyright__  = "Copyright 2012, Fareoffice CRS AB"
__maintainer__ = "Daniel Lindh"
__credits__    = "???"
__license__    = "???"
__version__    = "1.0.0"
__status__     = "Production"

*/


#include <stdio.h>


#ifdef FO_LANGUGE_DEBUG_MODE
	//
	// These global variables may cause some errors. Becuase the mysql module
	// needs to be build thread safe, with no global variables.
	//
	// But it looks to work for singel user mode.
	//
	int debugCounter1 = 0;
	int debugCounter2 = 0;

	/**
	* Handles prefix counter for _debug_echo_prefix, used
	* for grouping many debug_echo() output to one section.
	*
	*	@access public
	*/
	void _debug_begin(bool level1 = true, bool level2=false)
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
	*/
	void _debug_echo_prefix(char * file, int line)
	{
		fprintf(stderr, "%d-%d (%s - %d) ", debugCounter1, debugCounter2,  file, line);
	}


	/**
	* Writes a text to the mysql error file, with a prefix.
	*/
	void _debug_echo(char * file, int line, char const * const text)
	{
		_debug_echo_prefix(file, line);
		fprintf(stderr, "%s\n", text);
	}


	void _debug_echo(char * file, int line, char const * const text, unsigned short *value)
	{
		_debug_echo_prefix(file, line);
		fprintf(stderr, "%s%u\n", text, *value);
	}


	void _debug_echo(char * file, int line, char const * const text, char *value)
	{
		_debug_echo_prefix(file, line);
		fprintf(stderr, "%s%s\n", text, value);
	}


	void _debug_echo
	(
		char * file,
		int line,
		char const * const text,
		long unsigned int value
	)
	{
		_debug_echo_prefix(file, line);
		fprintf(stderr, "%s%ld\n", text, value);
	}


	/**
	* Writes what's in the dbColumn string, character by character,
	* both with charachter, number and memory adress.
	*/
	void _debug_print_string
	(
		char * file, int line,
		char const * const dbColumn,
		int length
	)
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

		_debug_echo(file, line,"---------- foFunction - Debug string --------");
		for (unsigned short i=0; i<length ;i++)
		{
			bytePos = (unsigned char*)(dbColumn+i);
			shortPos = (unsigned short*)(dbColumn+i);
			intPos = (unsigned int*)(dbColumn+i);
			_debug_echo_prefix(file, line);
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
		_debug_echo(file, line,"---------------------------------------------");
	}


#endif


void check_variable_type_compatiblity()
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
