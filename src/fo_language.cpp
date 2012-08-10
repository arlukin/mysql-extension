//
// Includes to build a mysql UDF.
//
/*
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
*/

#include <stdio.h>
#include <string.h>

#include "fo_string.h"
#include "fo_debug.cpp"
#include "fo_language.hpp"


void cfoLanguageHeader::load_from_string(char const * const dbColumn)
{
	validationCode  = (unsigned short*)(dbColumn+foLDC_VALIDATION);
	version 	    = (char*)(dbColumn+foLDC_VERSION);
	numOfLanguages  = (unsigned short*)(dbColumn+foLDC_NUMOFLANGUAGE);
	indexPos  	    = (U32*)(dbColumn+foLDC_INDEXPOS);
	dataPos 	    = (U32*)(dbColumn+foLDC_DATAPOS);
	default_language = (char*)(dbColumn+foLDC_default_language);
};


void cfoLanguageHeader::debug_print()
{
	#ifdef FO_LANGUGE_DEBUG_MODE
		debugEcho("    ---------- Header Begin -------------");
		debugEchoPrefix(); fprintf(stderr,	"      validationCode %d\n",  *validationCode);
		debugEchoPrefix(); fprintf(stderr,	"      version %d\n", 			  *version);
		debugEchoPrefix(); fprintf(stderr,	"      numOfLanguages %d\n",  *numOfLanguages);
		debugEchoPrefix(); fprintf(stderr,	"      indexPos %d\n", 			  *indexPos);
		debugEchoPrefix(); fprintf(stderr,	"      dataPos %d\n", 			  *dataPos);
		debugEchoPrefix(); fprintf(stderr,	"      default_language %s\n", default_language);
		debugEcho("    ---------- Header End -------------");
	#endif
}


void cfoLanguageIndex::load_from_string(char const * const dbColumn, U32 indexPos, U32 indexNumber)
{
	char const * currentIndexPos = (char*)(dbColumn+indexPos+(indexNumber*foLDC_INDEX_SIZE));

	language = (char *)(currentIndexPos+foLDC_INDEX_LANGUAGE);
	startPos = (U32 *)(currentIndexPos+foLDC_LANGUAGESIZE);
	length	 = (U32 *)(currentIndexPos+foLDC_INDEX_LENGTH);
};


void cfoLanguageIndex::debug_print()
{
	#ifdef FO_LANGUGE_DEBUG_MODE
		debugEcho("    ---------- Index Begin -------------");
		debugEchoPrefix(); fprintf(stderr,	"      language %s\n", language);
		debugEchoPrefix(); fprintf(stderr,	"      startPos %d\n", *startPos);
		debugEchoPrefix(); fprintf(stderr,	"      length %d\n",   *length);
		debugEcho("    ---------- Index end -------------");
	#endif
}


cfoLanguage::cfoLanguage()
{
	allocatedLanguageCount = 100;	// Number of languages to allocate space for.
	languageCount = 0;

	data = new char*[allocatedLanguageCount];
	dataLengths = new U32[allocatedLanguageCount];
	language = new char*[allocatedLanguageCount];
	totalDataLength = 0;
	default_language[0] = 0;	// No default language.
}


cfoLanguage::~cfoLanguage()
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
	default_language[0] = 0;
}


void cfoLanguage::load_language_data(char const * const dbColumn)
{
	debugEcho("  ------------- load_language_data Begin --------------");
	debug_print();

	//
	cfoLanguageHeader headerData;
	headerData.load_from_string(dbColumn);
	headerData.debug_print();

	// validationCode
	if (*headerData.validationCode == foLDC_VALIDATE_CODE)
	{
		// Version
		if (*headerData.version == 1)
		{
			if (languageCount+*headerData.numOfLanguages <= allocatedLanguageCount)
			{
				// default_language
				memcpy(default_language, headerData.default_language, foLDC_LANGUAGESIZE);

				cfoLanguageIndex indexData;
				for (int i=0;i<*headerData.numOfLanguages;i++)
				{
					indexData.load_from_string(dbColumn, *headerData.indexPos, i);
					indexData.debug_print();

					// dbColumn is the full database column in a string.
					// dataPos is where the position where the string data section begins.
					// indexdata.startPos is the pos where the language string begings in the data section.
					set_language_arr
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
					"Error: foFunction-fo_language-load_language_data Too many languages to allocate, current %d, max %d\n",
					languageCount+*headerData.numOfLanguages,
					allocatedLanguageCount
				);
			}
		}
	}

	debugEcho("  ------------- load_language_data End --------------");
}


