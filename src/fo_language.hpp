/*

The set language logic.

__author__     = "daniel.lindh@cybercow.se"
__copyright__  = "Copyright 2012, Fareoffice CRS AB"
__maintainer__ = "Daniel Lindh"
__version__    = "1.0.0"
__status__     = "Production"

*/


#ifndef FO_LANGUAGE_H
#define FO_LANGUAGE_H

#include "fo_string.h"

// Define variables that can vary on different OS.
typedef unsigned char  U8;
typedef unsigned short U16;
typedef unsigned int   U32;
typedef unsigned long  U64;

//
// foLanguageDataColumn
//
#define foLDC_LANGUAGESIZE 3    // Number of chars in the language code "SE_"

// Positions in the foLanguageDataColumn
#define foLDC_VALIDATION        0  // Size 2 bytes
#define foLDC_VERSION           2  // Size 1 byte
#define foLDC_NUMOFLANGUAGE     3  // Size 2 bytes
#define foLDC_INDEXPOS          5  // Size 4 bytes
#define foLDC_DATAPOS           9  // Size 4 bytes
#define foLDC_default_language  13 // Size foLDC_LANGUAGESIZE

#define foLDC_HEADERSIZE        (13+foLDC_LANGUAGESIZE)
#define foLDC_INDEXSIZE         (8+foLDC_LANGUAGESIZE)  // 8 bytes + languagesize
#define foLDC_OPENTAGSIZE       (7+foLDC_LANGUAGESIZE)  // Number of bytes in [LANG=XX_]
#define foLDC_CLOSETAGSIZE      7                       // Number of bytes in [/LANG]

// Positions in the index part.
#define foLDC_INDEX_LANGUAGE    0                        // Size languagesize bytes
#define foLDC_INDEX_STARTPOS    foLDC_LANGUAGESIZE       // Size 4 bytes
#define foLDC_INDEX_LENGTH      (4+foLDC_LANGUAGESIZE)   // Size 4 bytes
#define foLDC_INDEX_SIZE        (foLDC_LANGUAGESIZE+4+4) // Size of one index section

// The validation code in the LanguageDataColumn should match this.
#define foLDC_VALIDATE_CODE 65280

//
// Header delen av det som sparas i databasen.
//
class cfoLanguageHeader
{
public:
    unsigned short *validationCode;
    char  *version;
    unsigned short *numOfLanguages;
    U32 *indexPos;
    U32 *dataPos;
    char * default_language; // Will be the size of foLDC_default_language

    void load_from_string(char const * const dbColumn);
    void debug_print();
};


class cfoLanguageIndex
{
public:
    char *language; // Will be the size of foLDC_default_language
    U32 *startPos;
    U32 *length;

    void load_from_string(char const * const dbColumn, U32 indexPos, U32 indexNumber);
    void debug_print();
};


// Used to internaly in the functions store information about the language,
class cfoLanguage
{

public:
    unsigned short allocatedLanguageCount;  // Number of languages their is allocated space for.
    unsigned short languageCount;           // Number of languages
    char **language;                        // The language length is foLDC_LANGUAGESIZE. 'SE_'
    char **data;
    U32 *dataLengths;
    U32 totalDataLength;                    // Sum of the dataLengths
    char default_language[foLDC_LANGUAGESIZE];

    /**
    * Sets standard values for the foLanguage struct
    */
    cfoLanguage();

    /**
    * Clean up the foLanguage struct
    */
    ~cfoLanguage();

    /**
    * Load language data from the database column.
    */
    void load_language_data(char const * const dbColumn);

    /**
    *   Adds a language text to the foLanguage struct array.
    *
    *   @param  char*   languageCode
    *                   The language code for the text entered in text. ie. SE_
    *
    *   @param  char*   text
    *                   The text to add to the foLanguage array.
    *
    *   @param  number  length of text
    */
    void set_language_arr
    (
        char const * const languageCode,
        char const * const text,
        int const length
    );

    /**
    * Parseing the newText for language tags, and inserts the result in the
    * foLanguage struct
    *
    *
    * @params  char     newText
    *                   Example [LANG=SE_]Sverige[/LANG][LANG=UK_]Sweden[/LANG]
    *
    * @params   uint    length
    *                   Length of newText
    */
    void parse_language_data(char const * const newText, U32 length);

    /**
    *   Creates a new foLanguageDataColumn to be saved in the database.
    */
    void fill_language_data(void *str_ptr, unsigned long *length);

    /**
    * Print debug info about the object instance.
    */
    void debug_print();


private:


    /**
    *   Delete language from the datastruct foLanguage.
    */
    int _delete_language_arr(char const * const languageCode);

    /**
    *   Get the array index for a language.
    */
    int _get_language_position(char const * const languageCode);

    /**
    * Write the index and data information for a language to the string that
    * will be save as a foLanguageDataColumn to in the database.
    */
    void _write_language_data
    (
        FOString * foString, int * indexWPos, int * dataWPos, int const dataStartPos,
        char const * const language, char const * const text, U32 const length
    );
};


#endif
