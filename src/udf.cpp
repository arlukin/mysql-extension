/*

The UDF functions that are callable from mysql

__author__     = "daniel.lindh@cybercow.se"
__copyright__  = "Copyright 2012, Fareoffice CRS AB"
__maintainer__ = "Daniel Lindh"
__version__    = "1.0.0"
__status__     = "Production"

*/


#include <udf.hpp>
#include <fo_debug.hpp>
#include <fo_string.h>
#include <fo_language.hpp>


/////////////////////////////////////////////////////////
//
// PRIVATE:
//
/////////////////////////////////////////////////////////


/**
* Verify if an UDF argument (mysql column) is null or "" (empty).
*
* Works like the php function empty()
*
* @access private
*/
inline bool empty_arg(UDF_ARGS const * const args, const int index)
{
    if (args->args[index] == NULL || args->lengths[index] == 0)
        return true;
    else
        return false;
}


/**
* Modify: initid->ptr, initid->max_length, text_length
*
* Adds a 'language-text' to the foLanguage struct/array,
* but will first parse the newText string.
*
* @access private
*/
void createReturnString
(
        UDF_INIT * const initid,
        char const * const data_area_pos,
        U32 const * const text_start_pos,
        U32 &text_length,
        char const * const first_language,
        char const * const found_language
)
{
    FOString * fo_string = (FOString*)initid->ptr;

    if (text_start_pos == NULL)
    {
        fo_string->set(data_area_pos, text_length);
    }
    else
    {
        U32 orginal_text_length = text_length;

        if (found_language != NULL && text_start_pos != NULL)
        {
            text_length += (foLDC_OPENTAGSIZE+foLDC_CLOSETAGSIZE)*2;
        }

        fo_string->allocate_buffer(text_length);
        int initidWritePos = 0;

        if (found_language != NULL)
        {
            fo_string->overwrite(&initidWritePos, "[LANG=", 6);
            fo_string->overwrite(&initidWritePos, first_language, foLDC_LANGUAGESIZE);
            fo_string->overwrite(&initidWritePos, "]", 1);
            fo_string->overwrite(&initidWritePos, "[/LANG]", foLDC_CLOSETAGSIZE);

            fo_string->overwrite(&initidWritePos, "[LANG=", 6);
            fo_string->overwrite(&initidWritePos, found_language, foLDC_LANGUAGESIZE);
            fo_string->overwrite(&initidWritePos, "]", 1);
        }

        fo_string->overwrite(&initidWritePos, data_area_pos+*text_start_pos, orginal_text_length);

        if (found_language != NULL)
        {
            fo_string->overwrite(&initidWritePos, "[/LANG]", foLDC_CLOSETAGSIZE);
        }
    }
}

/////////////////////////////////////////////////////////
//
// PUBLIC:
//
/////////////////////////////////////////////////////////


/**
* Executed before setLanguage.
*
* If setLanguage is executed on several rows in a table, this function will be
* executed only once for the query. But setLanguage will be executed for each
* row.
*
* Ie. select setLanguage(...) from TEST_TABLE
*
* @access public
*/
my_bool setLanguage_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    debug_begin(true, false);
    debug_echo("------------- foFunction -> SetLanguage Begin --------------");

    if (args->arg_count < ARG_SET_LANGUAGE_COUNT)
    {
        // The message can be a maximum of MYSQL_ERRMSG_SIZE bytes long.
        strcpy(message,"usage: char* = setLanguage(db_column, newText, language, default_language)");
        return 1;
    }
    else
    {
        args->arg_type[ARG_SET_LANGUAGE_db_column]            = STRING_RESULT;
        args->arg_type[ARG_SET_LANGUAGE_NEWTEXT]             = STRING_RESULT;
        args->arg_type[ARG_SET_LANGUAGE_LANGUAGE]            = STRING_RESULT;
        args->arg_type[ARG_SET_LANGUAGE_default_language]    = STRING_RESULT;

        initid->decimals = 0;
        initid->maybe_null = 1;
        initid->max_length = 1600000; // Can return a maximum of this.
        initid->ptr = (char *)new FOString;    // Allocate 255 byte to begin with.

        return 0;
    }
}