void cfoLanguage::set_language_arr(char const * const languageCode, char const * const text, int const length)
{
	// First delete if any earlier language exist,
	// and then replace that array position.
	_delete_language_arr(languageCode);

	if (languageCount >= allocatedLanguageCount)
	{
		fprintf
		(
			stderr,
			"Error: foFunction-fo_language-set_language_arr Too many languages allocated, current %d, max %d\n",
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


void cfoLanguage::parse_language_data(char const * const newText, U32 length)
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
			set_language_arr(language, newTextNRead, closeTagPos-newTextNRead);
		}
	}

	delete[] newTextN;
}


void cfoLanguage::fill_language_data(void *str_ptr, unsigned long *length)
{
	debugEcho("  ------------- fill_language_data Begin --------------");
	debug_print();

	if (languageCount == 0)
	{
		*length = 0;
	}
	else
	{
		U32 dataStartPos = foLDC_HEADERSIZE+(languageCount*foLDC_INDEXSIZE);
		*length = dataStartPos+totalDataLength+(languageCount*(foLDC_OPENTAGSIZE+foLDC_CLOSETAGSIZE));

		FOString * foString = (FOString*)str_ptr;
		foString->allocate_buffer(*length);

		//
		// buildHeader
		//
		foString->overwrite(foLDC_VALIDATION, (unsigned short)foLDC_VALIDATE_CODE);
		foString->overwrite(foLDC_VERSION, (char)1);
		foString->overwrite(foLDC_NUMOFLANGUAGE, (unsigned short)languageCount);

		foString->overwrite(foLDC_INDEXPOS, (U32)foLDC_HEADERSIZE);

		foString->overwrite(foLDC_DATAPOS, (U32)dataStartPos);

		//
		// build index and data.
		//
		int indexWPos = foLDC_HEADERSIZE;
		int dataWPos = dataStartPos;
		int langPos = -1;

		if (default_language[0] != 0)
		{
			langPos = _get_language_position(default_language);

			// Language didnt exist.
			if (langPos == -1)
				default_language[0] = 0;
			else
				_write_language_data(foString, &indexWPos, &dataWPos, dataStartPos, default_language, data[langPos], dataLengths[langPos]);
		}

		for(int i=0;i<languageCount;i++)
		{
			if (langPos != i)
			{
				_write_language_data(foString, &indexWPos, &dataWPos, dataStartPos,  language[i], data[i], dataLengths[i]);

				// If no default language are set by the user,
				// use the first language in the foLanguage index as default language.
				if (default_language[0] == 0)
					memcpy(default_language, language[i], foLDC_LANGUAGESIZE);
			}
		}

		// default_language
		foString->overwrite(foLDC_default_language, default_language, foLDC_LANGUAGESIZE);
	}

	debugEcho("  ------------- fill_language_data End --------------");
}


void cfoLanguage::debug_print()
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

		debugEchoPrefix(); fprintf(stderr,	"      default_language: %s\n", default_language);

		debugEcho("    ---------- foLanguage End -------------");
	#endif
}


int cfoLanguage::_delete_language_arr(char const * const languageCode)
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


int cfoLanguage::_get_language_position(char const * const languageCode)
{
	for(int i=0;i<languageCount;i++)
	{
		if (strncmp(language[i], languageCode, foLDC_LANGUAGESIZE) == 0)
			return i;
	}

	return -1;
}


void cfoLanguage::_write_language_data
(
		FOString * foString, int * indexWPos, int * dataWPos, int const dataStartPos,
		char const * const language, char const * const text, U32 const length
)
{
	// IndexArea - language
	foString->overwrite(indexWPos, language, foLDC_LANGUAGESIZE);

	// IndexArea - startPos
	foString->overwrite(indexWPos, (U32)(*dataWPos+foLDC_OPENTAGSIZE-dataStartPos));

	// IndexArea - length
	foString->overwrite(indexWPos, length);

	// DataArea - Start tag [LANG=XX]
	foString->overwrite(dataWPos, "[LANG=", 6);
	foString->overwrite(dataWPos, language, foLDC_LANGUAGESIZE);
	foString->overwrite(dataWPos, "]", 1);

	// DataArea - Data
	foString->overwrite(dataWPos, text, length);

	// DataArea - End tag [/LANG]
	foString->overwrite(dataWPos, "[/LANG]", 7);

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
