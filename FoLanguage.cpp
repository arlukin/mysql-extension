/***************************************************************************
  fo Language functions

	Used to save multiple translations of a text in one mysql column.

	The texts are loaded from and saved to the database with two UDF functions,
	getLanguage and setLanguage.


	Functions
	---------

	* setLanguage
		See description in function header.

	* getLanguage
		See description in function header.

	Not yet implemented functions.
	------------------------------

	* getLanguageProperties(rule.description, 'validationcode');

	* delLanguage(rule.description, $language);


	Format of the foLanguageColumn.
	------------------------------

		The foLanguageColumn is saved in a "text" column in the mysql. The
		format of the information is saved in the following struct.

		HEADER
		INDEX*n
		DATA*n

	* Format of header saved in a mysql column

		NAME						|	SIZE	|	Description
		-----------------------------------------------------------------------------------------
		validationcode	|	word	|	Defines that this is a foLanguageColumn. IE: 655536.
		version					|	byte	|	Current version of the language struct format.
		numOfLanguage		|	word	|	Number of different languages saved in the column.
		indexpos				|	dword	|	Position where the index are saved.
		datapos					|	dword	|	Position where the texts are saved.
		defaultLanguage	|	word	|	Two character representation of default language.

	* Format of index saved in a mysql column repeated for each language
	  The first language in the index part are the default language.

		NAME			|	SIZE	              |	Description
		-----------------------------------------------------------------------------------------
		language	|	foLDC_LANGUAGESIZE	|	Language code represented by X number of character.
		startpos	|	dword								|	Position where the text for this language is started.
							|											|	0 is datapos. Excluding [LANG TAG.
		length		|	dword								|	Length of the text. Excluding [LANG TAG.


	*	Format of language data saved in the mysql column.

		NAME	|	SIZE	|	Description
		-----------------------------------------------------------------------------------------
		data	|	blob	|	The language tag Ie [LANG=SE_]Sverige[/LANG]

	Requirments of system/OS platform.
 	---------------------------------
	* We are assuming that a short is 2 bytes and a long is 8 bytes.


	Examples
	--------
	The file Sql/testLanguge.sql demonstrates the functionality of foLanguage. At the
	same time as it tests that the functions works as they should on the mysql server.

	Remember to set use your database ("use foMaster").

***************************************************************************/

//
// Includes to build a mysql UDF.
//
#ifdef STANDARD
	#include <stdio.h>
	#include <string.h>
#else
	#include <my_global.h>		// myssql 4
	#include <my_sys.h>
#endif


#include <mysql.h>
#include <m_ctype.h>
#include <m_string.h>

//
// Used by mysld to find the symbbol in the .so file.
//
extern "C"
{
	my_bool setLanguage_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
	char *setLanguage(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);
	void setLanguage_deinit(UDF_INIT *initid);

	my_bool getLanguage_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
	char *getLanguage(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);
	void getLanguage_deinit(UDF_INIT *initid);
}

#include "FoString.h"
#include "FoDebug.cpp"
#include "FoUdfFunctions.h"

//
// foLanguageDataColumn
//
#define foLDC_LANGUAGESIZE 3	// Number of chars in the language code "SE_"

// Positions in the foLanguageDataColumn
#define foLDC_VALIDATION			 0	// Size 2 bytes
#define foLDC_VERSION					 2	// Size 1 byte
#define foLDC_NUMOFLANGUAGE		 3	// Size 2 bytes
#define foLDC_INDEXPOS				 5	// Size 4 bytes
#define foLDC_DATAPOS					 9	// Size 4 bytes
#define foLDC_DEFAULTLANGUAGE	13	// Size foLDC_LANGUAGESIZE

#define foLDC_HEADERSIZE			(13+foLDC_LANGUAGESIZE)
#define foLDC_INDEXSIZE				(8+foLDC_LANGUAGESIZE)	// 8 bytes + languagesize
#define foLDC_OPENTAGSIZE			(7+foLDC_LANGUAGESIZE)	// Number of bytes in [LANG=XX_]
#define foLDC_CLOSETAGSIZE		7	// Number of bytes in [/LANG]