/**
* Executed after setLanguage.
*
* @see setLanguage_init
* @access public
*/
void setLanguage_deinit(UDF_INIT *initid)
{
    delete (FOString*)initid->ptr;
    debug_echo("------------- foFunction -> SetLanguage End --------------");
}


/**
* Executed for each row in a query.
*
* @see setLanguage_init
* @access public
*/
char *setLanguage(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
    debug_begin(false, true);
    cfoLanguage foLanguage;

    // Load the current language information, to add new language data to.
    // Can be NULL
    if (!empty_arg(args, ARG_SET_LANGUAGE_db_column))
    {
        foLanguage.load_language_data(args->args[ARG_SET_LANGUAGE_db_column]);
    }

    foLanguage.debug_print();

    // If LANGUAGE is set, the NEWTEXT will be added with that language
    // to the foLanguageColumn.
    if (!empty_arg(args, ARG_SET_LANGUAGE_LANGUAGE))
    {
        foLanguage.set_language_arr(
            args->args[ARG_SET_LANGUAGE_LANGUAGE],
            args->args[ARG_SET_LANGUAGE_NEWTEXT],
            args->lengths[ARG_SET_LANGUAGE_NEWTEXT]
        );
    }

    // If not the NEWTEXT is supposed to be in the FareofficeTag format
    // [LANG=SE_]...[/LANG].
    else
    {
        foLanguage.parse_language_data(
            args->args[ARG_SET_LANGUAGE_NEWTEXT],
            args->lengths[ARG_SET_LANGUAGE_NEWTEXT]
        );
    }

    if (!empty_arg(args, ARG_SET_LANGUAGE_default_language))
    {
        memcpy(
            foLanguage.default_language,
            args->args[ARG_SET_LANGUAGE_default_language],
            foLDC_LANGUAGESIZE
        );
    }

    foLanguage.fill_language_data(initid->ptr, length);

    FOString * fo_string = (FOString*)initid->ptr;

    if (*length == 0)
    {
        *is_null = true;
        return NULL;
    }
    else
    {
        return fo_string->get();
    }
}


/**
* Executed before getLanguage.
*
* If getLanguage is executed on several rows in a table, this function will be
* executed only once for the query. But getLanguage will be executed for each
* row.
*
* Ie. select getLanguage(...) from TEST_TABLE
*
* @access public
*/
my_bool getLanguage_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    debug_begin(true, false);
    debug_echo("------------- foFunction -> GetLanguage Begin --------------");

    if (args->arg_count < ARG_GET_LANGUAGE_COUNT)
    {
        // The message can be a maximum of MYSQL_ERRMSG_SIZE bytes long.
        strcpy(message,"usage: char* = getLanguage(db_column, first_language, second_language, default_language)");
        return 1;
    }
    else
    {
        args->arg_type[ARG_GET_LANGUAGE_db_column]   = STRING_RESULT;
        args->arg_type[ARG_GET_LANGUAGE_FIRSTLANG]  = STRING_RESULT;
        args->arg_type[ARG_GET_LANGUAGE_SECONDLANG] = STRING_RESULT;
        args->arg_type[ARG_GET_LANGUAGE_VIEWMODE]   = STRING_RESULT;

        initid->decimals = 0;
        initid->maybe_null = 1;
        initid->max_length = 1600000; // Can return a maximum of this.

        initid->ptr = (char *)new FOString;    // Allocate 255 byte to begin with.

        return 0;
    }
}


/**
* Executed after getLanguage.
*
* @see getLanguage_init
* @access public
*/
void getLanguage_deinit(UDF_INIT *initid)
{
    delete (FOString*)initid->ptr;
    debug_echo("------------- foFunction -> GetLanguage End --------------");
}


