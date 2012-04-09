/////////////////////////////////////////////////////////
//
// PRIVATE:
//
/////////////////////////////////////////////////////////

// Stoppar in en ny språk text i foLanguage struct arrayen,
// men parsar först newText strängen.
//

/**
* Verify if an UDF argument (mysql column) is null
* or "" (empty) like the php function empty()
*
*   @access public
*/
inline bool emptyARG(const UDF_ARGS * const args, const int index)
{
    if (args->args[index] == NULL || args->lengths[index] == 0)
        return true;
    else
        return false;
}

/**
* Creates a string buffer, and copies the argument from the UDFARG.
*
*   @access public
*/
inline int copyUDFARG(const UDF_ARGS * const args, const int &argIndex, char **buffer)
{
    if (!emptyARG(args, argIndex))
    {
        *buffer = new char[args->lengths[argIndex]+1];
        memcpy(*buffer, args->args[argIndex], args->lengths[argIndex]);
        *((*buffer)+args->lengths[argIndex]) = 0;

        return EC_SUCCESS;
    }
    else
    {
        *buffer = NULL;
        return EC_INVALID_UDF_ARG;
    }
}


/**
*   Modify: initid->ptr, initid->max_length, textLength
*
*   @access private
*/
void createReturnString
(
        UDF_INIT * const initid, char const * const dataAreaPos,
        U32 const * const textStartPos, U32 &textLength,
        char const * const firstLanguage, char const * const foundLanguage
)
{
    FOString * foString = (FOString*)initid->ptr;

    if (textStartPos == NULL)
    {
        foString->set(dataAreaPos, textLength);
    }
    else
    {
        U32 orginalTextLength = textLength;

        if (foundLanguage != NULL && textStartPos != NULL)
            textLength += (foLDC_OPENTAGSIZE+foLDC_CLOSETAGSIZE)*2;

        foString->allocate_buffer(textLength);
        int initidWritePos = 0;

        if (foundLanguage != NULL)
        {
            foString->overwrite(&initidWritePos, "[LANG=", 6);
            foString->overwrite(&initidWritePos, firstLanguage, foLDC_LANGUAGESIZE);
            foString->overwrite(&initidWritePos, "]", 1);
            foString->overwrite(&initidWritePos, "[/LANG]", foLDC_CLOSETAGSIZE);

            foString->overwrite(&initidWritePos, "[LANG=", 6);
            foString->overwrite(&initidWritePos, foundLanguage, foLDC_LANGUAGESIZE);
            foString->overwrite(&initidWritePos, "]", 1);
        }

        foString->overwrite(&initidWritePos, dataAreaPos+*textStartPos, orginalTextLength);

        if (foundLanguage != NULL)
        {
            foString->overwrite(&initidWritePos, "[/LANG]", foLDC_CLOSETAGSIZE);
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
        [0]dbColumn                 - "[LANGINDEX][LANG=SE]Hej världen[/LANG][LANG=UK]Hello world[/LANG]"
                                                    Can be null
        [1]newText                  - "Moi mukulat"
                                                    Can be null, will delete the language indicated by [2]language.
        [2]language                 - "FI"
                                                    If null, the [1]newText field must include the [LANG=xx] tag.
        [3]defaultLanguage  - "UK" can be null

    Return
        The concated string with the index.
        [...LANGINDEX....][LANG=UK]Hello world[/LANG][LANG=SE]Hej världen[/LANG][LANG=FI]Moi mukulat[/LANG]

    Example
        select setLanguage(NULL, "Moi mukulat]", "FI", "UK") as moo;

***************************************************************************/
#define ARG_SET_LANGUAGE_DBCOLUMN           0
#define ARG_SET_LANGUAGE_NEWTEXT                    1
#define ARG_SET_LANGUAGE_LANGUAGE           2
#define ARG_SET_LANGUAGE_DEFAULTLANGUAGE    3

#define ARG_SET_LANGUAGE_COUNT  4

/**
*
*   @access public
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
        args->arg_type[ARG_SET_LANGUAGE_DBCOLUMN]               = STRING_RESULT;
        args->arg_type[ARG_SET_LANGUAGE_NEWTEXT]                    = STRING_RESULT;
        args->arg_type[ARG_SET_LANGUAGE_LANGUAGE]           = STRING_RESULT;
        args->arg_type[ARG_SET_LANGUAGE_DEFAULTLANGUAGE]    = STRING_RESULT;

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
void setLanguage_deinit(UDF_INIT *initid)
{
    delete initid->ptr;
    debugEcho("------------- foFunction -> SetLanguage End --------------");
}

/**
*
*   @access public
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

    foLanguage.fillLanguageData(initid->ptr, length);

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

/***************************************************************************
    text = getLanguage(dbColumn, firstLang, secondLang, viewMode);
    ***********
    Geta language from a foLanguageColumn.

    Argument format
        [0]dbColumn         - "[LANGINDEX][LANG=SE]Hej världen[/LANG][LANG=UK]Hello world[/LANG]"
                                            Can be null.
        [1]firstLang        - "SE"  Look for this lang first.
                                            If null, return all the languages with tags.
        [2]secondLang       - "FI"  If not firstLang are found, try to find this.
                                            Can be null, won't search for this lang if null
        [3]viewMode         - 0 - Only firstLang and secondLang
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
#define ARG_GET_LANGUAGE_DBCOLUMN       0
#define ARG_GET_LANGUAGE_FIRSTLANG      1
#define ARG_GET_LANGUAGE_SECONDLANG   2
#define ARG_GET_LANGUAGE_VIEWMODE           3

#define ARG_GET_LANGUAGE_COUNT  4

/**
*
*   @access public
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
    delete initid->ptr;
    debugEcho("------------- foFunction -> GetLanguage End --------------");
}

/**
*
*   @access public
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
                debugEchoEX("ValidationCode=",  validationCode);
                debugEchoEX("firstLanguage=",   firstLanguage);
                debugEchoEX("secondLanguage=",  secondLanguage);
                debugEchoEX("defaultLanguage=",     defaultLanguage);
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
        FOString * foString = (FOString*)initid->ptr;
        return foString->get();
    }
}