// Positioner i index delen.
#define foLDC_INDEX_LANGUAGE	0	// Size languagesize bytes
#define foLDC_INDEX_STARTPOS	foLDC_LANGUAGESIZE	// Size 4 bytes
#define foLDC_INDEX_LENGTH		(4+foLDC_LANGUAGESIZE)	// Size 4 bytes
#define foLDC_INDEX_SIZE			(foLDC_LANGUAGESIZE+4+4) // Size of one index section

//
#define foLDC_VALIDATE_CODE 65280	// The validation code in the LanguageDataColumn should match this.

//
// Header delen av det som sparas i databasen.
//
class cfoLanguageHeader
{

public:
	unsigned short *validationCode;
	char  *version;
	unsigned short *numOfLanguages;
	U32   *indexPos;
	U32   *dataPos;
	char * defaultLanguage;	// Will be the size of foLDC_DEFAULTLANGUAGE

	void loadFromString(char const * const dbColumn)
	{
		validationCode = (unsigned short*)(dbColumn+foLDC_VALIDATION);
		version 			 = (char*)(dbColumn+foLDC_VERSION);
		numOfLanguages = (unsigned short*)(dbColumn+foLDC_NUMOFLANGUAGE);
		indexPos  		 = (U32*)(dbColumn+foLDC_INDEXPOS);
		dataPos 			 = (U32*)(dbColumn+foLDC_DATAPOS);
		defaultLanguage= (char*)(dbColumn+foLDC_DEFAULTLANGUAGE);
	};

	//

	inline void debugPrint()
	{
		#ifdef FO_LANGUGE_DEBUG_MODE
			debugEcho("    ---------- Header Begin -------------");
			debugEchoPrefix(); fprintf(stderr,	"      validationCode %d\n",  *validationCode);
			debugEchoPrefix(); fprintf(stderr,	"      version %d\n", 			  *version);
			debugEchoPrefix(); fprintf(stderr,	"      numOfLanguages %d\n",  *numOfLanguages);
			debugEchoPrefix(); fprintf(stderr,	"      indexPos %d\n", 			  *indexPos);
			debugEchoPrefix(); fprintf(stderr,	"      dataPos %d\n", 			  *dataPos);
			debugEchoPrefix(); fprintf(stderr,	"      defaultLanguage %s\n", defaultLanguage);
			debugEcho("    ---------- Header End -------------");
		#endif
	}
};

//

class cfoLanguageIndex
{
public:
	char *language; // Will be the size of foLDC_DEFAULTLANGUAGE
	U32 *startPos;
	U32 *length;

	void loadFromString(char const * const dbColumn, U32 indexPos, U32 indexNumber)
	{
		char const * currentIndexPos = (char*)(dbColumn+indexPos+(indexNumber*foLDC_INDEX_SIZE));

		language	= (char *)(currentIndexPos+foLDC_INDEX_LANGUAGE);
		startPos	= (U32 *)(currentIndexPos+foLDC_LANGUAGESIZE);
		length		= (U32 *)(currentIndexPos+foLDC_INDEX_LENGTH);
	};

	//

	inline void debugPrint()
	{
		#ifdef FO_LANGUGE_DEBUG_MODE
			debugEcho("    ---------- Index Begin -------------");
			debugEchoPrefix(); fprintf(stderr,	"      language %s\n", language);
			debugEchoPrefix(); fprintf(stderr,	"      startPos %d\n", *startPos);
			debugEchoPrefix(); fprintf(stderr,	"      length %d\n",   *length);
			debugEcho("    ---------- Index end -------------");
		#endif
	}
};

// Used to internaly in the functions store information
// about the language,
class cfoLanguage
{

public:
	unsigned short allocatedLanguageCount;	// Number of languages their is allocated space for.
	unsigned short languageCount;						// Number of languages
  char **language;												// The language length is foLDC_LANGUAGESIZE. 'SE_'
  char **data;
  U32 *dataLengths;
	U32 totalDataLength;	// Sum of the dataLengths
	char defaultLanguage[foLDC_LANGUAGESIZE];

	/**
	*	Sets standard values for the foLanguage struct
	*
	*	@access private
	*/
	cfoLanguage()
	{
		allocatedLanguageCount = 100;	// Number of languages to allocate space for.
		languageCount = 0;

		data = new char*[allocatedLanguageCount];
		dataLengths = new U32[allocatedLanguageCount];
		language = new char*[allocatedLanguageCount];
		totalDataLength = 0;
		defaultLanguage[0] = 0;	// No default language.
	}

