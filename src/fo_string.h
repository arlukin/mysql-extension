/***************************************************************************
	FoString

	Functions to modifiy a string.
***************************************************************************/
#ifndef _cString_h
#define _cString_h

class cfoString
{
private:
	char * str;
	int	strLength;

public:

	/**
	*
	*	@access public
	*/
	cfoString(int length = 0)
	{
		strLength = 0;
		str = NULL;
		allocateBuffer(length);
	};


	/**
	*
	*	@access public
	*/
	~cfoString()
	{
		delete[] str;
	};

	/**
	*
	*	@access public
	*/
	inline void allocateBuffer(int length)
	{
		if (strLength < length)
		{
			if (strLength > 0)
				delete[] str;

			strLength = length;
			str = new char[strLength];
		}
	};

	/**
	*
	*	@access public
	*/
	inline char* getString()
	{
		return str;
	};

	/**
	*
	*	@access public
	*/
	inline int getLength()
	{
		return strLength;
	};


	/**
	*
	*	@access public
	*/
	inline void setString(char const * const string, int length)
	{
		allocateBuffer(length);
		memcpy(str, string, length);
	};

	//
	// Members the modify the string.
	//

	// unsigned short
	inline void modString(int position, unsigned short value)
	{
		*((unsigned short*)(str+position)) = value;
	};

	inline void modString(int *position, unsigned short value)
	{
		modString(*position, value);
		*position+=sizeof(value);
	};

	// unsigned int
	inline void modString(int position, unsigned int value)
	{
		*((unsigned int*)(str+position)) = value;
	};

	inline void modString(int *position, unsigned int value)
	{
		modString(*position, value);
		*position+=sizeof(value);
	};

	// char/byte
	inline void modString(int position, char value)
	{
		*((char*)(str+position)) = value;
	};

	inline void modString(int *position, char value)
	{
		modString(*position, value);
		*position+=sizeof(value);
	};

	// unsigned long
	inline void modString(int position, unsigned long value)
	{
		*((unsigned long*)(str+position)) = value;
	};

	inline void modString(int * position, unsigned long value)
	{
		modString(*position, value);
		*position+=sizeof(value);
	};

	// char *
	inline void modString(int position, char const * const value, int length)
	{
		memcpy(str+position, value, length);
	};

	inline void modString(int * position, char const * const value, int const length)
	{
		modString(*position, value, length);
		*position += length;
	};
};

#endif
