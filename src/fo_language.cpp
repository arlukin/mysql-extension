/*

The set language logic.

__author__     = "daniel.lindh@cybercow.se"
__copyright__  = "Copyright 2012, Fareoffice CRS AB"
__maintainer__ = "Daniel Lindh"
__version__    = "1.0.0"
__status__     = "Production"

*/


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


#include <stdio.h>
#include <string.h>

#include <fo_debug.hpp>
#include <fo_language.hpp>


void cfoLanguageHeader::load_from_string(char const * const db_column)
{
	validation_code  = (unsigned short*)(db_column+foLDC_VALIDATION);
	version 	     = (char*)(db_column+foLDC_VERSION);
	num_of_languages = (unsigned short*)(db_column+foLDC_NUMOFLANGUAGE);
	index_pos  	     = (U32*)(db_column+foLDC_index_pos);
	data_pos 	     = (U32*)(db_column+foLDC_data_pos);
	default_language = (char*)(db_column+foLDC_default_language);
};


void cfoLanguageHeader::debug_print()
{
	#ifdef FO_LANGUGE_DEBUG_MODE
		debug_echo("    ---------- Header Begin -------------");
		debug_echo_prefix(); fprintf(stderr,	"      validation_code %d\n",  *validation_code);
		debug_echo_prefix(); fprintf(stderr,	"      version %d\n", 		   *version);
		debug_echo_prefix(); fprintf(stderr,	"      num_of_languages %d\n", *num_of_languages);
		debug_echo_prefix(); fprintf(stderr,	"      index_pos %d\n", 	   *index_pos);
		debug_echo_prefix(); fprintf(stderr,	"      data_pos %d\n", 		   *data_pos);
		debug_echo_prefix(); fprintf(stderr,	"      default_language %s\n", default_language);
		debug_echo("    ---------- Header End -------------");
	#endif
}


void cfoLanguageIndex::load_from_string(char const * const db_column, U32 const index_pos, U32 const index_number)
{
	char const * current_index_pos = (char*)(db_column+index_pos+(index_number*foLDC_INDEX_SIZE));

	language = (char *)(current_index_pos+foLDC_INDEX_LANGUAGE);
	start_pos = (U32 *)(current_index_pos+foLDC_LANGUAGESIZE);
	length	 = (U32 *)(current_index_pos+foLDC_INDEX_LENGTH);
};


void cfoLanguageIndex::debug_print()
{
	#ifdef FO_LANGUGE_DEBUG_MODE
		debug_echo("    ---------- Index Begin -------------");
		debug_echo_prefix(); fprintf(stderr,	"      language %s\n", language);
		debug_echo_prefix(); fprintf(stderr,	"      start_pos %d\n", *start_pos);
		debug_echo_prefix(); fprintf(stderr,	"      length %d\n",   *length);
		debug_echo("    ---------- Index end -------------");
	#endif
}


cfoLanguage::cfoLanguage()
{
	allocated_language_count = 100;	// Number of languages to allocate space for.
	language_count = 0;

	data = new char*[allocated_language_count];
	data_lengths = new U32[allocated_language_count];
	language = new char*[allocated_language_count];
	total_data_length = 0;
	default_language[0] = 0;	// No default language.
}


cfoLanguage::~cfoLanguage()
{
	for(int i = 0; i < language_count ; i++)
	{
		delete[] data[i];
		delete[] language[i];
	}

	allocated_language_count = 0;
	language_count = 0;

	delete[] data;
	delete[] data_lengths;
	delete[] language;
	total_data_length = 0;
	default_language[0] = 0;
}


void cfoLanguage::load_language_data(char const * const db_column)
{
	debug_echo("  ------------- load_language_data Begin --------------");
	debug_print();

	//
	cfoLanguageHeader header_data;
	header_data.load_from_string(db_column);
	header_data.debug_print();

	// validation_code
	if (*header_data.validation_code == foLDC_VALIDATE_CODE)
	{
		// Version
		if (*header_data.version == 1)
		{
			if (language_count+*header_data.num_of_languages <= allocated_language_count)
			{
				// default_language
				memcpy(default_language, header_data.default_language, foLDC_LANGUAGESIZE);

				cfoLanguageIndex index_data;
				for (int i=0;i<*header_data.num_of_languages;i++)
				{
					index_data.load_from_string(db_column, *header_data.index_pos, i);
					index_data.debug_print();

					// db_column is the full database column in a string.
					// data_pos is where the position where the string data section begins.
					// index_data.start_pos is the pos where the language string begings in the data section.
					set_language_arr
					(
						index_data.language,
						(char *)(db_column+*header_data.data_pos+*index_data.start_pos),
						*index_data.length
					);
				}
			}
			else
			{
				fprintf
				(
					stderr,
					"Error: foFunction-fo_language-load_language_data Too many languages to allocate, current %d, max %d\n",
					language_count+*header_data.num_of_languages,
					allocated_language_count
				);
			}
		}
	}

	debug_echo("  ------------- load_language_data End --------------");
}


