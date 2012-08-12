/////////////////////////////////////////////////////////
//
// PRIVATE:
//
/////////////////////////////////////////////////////////

include <udf.hpp>

/**
* Verify if an UDF argument (mysql column) is null or "" (empty).
*
* Works like the php function empty()
*
* @access private
*/
inline bool empty_arg(const UDF_ARGS * const args, const int index)
{
    if (args->args[index] == NULL || args->lengths[index] == 0)
        return true;
    else
        return false;
}


/////////////////////////////////////////////////////////
//
// PUBLIC:
//
/////////////////////////////////////////////////////////


/**
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
        strcpy(message,"usage: char* = setLanguage(dbColumn, newText, language, default_language)");
        return 1;
    }
    else
    {
        args->arg_type[ARG_SET_LANGUAGE_DBCOLUMN]               = STRING_RESULT;
        args->arg_type[ARG_SET_LANGUAGE_NEWTEXT]                    = STRING_RESULT;
        args->arg_type[ARG_SET_LANGUAGE_LANGUAGE]           = STRING_RESULT;
        args->arg_type[ARG_SET_LANGUAGE_default_language]    = STRING_RESULT;

        initid->decimals = 0;
        initid->maybe_null = 1;
        initid->max_length = 16000000; // Can return a maximum of this.
        initid->ptr = (char *)new FOString;    // Allocate 255 byte to begin with.

        return 0;
    }
}


/**
*
* @access public
*/
void setLanguage_deinit(UDF_INIT *initid)
{
    delete (FOString*)initid->ptr;
    debug_echo("------------- foFunction -> SetLanguage End --------------");
}


/**
*
*   @access public
*/
char *setLanguage(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
    debug_begin(false, true);
    cfoLanguage foLanguage;

    // Load the current language information, to add new language data to.
    // Can be NULL
    if (!empty_arg(args, ARG_SET_LANGUAGE_DBCOLUMN))
    {
        foLanguage.load_language_data(args->args[ARG_SET_LANGUAGE_DBCOLUMN]);
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

    FOString * foString = (FOString*)initid->ptr;

    if (*length == 0)
    {
        *is_null = true;
        return NULL;
    }
    else
    {
        return foString->get();
    }
}


/**
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
        strcpy(message,"usage: char* = getLanguage(dbColumn, firstLanguage, secondLanguage, default_language)");
        return 1;
    }
    else
    {
        args->arg_type[ARG_GET_LANGUAGE_DBCOLUMN]       = STRING_RESULT;
        args->arg_type[ARG_GET_LANGUAGE_FIRSTLANG]      = STRING_RESULT;
        args->arg_type[ARG_GET_LANGUAGE_SECONDLANG]   = STRING_RESULT;
        args->arg_type[ARG_GET_LANGUAGE_VIEWMODE]           = INT_RESULT;

        initid->decimals = 0;
        initid->maybe_null = 1;
        initid->max_length = 16000000; // Can return a maximum of this.
        initid->ptr = (char *)new FOString;    // Allocate 255 byte to begin with.

        return 0;
    }
}


/**
*
*   @access public
*/
void getLanguage_deinit(UDF_INIT *initid)
{
    delete (FOString*)initid->ptr;
    debug_echo("------------- foFunction -> GetLanguage End --------------");
}


/**
*
*   @access public
*/
char *getLanguage(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
    debug_begin(false, true);
    if (!empty_arg(args, ARG_GET_LANGUAGE_DBCOLUMN))
    {
        // Get arguments from the function call.
        char *dbColumn = args->args[ARG_GET_LANGUAGE_DBCOLUMN];

        unsigned short * validationCode = (unsigned short*)(dbColumn+foLDC_VALIDATION);
        if (*validationCode == foLDC_VALIDATE_CODE)
        {
            char *firstLanguage  = NULL;
            char *secondLanguage = NULL;

            if (!empty_arg(args, ARG_GET_LANGUAGE_FIRSTLANG))
            {
                firstLanguage = args->args[ARG_GET_LANGUAGE_FIRSTLANG];
            }

            if (!empty_arg(args, ARG_GET_LANGUAGE_SECONDLANG))
            {
                secondLanguage = args->args[ARG_GET_LANGUAGE_SECONDLANG];
            }

            char *default_language = NULL;

            // Should we use the default value from the foLanuageColumn.
            if (*args->args[ARG_GET_LANGUAGE_VIEWMODE] == 1 || *args->args[ARG_GET_LANGUAGE_VIEWMODE] == 3)
            {
                default_language = dbColumn+foLDC_default_language;
            }

            // Debug code
            #ifndef FO_LANGUGE_DEBUG_MODE
                cfoLanguageHeader oLangaugeHeader;
                oLangaugeHeader.load_from_string(dbColumn);
                oLangaugeHeader.debug_print();
                debug_echo_ex("ValidationCode=",   validationCode);
                debug_echo_ex("firstLanguage=",    firstLanguage);
                debug_echo_ex("secondLanguage=",   secondLanguage);
                debug_echo_ex("default_language=", default_language);
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
                                     (default_language != NULL && languageFound == false && strncmp(indexReadPos, default_language, foLDC_LANGUAGESIZE) == 0))
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
        FOString * foString = (FOString*)initid->ptr;
        return foString->get();
    }
}
