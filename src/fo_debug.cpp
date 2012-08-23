/*

Functions that will print debug information to the mysql error file.

The define FO_LANGUGE_DEBUG_MODE, needs to be true, to get the
debug information.

__author__     = "daniel.lindh@cybercow.se"
__copyright__  = "Copyright 2012, Fareoffice CRS AB"
__maintainer__ = "Daniel Lindh"
__version__    = "1.0.0"
__status__     = "Production"

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fo_debug.hpp>


#ifdef FO_LANGUGE_DEBUG_MODE
	//
	// These global variables may cause some errors. Becuase the mysql module
	// needs to be build thread safe, with no global variables.
	//
	// But it looks to work for singel user mode.
	//
	int debug_counter1 = 0;
	int debug_counter2 = 0;


	void _debug_begin(bool const level1, bool const level2)
	{
		if (level1)
		{
			debug_counter1++;
			debug_counter2 = 0;
		}

		if (level2)
		{
			debug_counter2++;
		}
	}


	/**
	* Writes the debug prefix to the mysql error file.
	*/
	void _debug_echo_prefix(char const * const file, int const line)
	{
		fprintf
		(
			stderr, "%d-%d (%s - %d) ",
			debug_counter1, debug_counter2, file, line
		);
	}


	void _debug_echo(char const * const file, int const line, char const * const text)
	{
		_debug_echo_prefix(file, line);
		fprintf(stderr, "%s\n", text);
	}


	void _debug_echo(char const * const file, int const line, char const * const text, unsigned short *value)
	{
		_debug_echo_prefix(file, line);
		fprintf(stderr, "%s%u\n", text, *value);
	}


	void _debug_echo(char const * const file, int const line, char const * const text, char *value)
	{
		_debug_echo_prefix(file, line);
		fprintf(stderr, "%s%s\n", text, value);
	}


	void _debug_echo
	(
		char const * const file,
		int const line,
		char const * const text,
		long unsigned int const value
	)
	{
		_debug_echo_prefix(file, line);
		fprintf(stderr, "%s%ld\n", text, value);
	}


	void _debug_print_string
	(
		char const * const file, int line,
		char const * const db_column,
		int const length
	)
	{
		unsigned char * byte_pos;
		unsigned short * short_pos;
		unsigned int * int_pos;
		fprintf
		(
			stderr,
			"byte_pos(%lu)\n",
			sizeof(byte_pos)
		);

		_debug_echo(file, line,"---------- foFunction - Debug string -------");
		for (unsigned short i=0; i<length ;i++)
		{
			byte_pos = (unsigned char*)(db_column+i);
			short_pos = (unsigned short*)(db_column+i);
			int_pos = (unsigned int*)(db_column+i);
			_debug_echo_prefix(file, line);
			fprintf
			(
				stderr,
				"Pos(%2d), Adr(%lu), UByte(%3d), UShort(%5d), UInt(%10d), Chr(%c)\n",
				i,
				(unsigned long)byte_pos,
				abs(*byte_pos),
				abs(*short_pos),
				abs(*int_pos),
				*byte_pos
			);
		}
		_debug_echo(file, line,"--------------------------------------------");
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