void cfoLanguage::set_language_arr(char const * const language_code, char const * const text, int const length)
{
	// First delete if any earlier language exist,
	// and then replace that array position.
	_delete_language_arr(language_code);

	if (language_count >= allocated_language_count)
	{
		fprintf
		(
			stderr,
			"Error: foFunction-fo_language-set_language_arr Too many languages allocated, current %d, max %d\n",
			language_count,
			allocated_language_count
		);

		return;
	}

	// Any text do add?
	if (length>0)
	{
		// Create a new array position.
		int index = language_count;

		// Language
		language[index] = new char[foLDC_LANGUAGESIZE];
		memcpy(language[index], language_code, foLDC_LANGUAGESIZE);

		// Data
		data_lengths[index] = length;
		data[index] = new char[length];
		memcpy(data[index], text, length);
		total_data_length += length;

		language_count++;
	}
}


void cfoLanguage::parse_language_data(char const * const newText, U32 const length)
{
	char * new_text_n = new char[length+1];
	memcpy(new_text_n, newText, length);
	new_text_n[length] = 0;

	char * new_text_n_read = new_text_n;
	char * open_tag_pos, *closeTagPos, * language;

	open_tag_pos = strstr(new_text_n_read, "[LANG=");
	while(open_tag_pos != NULL)
	{
		// Points to the language code.
		new_text_n_read = open_tag_pos+(foLDC_OPENTAGSIZE-foLDC_LANGUAGESIZE)-1;
		language = new_text_n_read;
		new_text_n_read +=foLDC_LANGUAGESIZE;

		if (*(new_text_n_read) == ']')
		{
			new_text_n_read +=1;
			closeTagPos = strstr(new_text_n_read, "[/LANG]");
		}
		else
		{
			closeTagPos = NULL;
		}

		open_tag_pos = strstr(new_text_n_read, "[LANG=");

		// Don't setLanguage for language de_
		// in this string "[lang=de_]text[lang=se_]text[/lang]". se is however valid for parsing.
		if (open_tag_pos == NULL || (closeTagPos != NULL && closeTagPos < open_tag_pos))
		{
			set_language_arr(language, new_text_n_read, closeTagPos-new_text_n_read);
		}
	}

	delete[] new_text_n;
}


void cfoLanguage::fill_language_data(void *str_ptr, unsigned long *length)
{
	debug_echo("  ------------- fill_language_data Begin --------------");
	debug_print();

	if (language_count == 0)
	{
		*length = 0;
	}
	else
	{
		U32 data_start_pos = foLDC_HEADERSIZE+(language_count*foLDC_INDEXSIZE);
		*length = data_start_pos+total_data_length+(language_count*(foLDC_OPENTAGSIZE+foLDC_CLOSETAGSIZE));

		FOString * fo_string = (FOString*)str_ptr;
		fo_string->allocate_buffer(*length);

		//
		// buildHeader
		//
		fo_string->overwrite(foLDC_VALIDATION, (unsigned short)foLDC_VALIDATE_CODE);
		fo_string->overwrite(foLDC_VERSION, (char)1);
		fo_string->overwrite(foLDC_NUMOFLANGUAGE, (unsigned short)language_count);

		fo_string->overwrite(foLDC_index_pos, (U32)foLDC_HEADERSIZE);

		fo_string->overwrite(foLDC_data_pos, (U32)data_start_pos);

		//
		// build index and data.
		//
		int index_w_pos = foLDC_HEADERSIZE;
		int data_w_pos = data_start_pos;
		int langPos = -1;

		if (default_language[0] != 0)
		{
			langPos = _get_language_position(default_language);

			// Language didnt exist.
			if (langPos == -1)
				default_language[0] = 0;
			else
				_write_language_data(fo_string, &index_w_pos, &data_w_pos, data_start_pos, default_language, data[langPos], data_lengths[langPos]);
		}

		for(int i=0;i<language_count;i++)
		{
			if (langPos != i)
			{
				_write_language_data(fo_string, &index_w_pos, &data_w_pos, data_start_pos,  language[i], data[i], data_lengths[i]);

				// If no default language are set by the user,
				// use the first language in the foLanguage index as default language.
				if (default_language[0] == 0)
					memcpy(default_language, language[i], foLDC_LANGUAGESIZE);
			}
		}

		// default_language
		fo_string->overwrite(foLDC_default_language, default_language, foLDC_LANGUAGESIZE);
	}

	debug_echo("  ------------- fill_language_data End --------------");
}