	/**
	*	Clean up the foLanguage struct
	*
	*	@access private
	*/
	~cfoLanguage()
	{
		for(int i = 0; i < languageCount ; i++)
		{
			delete[] data[i];
			delete[] language[i];
		}

		allocatedLanguageCount = 0;
		languageCount = 0;

		delete[] data;
		delete[] dataLengths;
		delete[] language;
		totalDataLength = 0;
		defaultLanguage[0] = 0;
	}

	/**
	*	Load language data from the database column.
	*
	*	@access private
	*/
	void loadLanguageData(char const * const dbColumn)
	{
		debugEcho("  ------------- loadLanguageData Begin --------------");
		debugPrint();

		//
		cfoLanguageHeader headerData;
		headerData.loadFromString(dbColumn);
		headerData.debugPrint();

		// validationCode
		if (*headerData.validationCode == foLDC_VALIDATE_CODE)
		{
			// Version
			if (*headerData.version == 1)
			{
				if (languageCount+*headerData.numOfLanguages <= allocatedLanguageCount)
				{
					// DefaultLanguage
					memcpy(defaultLanguage, headerData.defaultLanguage, foLDC_LANGUAGESIZE);

					cfoLanguageIndex indexData;
					for (int i=0;i<*headerData.numOfLanguages;i++)
					{
						indexData.loadFromString(dbColumn, *headerData.indexPos, i);
						indexData.debugPrint();

						// dbColumn is the full database column in a string.
						// dataPos is where the position where the string data section begins.
						// indexdata.startPos is the pos where the language string begings in the data section.
						setLanguageArr
						(
							indexData.language,
							(char *)(dbColumn+*headerData.dataPos+*indexData.startPos),
							*indexData.length
						);
					}
				}
				else
				{
					fprintf
					(
						stderr,
						"Error: foFunction-fo_language-loadLanguageData Too many languages to allocate, current %d, max %d\n",
						languageCount+*headerData.numOfLanguages,
						allocatedLanguageCount
					);
				}
			}
		}

		debugEcho("  ------------- loadLanguageData End --------------");
	}

	/**
	*	Adds a language text to the foLanguage struct array.
	*
	*
	*	@param 	UDF_FOLANGUAGE* 	foLanguage
	*														Struct including all the languge data.
	*
	*	@param 	char* 						lanugage
	*														The language code for the text entered in text. ie. SE_
	*
	*	@param 	char* 						text
	*														The text to add to the foLanguage array.
	*
	*	@param 	number 						$length
	*														Length of the text to add, excluding the language tag ([LANG=SE_]).
	*
	*	@access private
	*/
	void setLanguageArr(char const * const languageCode, char const * const text, int const length)
	{
		// First delete if any earlier language exist,
		// and then replace that array position.
		_deleteLanguageArr(languageCode);

		if (languageCount >= allocatedLanguageCount)
		{
			fprintf
			(
				stderr,
				"Error: foFunction-fo_language-setLanguageArr Too many languages allocated, current %d, max %d\n",
				languageCount,
				allocatedLanguageCount
			);

			return;
		}

		// Any text do add?
		if (length>0)
		{
			// Create a new array position.
			int index = languageCount;

			// Language
			language[index] = new char[foLDC_LANGUAGESIZE];
			memcpy(language[index], languageCode, foLDC_LANGUAGESIZE);

			// Data
			dataLengths[index] = length;
			data[index] = new char[length];
			memcpy(data[index], text, length);
			totalDataLength += length;

			languageCount++;
		}
	}

