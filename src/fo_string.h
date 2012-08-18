/*

The string/byte modifing class.

__author__     = "daniel.lindh@cybercow.se"
__copyright__  = "Copyright 2012, Fareoffice CRS AB"
__maintainer__ = "Daniel Lindh"
__credits__    = "???"
__license__    = "???"
__version__    = "1.0.0"
__status__     = "Production"

*/


#ifndef _cString_h
#define _cString_h

#include <string.h>

class FOString
{
private:
	int	_length;
	char * _str;

public:

	FOString(int len = 0)
	{
		_length = 0;
		_str = NULL;
		allocate_buffer(len);
	};

   	// Copy constructor
    FOString(const FOString &source)
    {
        set(source.get(), source.length());
    }

	FOString& operator=(const FOString &source)
	{
		set(source.get(), source.length());
		return *this;
	}

	~FOString()
	{
		delete[] _str;
	};

	inline void allocate_buffer(int len)
	{
		if (_length < len)
		{
			if (_str != NULL)
			{
				delete[] _str;
				_str = NULL;
			}

			_length = len;
			_str = new char[_length];
			// Fill string with string terminators.
			for (int i=0;i < _length;i++)
				_str[i] = '\0';
		}
	};

	const int& length() const
	{
		return _length;
	};

	char * get() const
	{
		return _str;
	};

	inline void set(char const * const string, int len)
	{
		allocate_buffer(len);
		memcpy(_str, string, len);
	};

	//
	// Members to modify the string.
	//

	// char/byte
	inline void overwrite(int position, char value)
	{
		allocate_buffer(position + sizeof(value));
		*((char*)(_str+position)) = value;
	};

	inline void overwrite(int *position, char value)
	{
		overwrite(*position, value);
		*position+=sizeof(value);
	};

	// unsigned short
	inline void overwrite(int position, unsigned short value)
	{
		allocate_buffer(position + sizeof(value));
		*((unsigned short*)(_str+position)) = value;
	};

	inline void overwrite(int *position, unsigned short value)
	{
		overwrite(*position, value);
		*position+=sizeof(value);
	};

	// unsigned int
	inline void overwrite(int position, unsigned int value)
	{
		allocate_buffer(position + sizeof(value));
		*((unsigned int*)(_str+position)) = value;
	};

	inline void overwrite(int *position, unsigned int value)
	{
		overwrite(*position, value);
		*position+=sizeof(value);
	};

	// unsigned long
	inline void overwrite(int position, unsigned long value)
	{
		allocate_buffer(position + sizeof(value));
		*((unsigned long*)(_str+position)) = value;
	};

	inline void overwrite(int * position, unsigned long value)
	{
		overwrite(*position, value);
		*position+=sizeof(value);
	};

	// char *
	inline void overwrite(int position, char const * const value, int len)
	{
		allocate_buffer(position + len);
		memcpy(_str+position, value, len);
	};

	inline void overwrite(int * position, char const * const value, int const len)
	{
		overwrite(*position, value, len);
		*position += len;
	};
};

#endif