/**
* Executed for each row in a query.
*
* @see getLanguage_init
*
* @access public
*/
char *getLanguage(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
    debug_begin(false, true);
    if (!empty_arg(args, ARG_GET_LANGUAGE_db_column))
    {
        // Get arguments from the function call.
        char *db_column = args->args[ARG_GET_LANGUAGE_db_column];

        unsigned short * validation_code = (unsigned short*)(db_column+foLDC_VALIDATION);
        if (*validation_code == foLDC_VALIDATE_CODE)
        {
            char *first_language  = NULL;
            char *second_language = NULL;

            if (!empty_arg(args, ARG_GET_LANGUAGE_FIRSTLANG))
            {
                first_language = args->args[ARG_GET_LANGUAGE_FIRSTLANG];
            }

            if (!empty_arg(args, ARG_GET_LANGUAGE_SECONDLANG))
            {
                second_language = args->args[ARG_GET_LANGUAGE_SECONDLANG];
            }

            char *default_language = NULL;

            // Should we use the default value from the foLanuageColumn.
            if
            (
                (strncmp(args->args[ARG_GET_LANGUAGE_VIEWMODE], "1", 1) == 0) ||
                (strncmp(args->args[ARG_GET_LANGUAGE_VIEWMODE], "3", 1) == 0)
            )
            {
                default_language = db_column+foLDC_default_language;
            }

            // Debug code
            #ifndef FO_LANGUGE_DEBUG_MODE
                cfoLanguageHeader langauge_header;
                langauge_header.load_from_string(db_column);
                langauge_header.debug_print();
                debug_echo_ex("validation_code=",   validation_code);
                debug_echo_ex("first_language=",    first_language);
                debug_echo_ex("second_language=",   second_language);
                debug_echo_ex("default_language=", default_language);
            #endif

            // Set easier to use variables
            U32 * db_columndata_pos = (U32*)(db_column+foLDC_data_pos);
            U32 * db_columnindex_pos = (U32 *)(db_column+foLDC_index_pos);

            U32 * text_start_pos = NULL;
            U32 text_length = 0;
            bool language_found = false;
            char * found_languagePos = NULL;

            //
            // Read the data
            //
            if (first_language == NULL)
            {
                text_start_pos = NULL;
                text_length = args->lengths[ARG_GET_LANGUAGE_db_column]-*db_columndata_pos;
                language_found = true;
            }
            else
            {
                char * index_read_pos = db_column+*db_columnindex_pos;
                for (int i=0;i<(char)(db_column[foLDC_NUMOFLANGUAGE]);i++)
                {
                    // index_read_pos points to language
                    if (strncmp(index_read_pos, first_language, foLDC_LANGUAGESIZE) == 0)
                    {
                        text_start_pos = (U32*)(index_read_pos+foLDC_LANGUAGESIZE);
                        text_length = *((U32*)(index_read_pos+foLDC_LANGUAGESIZE+4));
                        found_languagePos = NULL;
                        language_found = true;
                        break;
                    }
                    else if ((second_language != NULL && strncmp(index_read_pos, second_language, foLDC_LANGUAGESIZE) == 0) ||
                                     (default_language != NULL && language_found == false && strncmp(index_read_pos, default_language, foLDC_LANGUAGESIZE) == 0))
                    {
                        text_start_pos = (U32*)(index_read_pos+foLDC_LANGUAGESIZE);
                        text_length = *((U32*)(index_read_pos+foLDC_LANGUAGESIZE+4));
                        language_found = true;

                        if
                        (
                            (strncmp(args->args[ARG_GET_LANGUAGE_VIEWMODE], "2", 1) == 0) ||
                            (strncmp(args->args[ARG_GET_LANGUAGE_VIEWMODE], "3", 1) == 0)
                        )
                        {
                            found_languagePos = index_read_pos;

                        }
                    }

                    // Jump to next index "row"
                    index_read_pos+=foLDC_INDEXSIZE;
                }
            }

            if (language_found == true)
                createReturnString(initid, db_column+*db_columndata_pos, text_start_pos, text_length, first_language, found_languagePos);

            *length = text_length;
        }
        else
        {
            *length = args->lengths[ARG_GET_LANGUAGE_db_column];
            U32 text_length = *length;
            createReturnString(initid, db_column, 0, text_length, NULL, NULL);
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
        FOString * fo_string = (FOString*)initid->ptr;
        return fo_string->get();
    }
}