	/**
	*	Parseing the newText for language tags, and inserts the result in the foLanguage struct
	*
	* @params	foLanguage 	Language
	*											array struct.
	*
	* @params  char				newText
	*											Example [LANG=SE_]Sverige[/LANG][LANG=UK_]Sweden[/LANG]
	*
	* @params	uint				length
	*											Length of newText Example 47
	*	@access private
	*/
	void parseLanguageData(char const * const newText, U32 length)
	{
		char * newTextN = new char[length+1];
		memcpy(newTextN, newText, length);
		newTextN[length] = 0;

		char * newTextNRead = newTextN;
		char * openTagPos, *closeTagPos, * language;

		openTagPos = strstr(newTextNRead, "[LANG=");
		while(openTagPos != NULL)
		{
			// Points to the language code.
			newTextNRead = openTagPos+(foLDC_OPENTAGSIZE-foLDC_LANGUAGESIZE)-1;
			language = newTextNRead;
			newTextNRead +=foLDC_LANGUAGESIZE;

			if (*(newTextNRead) == ']')
			{
				newTextNRead +=1;
				closeTagPos = strstr(newTextNRead, "[/LANG]");
			}
			else
			{
				closeTagPos = NULL;
			}

			openTagPos = strstr(newTextNRead, "[LANG=");

			// Don't setLanguage for language de_
			// in this string "[lang=de_]text[lang=se_]text[/lang]". se is however valid for parsing.
			if (openTagPos == NULL || (closeTagPos != NULL && closeTagPos < openTagPos))
			{
				setLanguageArr(language, newTextNRead, closeTagPos-newTextNRead);
			}
		}

		delete[] newTextN;
	}

	/**
	*	Creates a new foLanguageDataColumn to be saved in the database.
	*
	*	@access private
	*/
	void fillLanguageData(UDF_INIT *initid, unsigned long *length)
	{
		debugEcho("  ------------- fillLanguageData Begin --------------");
		debugPrint();

		if (languageCount == 0)
		{
			*length = 0;
		}
		else
		{
			U32 dataStartPos = foLDC_HEADERSIZE+(languageCount*foLDC_INDEXSIZE);
			*length = dataStartPos+totalDataLength+(languageCount*(foLDC_OPENTAGSIZE+foLDC_CLOSETAGSIZE));

			cfoString * foString = (cfoString*)initid->ptr;
			foString->allocateBuffer(*length);

			//
			// buildHeader
			//
			foString->modString(foLDC_VALIDATION, (unsigned short)foLDC_VALIDATE_CODE);
			foString->modString(foLDC_VERSION, (char)1);
			foString->modString(foLDC_NUMOFLANGUAGE, (unsigned short)languageCount);

			foString->modString(foLDC_INDEXPOS, (U32)foLDC_HEADERSIZE);

			foString->modString(foLDC_DATAPOS, (U32)dataStartPos);

			//
			// build index and data.
			//
			int indexWPos = foLDC_HEADERSIZE;
			int dataWPos = dataStartPos;
			int langPos = -1;

			if (defaultLanguage[0] != 0)
			{
				langPos = _getLanguagePosition(defaultLanguage);

				// Language didnt exist.
				if (langPos == -1)
					defaultLanguage[0] = 0;
				else
					_writeLanguageData(foString, &indexWPos, &dataWPos, dataStartPos, defaultLanguage, data[langPos], dataLengths[langPos]);
			}

			for(int i=0;i<languageCount;i++)
			{
				if (langPos != i)
				{
					_writeLanguageData(foString, &indexWPos, &dataWPos, dataStartPos,  language[i], data[i], dataLengths[i]);

					// If no default language are set by the user,
					// use the first language in the foLanguage index as default language.
					if (defaultLanguage[0] == 0)
						memcpy(defaultLanguage, language[i], foLDC_LANGUAGESIZE);
				}
			}

			// defaultLanguage
			foString->modString(foLDC_DEFAULTLANGUAGE, defaultLanguage, foLDC_LANGUAGESIZE);
		}

		debugEcho("  ------------- fillLanguageData End --------------");
	}

	/**
	*
	*	@access private
	*/
	inline void debugPrint()
	{
		#ifdef FO_LANGUGE_DEBUG_MODE
			debugEcho("    ---------- foLanguage Begin -------------");
			debugEchoPrefix(); fprintf(stderr,	"      allocatedLanguageCount: %d\n", allocatedLanguageCount);
			debugEchoPrefix(); fprintf(stderr,	"      languageCount: %d\n", languageCount);

			debugEchoPrefix(); fprintf(stderr,	"      languages\n");
			for(int i=0;i<languageCount;i++)
			{
				debugEchoPrefix(); fprintf(stderr,	"        language: %s\n", language[i]);
				debugEchoPrefix(); fprintf(stderr,	"        data: %s\n", data[i]);
				debugEchoPrefix(); fprintf(stderr,	"        dataLengths: %d\n", dataLengths[i]);
				debugEcho("");
			}
			debugEchoPrefix(); fprintf(stderr,	"      totalDataLength: %d\n", totalDataLength);

			debugEchoPrefix(); fprintf(stderr,	"      defaultLanguage: %s\n", defaultLanguage);

			debugEcho("    ---------- foLanguage End -------------");
		#endif
	}

private:



	/**
	*	Delete language from the data struct foLanguage.
	*
	*	@access private
	*/
	int _deleteLanguageArr(char const * const languageCode)
	{
		// Letar reda på den language array som ska tas bort.
		for(int i=0;i<languageCount;i++)
		{
			if (strncmp(language[i], languageCode, foLDC_LANGUAGESIZE) == 0)
			{
				totalDataLength -= dataLengths[i];
				delete[] language[i];
				delete[] data[i];

				// Reindex all the languages, to remove the "unused"
				// areas in the array.
				languageCount--;
				for(unsigned short j=i;j<languageCount;j++)
				{
					data[j] = data[j+1];
					dataLengths[j] = dataLengths[j+1];
					language[j] = language[j+1];
				}
				return i;
			}
		}

		return -1;
	}

	/**
	*	Get the array index for a language.
	*
	*	@access private
	*/
	int _getLanguagePosition(char const * const languageCode)
	{
		for(int i=0;i<languageCount;i++)
		{
			if (strncmp(language[i], languageCode, foLDC_LANGUAGESIZE) == 0)
				return i;
		}

		return -1;
	}

	/**
	*	Write the index and data information for a language to the string that
	* will be save as a foLanguageDataColumn to in the database.
	*
	*	@access private
	*/
	void _writeLanguageData
	(
			cfoString * foString, int * indexWPos, int * dataWPos, int const dataStartPos,
			char const * const language, char const * const text, U32 const length
	)
	{
		// IndexArea - language
		foString->modString(indexWPos, language, foLDC_LANGUAGESIZE);

		// IndexArea - startPos
		foString->modString(indexWPos, (U32)(*dataWPos+foLDC_OPENTAGSIZE-dataStartPos));

		// IndexArea - length
		foString->modString(indexWPos, length);

		// DataArea - Start tag [LANG=XX]
		foString->modString(dataWPos, "[LANG=", 6);
		foString->modString(dataWPos, language, foLDC_LANGUAGESIZE);
		foString->modString(dataWPos, "]", 1);

		// DataArea - Data
		foString->modString(dataWPos, text, length);

		// DataArea - End tag [/LANG]
		foString->modString(dataWPos, "[/LANG]", 7);

		// Debug information
		#ifdef FO_LANGUGE_DEBUG_MODE
			debugEcho("    ------------- writeLanguageData Begin --------------");
			debugEchoPrefix(); fprintf(stderr,	"      language %s\n",  language);
			debugEchoPrefix(); fprintf(stderr,	"      startPos %d\n",  (U32)(*dataWPos+foLDC_OPENTAGSIZE-dataStartPos));
			debugEchoPrefix(); fprintf(stderr,	"      length %d\n",  length);
			debugEchoPrefix(); fprintf(stderr,	"      text %s\n",  text);
			debugEcho("    ------------- writeLanguageData End --------------");
		#endif
	}

};

//

/////////////////////////////////////////////////////////
//
// PRIVATE:
//
/////////////////////////////////////////////////////////

// Stoppar in en ny språk text i foLanguage struct arrayen,
// men parsar först newText strängen.
//

/**
*	Modify: initid->ptr, initid->max_length, textLength
*
*	@access private
*/
void createReturnString
(
		UDF_INIT * const initid, char const * const dataAreaPos,
		U32 const * const textStartPos, U32 &textLength,
		char const * const firstLanguage, char const * const foundLanguage
)
{
	cfoString * foString = (cfoString*)initid->ptr;

	if (textStartPos == NULL)
	{
		foString->setString(dataAreaPos, textLength);
	}
	else
	{
		U32 orginalTextLength = textLength;

		if (foundLanguage != NULL && textStartPos != NULL)
			textLength += (foLDC_OPENTAGSIZE+foLDC_CLOSETAGSIZE)*2;

		foString->allocateBuffer(textLength);
		int initidWritePos = 0;

		if (foundLanguage != NULL)
		{
			foString->modString(&initidWritePos, "[LANG=", 6);
			foString->modString(&initidWritePos, firstLanguage, foLDC_LANGUAGESIZE);
			foString->modString(&initidWritePos, "]", 1);
			foString->modString(&initidWritePos, "[/LANG]", foLDC_CLOSETAGSIZE);

			foString->modString(&initidWritePos, "[LANG=", 6);
			foString->modString(&initidWritePos, foundLanguage, foLDC_LANGUAGESIZE);
			foString->modString(&initidWritePos, "]", 1);
		}

		foString->modString(&initidWritePos, dataAreaPos+*textStartPos, orginalTextLength);

		if (foundLanguage != NULL)
		{
			foString->modString(&initidWritePos, "[/LANG]", foLDC_CLOSETAGSIZE);
		}
	}
}