void cfoLanguage::debug_print()
{
	#ifdef FO_LANGUGE_DEBUG_MODE
		debug_echo("    ---------- foLanguage Begin -------------");
		debug_echo_prefix(); fprintf(stderr,	"      allocated_language_count: %d\n", allocated_language_count);
		debug_echo_prefix(); fprintf(stderr,	"      language_count: %d\n", language_count);

		debug_echo_prefix(); fprintf(stderr,	"      languages\n");
		for(int i=0;i<language_count;i++)
		{
			debug_echo_prefix(); fprintf(stderr,	"        language: %s\n", language[i]);
			debug_echo_prefix(); fprintf(stderr,	"        data: %s\n", data[i]);
			debug_echo_prefix(); fprintf(stderr,	"        data_lengths: %d\n", data_lengths[i]);
			debug_echo("");
		}
		debug_echo_prefix(); fprintf(stderr,	"      total_data_length: %d\n", total_data_length);

		debug_echo_prefix(); fprintf(stderr,	"      default_language: %s\n", default_language);

		debug_echo("    ---------- foLanguage End -------------");
	#endif
}


int cfoLanguage::_delete_language_arr(char const * const language_code)
{
	// Letar reda på den language array som ska tas bort.
	for(int i=0;i<language_count;i++)
	{
		if (strncmp(language[i], language_code, foLDC_LANGUAGESIZE) == 0)
		{
			total_data_length -= data_lengths[i];
			delete[] language[i];
			delete[] data[i];

			// Reindex all the languages, to remove the "unused"
			// areas in the array.
			language_count--;
			for(unsigned short j=i;j<language_count;j++)
			{
				data[j] = data[j+1];
				data_lengths[j] = data_lengths[j+1];
				language[j] = language[j+1];
			}
			return i;
		}
	}

	return -1;
}


int cfoLanguage::_get_language_position(char const * const language_code)
{
	for(int i=0;i<language_count;i++)
	{
		if (strncmp(language[i], language_code, foLDC_LANGUAGESIZE) == 0)
			return i;
	}

	return -1;
}


void cfoLanguage::_write_language_data
(
		FOString * const fo_string, int * index_w_pos, int * data_w_pos, int const data_start_pos,
		char const * const language, char const * const text, U32 const length
)
{
	// IndexArea - language
	fo_string->overwrite(index_w_pos, language, foLDC_LANGUAGESIZE);

	// IndexArea - start_pos
	fo_string->overwrite(index_w_pos, (U32)(*data_w_pos+foLDC_OPENTAGSIZE-data_start_pos));

	// IndexArea - length
	fo_string->overwrite(index_w_pos, length);

	// DataArea - Start tag [LANG=XX]
	fo_string->overwrite(data_w_pos, "[LANG=", 6);
	fo_string->overwrite(data_w_pos, language, foLDC_LANGUAGESIZE);
	fo_string->overwrite(data_w_pos, "]", 1);

	// DataArea - Data
	fo_string->overwrite(data_w_pos, text, length);

	// DataArea - End tag [/LANG]
	fo_string->overwrite(data_w_pos, "[/LANG]", 7);

	// Debug information
	#ifdef FO_LANGUGE_DEBUG_MODE
		debug_echo("    ------------- writeLanguageData Begin --------------");
		debug_echo_prefix(); fprintf(stderr,	"      language %s\n",  language);
		debug_echo_prefix(); fprintf(stderr,	"      start_pos %d\n",  (U32)(*data_w_pos+foLDC_OPENTAGSIZE-data_start_pos));
		debug_echo_prefix(); fprintf(stderr,	"      length %d\n",  length);
		debug_echo_prefix(); fprintf(stderr,	"      text %s\n",  text);
		debug_echo("    ------------- writeLanguageData End --------------");
	#endif
}
