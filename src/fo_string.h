#ifndef _cString_h
#define _cString_h

class FOString
{
private:
	int	_length;
	char * str;

public:

	FOString(int len = 0)
	{
		_length = 0;
		str = NULL;
		allocate_buffer(len);
	};

	~FOString()
	{
		delete[] str;
	};

	inline void allocate_buffer(int len)
	{
		if (_length < len)
		{
			if (str != NULL)
			{
				delete[] str;
				str = NULL;
			}

			_length = len;
			str = new char[_length];
			// Fill string with string terminators.
			for (int i=0;i < _length;i++)
				str[i] = '\0';
		}
	};

	inline int length()
	{
		return _length;
	};

	inline char* get()
	{
		return str;
	};

	inline void set(char const * const string, int len)
	{
		allocate_buffer(len);
		memcpy(str, string, len);
	};

	//
	// Members to modify the string.
	//

	// char/byte
	inline void overwrite(int position, char value)
	{
		allocate_buffer(position + sizeof(value));
		*((char*)(str+position)) = value;
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
		*((unsigned short*)(str+position)) = value;
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
		*((unsigned int*)(str+position)) = value;
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
		*((unsigned long*)(str+position)) = value;
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
		memcpy(str+position, value, len);
	};

	inline void overwrite(int * position, char const * const value, int const len)
	{
		overwrite(*position, value, len);
		*position += len;
	};
};

#endif