/////////////////////////////////////////////////////////
//
// PUBLIC:
//
/////////////////////////////////////////////////////////

/***************************************************************************
	char* = setLanguage(dbColumn, newText, language, defaultLanguage)
	***********
	Concatenates a foLanguageColumn with a new language.

	Argument format
		[0]dbColumn					- "[LANGINDEX][LANG=SE]Hej världen[/LANG][LANG=UK]Hello world[/LANG]"
													Can be null
		[1]newText					- "Moi mukulat"
													Can be null, will delete the language indicated by [2]language.
		[2]language					- "FI"
													If null, the [1]newText field must include the [LANG=xx] tag.
		[3]defaultLanguage	- "UK" can be null

	Return
		The concated string with the index.
		[...LANGINDEX....][LANG=UK]Hello world[/LANG][LANG=SE]Hej världen[/LANG][LANG=FI]Moi mukulat[/LANG]

	Example
		select setLanguage(NULL, "Moi mukulat]", "FI", "UK") as moo;

***************************************************************************/
#define ARG_SET_LANGUAGE_DBCOLUMN 		   	0
#define ARG_SET_LANGUAGE_NEWTEXT 					1
#define ARG_SET_LANGUAGE_LANGUAGE       	2
#define ARG_SET_LANGUAGE_DEFAULTLANGUAGE	3

#define ARG_SET_LANGUAGE_COUNT	4

/**
*
*	@access public
*/
my_bool setLanguage_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	debugBegin(true, false);
	debugEcho("------------- foFunction -> SetLanguage Begin --------------");

  if (args->arg_count < ARG_SET_LANGUAGE_COUNT)
  {
		// The message can be a maximum of MYSQL_ERRMSG_SIZE bytes long.
    strcpy(message,"usage: char* = setLanguage(dbColumn, newText, language, defaultLanguage)");
    return 1;
  }
	else
	{
		args->arg_type[ARG_SET_LANGUAGE_DBCOLUMN] 				= STRING_RESULT;
		args->arg_type[ARG_SET_LANGUAGE_NEWTEXT] 					= STRING_RESULT;
		args->arg_type[ARG_SET_LANGUAGE_LANGUAGE]       	= STRING_RESULT;
		args->arg_type[ARG_SET_LANGUAGE_DEFAULTLANGUAGE] 	= STRING_RESULT;

		initid->decimals = 0;
		initid->maybe_null = 1;
		initid->max_length = 16000000; // Can return a maximum of this.
		initid->ptr = (char *)new cfoString;	// Allocate 255 byte to begin with.

		return 0;
	}
}

/**
*
*	@access public
*/
void setLanguage_deinit(UDF_INIT *initid)
{
	delete initid->ptr;
	debugEcho("------------- foFunction -> SetLanguage End --------------");
}

