/*

The UDF functions.

__author__     = "daniel.lindh@cybercow.se"
__copyright__  = "Copyright 2012, Fareoffice CRS AB"
__maintainer__ = "Daniel Lindh"
__credits__    = "???"
__license__    = "???"
__version__    = "1.0.0"
__status__     = "Production"

*/

#ifndef FO_UDF_H
#define FO_UDF_H

#ifdef STANDARD
    #include <stdio.h>
    #include <string.h>
#else
    #include <my_global.h>  // myssql 4
    #include <my_sys.h>
#endif

#include <mysql.h>
#include <m_ctype.h>
#include <m_string.h>

//
// Used by mysqld to find the symbol table in the .so file.
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


/**
* char* = setLanguage(dbColumn, newText, language, default_language)
*
* Concatenates a foLanguageColumn with a new language.
*
* Argument format
*     [0]dbColumn         - "[LANGINDEX][LANG=SE]Hej världen[/LANG][LANG=UK]Hello world[/LANG]"
*                           Can be null
*     [1]newText          - "Moi mukulat"
*                           Can be null, will delete the language indicated by [2]language.
*     [2]language         - "FI"
*                           If null, the [1]newText field must include the [LANG=xx] tag.
*     [3]default_language  - "UK" can be null
*
* Return
*     The concated string with the index.
*     [...LANGINDEX....][LANG=UK]Hello world[/LANG][LANG=SE]Hej världen[/LANG][LANG=FI]Moi mukulat[/LANG]
*
* Example
*     select setLanguage(NULL, "Moi mukulat]", "FI", "UK") as description;
*
*/
#define ARG_SET_LANGUAGE_DBCOLUMN         0
#define ARG_SET_LANGUAGE_NEWTEXT          1
#define ARG_SET_LANGUAGE_LANGUAGE         2
#define ARG_SET_LANGUAGE_default_language 3

#define ARG_SET_LANGUAGE_COUNT  4


/**
* text = getLanguage(dbColumn, firstLang, secondLang, viewMode);
*
* Get a language from a foLanguageColumn.
* Argument format
*     [0]dbColumn   - "[LANGINDEX][LANG=SE]Hej världen[/LANG][LANG=UK]Hello world[/LANG]"
*                     Can be null.
*     [1]firstLang  - "SE"  Look for this lang first.
*                     If null, return all the languages with tags.
*     [2]secondLang - "FI"  If not firstLang are found, try to find this.
*                     Can be null, won't search for this lang if null
*     [3]viewMode   - 0 - Only firstLang and secondLang
*                     1 - If none of the above are found and this is true
*                         look for the first langouage in the foLanguageColumn.
*                         return NULL if false.
*                     2 - Like mode 0, but if only secondLang is found return
*                         with tags [LANG=SE][/LANG][LANG=UK]English[/LANG]
*                     3 - Like mode 1, but if onlye secondLang or defaultLang
*                         is found return with tags.
* Return
*     The wanted string, or with tags ie. [LANG=SE]COW[/LANG]
*
* Example
*     select setLanguage(rule.rule_name, "FI", "UK", 1) as rule_name;
*/
#define ARG_GET_LANGUAGE_DBCOLUMN   0
#define ARG_GET_LANGUAGE_FIRSTLANG  1
#define ARG_GET_LANGUAGE_SECONDLANG 2
#define ARG_GET_LANGUAGE_VIEWMODE   3

#define ARG_GET_LANGUAGE_COUNT  4

#endif
