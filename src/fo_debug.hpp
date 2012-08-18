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
// #define FO_LANGUGE_DEBUG_MODE

#ifndef FO_LANGUGE_DEBUG_MODE
	#define debug_begin(level1, level2)
	#define debug_echo_prefix()
	#define debug_echo(text)
	#define debug_echo_ex(text, value)
	#define debug_print_string(dbColumn, length)

#else
	//
	// These global variables may cause some errors. Becuase the mysql module
	// needs to be build thread safe, with no global variables.
	//
	// But it looks to work for singel user mode.
	//
	int debugCounter1 = 0;
	int debugCounter2 = 0;

	#define debug_begin(level1, level2) _debug_begin(level1, level2)
	#define debug_echo_prefix() _debug_echo_prefix(__FILE__, __LINE__)
	#define debug_echo(text) _debug_echo(__FILE__, __LINE__, text)
    #define debug_echo_ex(text, value) _debug_echo(__FILE__, __LINE__, text, value)
	#define debug_print_string(dbColumn, length) _debug_print_string(__FILE__, __LINE__, dbColumn, length)


	/**
	* Handles prefix counter for _debug_echo_prefix, used
	* for grouping many debug_echo() output to one section.
	*
	*	@access public
	*/
	void _debug_begin(bool level1 = true, bool level2=false);

	/**
	* Writes the debug prefix to the mysql error file.
	*/
	void _debug_echo_prefix(char * file, int line);

	/**
	* Writes a text to the mysql error file, with a prefix.
	*/
	void _debug_echo(char * file, int line, char const * const text);


	void _debug_echo(char * file, int line, char const * const text, unsigned short *value);

	void _debug_echo(char * file, int line, char const * const text, char *value);


	void _debug_echo
	(
		char * file,
		int line,
		char const * const text,
		long unsigned int value
	);


	/**
	* Writes what's in the dbColumn string, character by character,
	* both with charachter, number and memory adress.
	*/
	void _debug_print_string
	(
		char * file, int line,
		char const * const dbColumn,
		int length
	);


#endif


void check_variable_type_compatiblity();