/**
*
*	@access public
*/
char *setLanguage(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	debugBegin(false, true);
	cfoLanguage foLanguage;

	// Load the current language information, to add new language
	// data too. Can be NULL
	if (!emptyARG(args, ARG_SET_LANGUAGE_DBCOLUMN))
		foLanguage.loadLanguageData(args->args[ARG_SET_LANGUAGE_DBCOLUMN]);

	foLanguage.debugPrint();

	// If LANGUAGE is set, the NEWTEXT will be added with that language
	// to the foLanguageColumn.
	if (!emptyARG(args, ARG_SET_LANGUAGE_LANGUAGE))
	{
		foLanguage.setLanguageArr(args->args[ARG_SET_LANGUAGE_LANGUAGE], args->args[ARG_SET_LANGUAGE_NEWTEXT], args->lengths[ARG_SET_LANGUAGE_NEWTEXT]);
	}

	// If not the NEWTEXT is supposed to be in the FareofficeTag format [LANG=SE_]...[/LANG].
	else
	{
		foLanguage.parseLanguageData(args->args[ARG_SET_LANGUAGE_NEWTEXT], args->lengths[ARG_SET_LANGUAGE_NEWTEXT]);
	}

	if (!emptyARG(args, ARG_SET_LANGUAGE_DEFAULTLANGUAGE))
		memcpy(foLanguage.defaultLanguage, args->args[ARG_SET_LANGUAGE_DEFAULTLANGUAGE], foLDC_LANGUAGESIZE);

	foLanguage.fillLanguageData(initid, length);

	cfoString * foString = (cfoString*)initid->ptr;

	if (*length == 0)
	{
		*is_null = true;
		return NULL;
	}
	else
	{
		return foString->getString();
	}
}

/***************************************************************************
	text = getLanguage(dbColumn, firstLang, secondLang, viewMode);
	***********
	Geta language from a foLanguageColumn.

	Argument format
		[0]dbColumn			- "[LANGINDEX][LANG=SE]Hej världen[/LANG][LANG=UK]Hello world[/LANG]"
										 	Can be null.
		[1]firstLang		- "SE"	Look for this lang first.
											If null, return all the languages with tags.
		[2]secondLang		- "FI"	If not firstLang are found, try to find this.
											Can be null, won't search for this lang if null
		[3]viewMode			- 0 - Only firstLang and secondLang
											1 - If none of the above are found and this is true
													look for the first langouage in the foLanguageColumn.
													return NULL if false.
											2 - Like mode 0, but if only secondLang is found return with tags [LANG=SE][/LANG][LANG=UK]English[/LANG]
											3 - Like mode 1, but if onlye secondLang or defaultLang is found return with tags.

	Return
		The wanted string, or with tags ie. [LANG=SE]COW[/LANG]

	Example
		select setLanguage(rule.rule_name, "FI", "UK", 1) as rule_name;

***************************************************************************/
#define ARG_GET_LANGUAGE_DBCOLUMN 		0
#define ARG_GET_LANGUAGE_FIRSTLANG 		1
#define ARG_GET_LANGUAGE_SECONDLANG   2
#define ARG_GET_LANGUAGE_VIEWMODE			3

#define ARG_GET_LANGUAGE_COUNT	4

/**
*
*	@access public
*/
my_bool getLanguage_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	checkVariableTypeCompatiblity();		
	
	debugBegin(true, false);
	debugEcho("------------- foFunction -> GetLanguage Begin --------------");

  if (args->arg_count < ARG_GET_LANGUAGE_COUNT)
  {
		// The message can be a maximum of MYSQL_ERRMSG_SIZE bytes long.
    strcpy(message,"usage: char* = getLanguage(dbColumn, firstLanguage, secondLanguage, defaultLanguage)");
    return 1;
  }
	else
	{
		args->arg_type[ARG_GET_LANGUAGE_DBCOLUMN] 		= STRING_RESULT;
		args->arg_type[ARG_GET_LANGUAGE_FIRSTLANG] 		= STRING_RESULT;
		args->arg_type[ARG_GET_LANGUAGE_SECONDLANG]   = STRING_RESULT;
		args->arg_type[ARG_GET_LANGUAGE_VIEWMODE]			= INT_RESULT;

		initid->decimals = 0;
		initid->maybe_null = 1;
		initid->max_length = 16000000; // Can return a maximum of this.
		initid->ptr = (char *)new cfoString;	// Allocate 255 byte to begin with.

		return 0;
	}
}

/**
*
*	@access public
*/
void getLanguage_deinit(UDF_INIT *initid)
{
	delete initid->ptr;
	debugEcho("------------- foFunction -> GetLanguage End --------------");
}

