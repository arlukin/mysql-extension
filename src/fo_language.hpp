#ifndef FO_LANGUAGE_H
#define FO_LANGUAGE_H

// Define variables that can vary on different OS.
typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef unsigned long U64;

//
// foLanguageDataColumn
//
#define foLDC_LANGUAGESIZE 3    // Number of chars in the language code "SE_"

// Positions in the foLanguageDataColumn
#define foLDC_VALIDATION             0  // Size 2 bytes
#define foLDC_VERSION                    2  // Size 1 byte
#define foLDC_NUMOFLANGUAGE      3  // Size 2 bytes
#define foLDC_INDEXPOS               5  // Size 4 bytes
#define foLDC_DATAPOS                    9  // Size 4 bytes
#define foLDC_DEFAULTLANGUAGE   13  // Size foLDC_LANGUAGESIZE

#define foLDC_HEADERSIZE            (13+foLDC_LANGUAGESIZE)
#define foLDC_INDEXSIZE             (8+foLDC_LANGUAGESIZE)  // 8 bytes + languagesize
#define foLDC_OPENTAGSIZE           (7+foLDC_LANGUAGESIZE)  // Number of bytes in [LANG=XX_]
#define foLDC_CLOSETAGSIZE      7   // Number of bytes in [/LANG]

// Positioner i index delen.
#define foLDC_INDEX_LANGUAGE    0   // Size languagesize bytes
#define foLDC_INDEX_STARTPOS    foLDC_LANGUAGESIZE  // Size 4 bytes
#define foLDC_INDEX_LENGTH      (4+foLDC_LANGUAGESIZE)  // Size 4 bytes
#define foLDC_INDEX_SIZE            (foLDC_LANGUAGESIZE+4+4) // Size of one index section

//
#define foLDC_VALIDATE_CODE 65280   // The validation code in the LanguageDataColumn should match this.

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
    char * defaultLanguage; // Will be the size of foLDC_DEFAULTLANGUAGE

    void loadFromString(char const * const dbColumn);
    inline void debugPrint();
};

class cfoLanguageIndex
{
public:
    char *language; // Will be the size of foLDC_DEFAULTLANGUAGE
    U32 *startPos;
    U32 *length;

    void loadFromString(char const * const dbColumn, U32 indexPos, U32 indexNumber);
    inline void debugPrint();
};

// Used to internaly in the functions store information about the language,
class cfoLanguage
{

public:
    unsigned short allocatedLanguageCount;  // Number of languages their is allocated space for.
    unsigned short languageCount;                       // Number of languages
    char **language;                                                // The language length is foLDC_LANGUAGESIZE. 'SE_'
    char **data;
    U32 *dataLengths;
    U32 totalDataLength;    // Sum of the dataLengths
    char defaultLanguage[foLDC_LANGUAGESIZE];

    /**
    *   Sets standard values for the foLanguage struct
    *
    *   @access private
    */
    cfoLanguage();

    /**
    *   Clean up the foLanguage struct
    *
    *   @access private
    */
    ~cfoLanguage();

    /**
    *   Load language data from the database column.
    *
    *   @access private
    */
    void loadLanguageData(char const * const dbColumn);

    /**
    *   Adds a language text to the foLanguage struct array.
    *
    *
    *   @param  UDF_FOLANGUAGE*     foLanguage
    *                                                       Struct including all the languge data.
    *
    *   @param  char*                       lanugage
    *                                                       The language code for the text entered in text. ie. SE_
    *
    *   @param  char*                       text
    *                                                       The text to add to the foLanguage array.
    *
    *   @param  number                      $length
    *                                                       Length of the text to add, excluding the language tag ([LANG=SE_]).
    *
    *   @access private
    */
    void setLanguageArr(char const * const languageCode, char const * const text, int const length);

    /**
    *   Parseing the newText for language tags, and inserts the result in the foLanguage struct
    *
    * @params   foLanguage  Language
    *                                           array struct.
    *
    * @params  char             newText
    *                                           Example [LANG=SE_]Sverige[/LANG][LANG=UK_]Sweden[/LANG]
    *
    * @params   uint                length
    *                                           Length of newText Example 47
    *   @access private
    */
    void parseLanguageData(char const * const newText, U32 length);

    /**
    *   Creates a new foLanguageDataColumn to be saved in the database.
    *
    *   @access public
    */
    void fillLanguageData(void *str_ptr, unsigned long *length);

    /**
    *
    *   @access private
    */
    inline void debugPrint();

private:

    /**
    *   Delete language from the datastruct foLanguage.
    *
    *   @access private
    */
    int _deleteLanguageArr(char const * const languageCode);

    /**
    *   Get the array index for a language.
    *
    *   @access private
    */
    int _getLanguagePosition(char const * const languageCode);

    /**
    *   Write the index and data information for a language to the string that
    * will be save as a foLanguageDataColumn to in the database.
    *
    *   @access private
    */
    void _writeLanguageData
    (
            FOString * foString, int * indexWPos, int * dataWPos, int const dataStartPos,
            char const * const language, char const * const text, U32 const length
    );
};


#endif
