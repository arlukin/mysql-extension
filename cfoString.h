#ifndef _cString_h
#define _cString_h

class cfoString
{
private:
	char * str;
	int	strLength;

public:
	cfoString(int length)
	{
		strLength = 0;
		str = NULL;
		allocateBuffer(length);
	};

	~cfoString()
	{
		delete str;
	};

	//

	inline void allocateBuffer(int length)
	{
		if (strLength < length)
		{
			strLength = length;
			delete str;
			str = new char[strLength];
		}
	};

	//

	inline char* getString()
	{
		return str;
	};

	inline int getLength()
	{
		return strLength;
	};


	inline void setString(char const * const string, int length)
	{
		allocateBuffer(length);
		memcpy(str, string, length);
	};

	// Mod members

	// unsigned short

	inline void modString(int position, unsigned short value)
	{
		*((unsigned short*)(str+position)) = value;
	};

	inline void modString(int *position, unsigned short value)
	{
		modString(*position, value);
		*position+=sizeof(unsigned long);
	};

	// char/byte

	inline void modString(int position, char value)
	{
		*((char*)(str+position)) = value;
	};

	inline void modString(int *position, char value)
	{
		modString(*position, value);
		*position+=sizeof(char);
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