/**
*
*	@access public
*/
char *getLanguage(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	debugBegin(false, true);
	if (!emptyARG(args, ARG_GET_LANGUAGE_DBCOLUMN))
	{
		//
		// Get arguments from the function call.
		//
		char *dbColumn = args->args[ARG_GET_LANGUAGE_DBCOLUMN];

		unsigned short * validationCode = (unsigned short*)(dbColumn+foLDC_VALIDATION);
		if (*validationCode == foLDC_VALIDATE_CODE)
		{
			char *firstLanguage  = NULL;
			char *secondLanguage = NULL;

			if (!emptyARG(args, ARG_GET_LANGUAGE_FIRSTLANG))
				firstLanguage = args->args[ARG_GET_LANGUAGE_FIRSTLANG];

			if (!emptyARG(args, ARG_GET_LANGUAGE_SECONDLANG))
				secondLanguage = args->args[ARG_GET_LANGUAGE_SECONDLANG];

			char *defaultLanguage = NULL;

			// Should we use the default value from the foLanuageColumn.
			if (*args->args[ARG_GET_LANGUAGE_VIEWMODE] == 1 || *args->args[ARG_GET_LANGUAGE_VIEWMODE] == 3)
				defaultLanguage = dbColumn+foLDC_DEFAULTLANGUAGE;

			// Debug code		
			#ifndef FO_LANGUGE_DEBUG_MODE
				cfoLanguageHeader oLangaugeHeader;
				oLangaugeHeader .loadFromString(dbColumn);
				oLangaugeHeader.debugPrint();
				debugEchoEX("ValidationCode=", 	validationCode);
				debugEchoEX("firstLanguage=", 	firstLanguage);
				debugEchoEX("secondLanguage=", 	secondLanguage);
				debugEchoEX("defaultLanguage=", 	defaultLanguage);				
			#endif

			// Set easier to use variables
			U32 * dbColumnDataPos = (U32*)(dbColumn+foLDC_DATAPOS);
			U32 * dbColumnIndexPos = (U32 *)(dbColumn+foLDC_INDEXPOS);

			U32 * textStartPos = NULL;
			U32 textLength = 0;
			bool languageFound = false;
			char * foundLanguagePos = NULL;

			//
			// Read the data
			//
			if (firstLanguage == NULL)
			{
				textStartPos = NULL;
				textLength = args->lengths[ARG_GET_LANGUAGE_DBCOLUMN]-*dbColumnDataPos;
				languageFound = true;
			}
			else
			{
				char * indexReadPos = dbColumn+*dbColumnIndexPos;
				for (int i=0;i<(char)(dbColumn[foLDC_NUMOFLANGUAGE]);i++)
				{
					// indexReadPos points to language
					if (strncmp(indexReadPos, firstLanguage, foLDC_LANGUAGESIZE) == 0)
					{
						textStartPos = (U32*)(indexReadPos+foLDC_LANGUAGESIZE);
						textLength = *((U32*)(indexReadPos+foLDC_LANGUAGESIZE+4));
						foundLanguagePos = NULL;
						languageFound = true;
						break;
					}
					else if ((secondLanguage != NULL && strncmp(indexReadPos, secondLanguage, foLDC_LANGUAGESIZE) == 0) ||
									 (defaultLanguage != NULL && languageFound == false && strncmp(indexReadPos, defaultLanguage, foLDC_LANGUAGESIZE) == 0))
					{
						textStartPos = (U32*)(indexReadPos+foLDC_LANGUAGESIZE);
						textLength = *((U32*)(indexReadPos+foLDC_LANGUAGESIZE+4));
						languageFound = true;

						if (*args->args[ARG_GET_LANGUAGE_VIEWMODE] == 2 || *args->args[ARG_GET_LANGUAGE_VIEWMODE] == 3)
							foundLanguagePos = indexReadPos;
					}

					// Jump to next index "row"
					indexReadPos+=foLDC_INDEXSIZE;
				}
			}

			if (languageFound == true)
				createReturnString(initid, dbColumn+*dbColumnDataPos, textStartPos, textLength, firstLanguage, foundLanguagePos);

			*length = textLength;
		}
		else
		{
			*length = args->lengths[ARG_GET_LANGUAGE_DBCOLUMN];
			U32 textLength = *length;
			createReturnString(initid, dbColumn, 0, textLength, NULL, NULL);
		}
	}
	else
	{
		*length = 0;
	}

	if (*length == 0)
	{
		*is_null = true;
		return NULL;
	}
	else
	{
		cfoString * foString = (cfoString*)initid->ptr;
		return foString->getString();
	}
}
