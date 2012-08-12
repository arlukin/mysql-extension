#include <UnitTest++.h>

typedef bool my_bool;


enum Item_result {STRING_RESULT,REAL_RESULT,INT_RESULT};


typedef struct st_udf_args
{
  unsigned int arg_count;       /* Number of arguments */
  enum Item_result *arg_type;   /* Pointer to item_results */
  char **args;                  /* Pointer to argument */
  unsigned long *lengths;       /* Length of string arguments */
  char *maybe_null;             /* Set to 1 for all maybe_null args */
} UDF_ARGS;


typedef struct st_udf_init
{
  bool maybe_null;          /* 1 if function can return NULL */
  unsigned int decimals;    /* for real functions */
  unsigned int max_length;  /* For string functions */
  char    *ptr;             /* free pointer for function data */
  bool const_item;          /* 0 if result is independent of arguments */
} UDF_INIT;



#include <fo_string.h>
#include <fo_language.hpp>
#include <udf.hpp>


UDF_ARGS * createUDFARG(int count)
{
    UDF_ARGS *args = new UDF_ARGS;

    args->args = new char*[count];
    args->arg_count = count;
    args->lengths = new unsigned long[count];
    args->arg_type = new Item_result[count];

    return args;
}


void freeUDFARG(UDF_ARGS *args)
{
    for (unsigned int i = 0; i < args->arg_count; i++)
    {
        delete(args->args[i]);
    }
    delete args->args;
    delete args->lengths;
}


void set_empty_UDFARG(UDF_ARGS *args, int index)
{
        args->args[index] = NULL;
        args->lengths[index] = 0;
}


void setUDFARG(UDF_ARGS *args, int index, char const * const text)
{
    if (text == NULL)
    {
        set_empty_UDFARG(args, index);
    }
    else
    {
        args->args[index] = new char[strlen(text)+1];
        args->lengths[index] = strlen(text);
        memcpy(args->args[index], text, strlen(text));
        args->args[index][strlen(text)] = 0;
    }
}


void setUDFARG(UDF_ARGS *args, int index, char const * const text, int length)
{
    if (text == NULL)
    {
        set_empty_UDFARG(args, index);
    }
    else
    {
        args->args[index] = new char[length];
        args->lengths[index] = length;
        memcpy(args->args[index], text, length);
    }
}


void setUDFARG(UDF_ARGS *args, int index, int value)
{
    args->args[index] = (char*)new int;
    args->lengths[index] = sizeof(int);
    *args->args[index] = value;
}


void setUDFARG(UDF_ARGS *args, int index, FOString * const foString)
{
    if (foString == NULL)
    {
        set_empty_UDFARG(args, index);
    }
    else
    {
        setUDFARG(args, index, foString->get(), foString->length());
    }
}


void setUDFARGNUM(UDF_ARGS *args, int index, double value)
{
    args->args[index] = (char*)new double;
    args->lengths[index] = sizeof(double);
    args->args[index] = (char*)&value;
}

//
//
//


FOString * selectSetLanguage(FOString * dbColumn, char const * const newText, char const *  const language, char const * const defaultLanuage)
{
    char *is_null = new char;
    char *error = new char;

    UDF_ARGS *args = createUDFARG(ARG_SET_LANGUAGE_COUNT);

    setUDFARG(args, ARG_SET_LANGUAGE_DBCOLUMN,              dbColumn);
    setUDFARG(args, ARG_SET_LANGUAGE_NEWTEXT,                   newText);
    setUDFARG(args, ARG_SET_LANGUAGE_LANGUAGE,              language);
    setUDFARG(args, ARG_SET_LANGUAGE_default_language,  defaultLanuage);

    char * result = NULL;
    unsigned long * length = new unsigned long;
    char *message = new char[100];

    UDF_INIT *initidRet;
    initidRet = new UDF_INIT;

    setLanguage_init(initidRet, args, message);
    char * retString = setLanguage(initidRet, args, result, length, is_null, error);
    setLanguage_deinit(initidRet);  // Orkar inte fixa return värdet.

    FOString *retArg = new FOString(*length);
    retArg->set(retString, *length);

    freeUDFARG(args);

    delete error;
    delete is_null;
    delete result;
    delete length;
    delete initidRet;

    return retArg;
}


char * selectGetLanguage(FOString *dbColumn, char const * const  firstLang, char const * const secondLang, int viewMode)
{
    char *is_null = new char;
    char *error = new char;

    UDF_ARGS *args = createUDFARG(ARG_GET_LANGUAGE_COUNT);

    setUDFARG(args, ARG_GET_LANGUAGE_DBCOLUMN,              dbColumn);
    setUDFARG(args, ARG_GET_LANGUAGE_FIRSTLANG,             firstLang);
    setUDFARG(args, ARG_GET_LANGUAGE_SECONDLANG,            secondLang);
    setUDFARG(args, ARG_GET_LANGUAGE_VIEWMODE,              viewMode);

    char * result = NULL;
    unsigned long * length = new unsigned long;
    UDF_INIT *initidRet;
    initidRet = new UDF_INIT;
    char *message = new char[100];

    getLanguage_init(initidRet, args, message);
    char * ret = getLanguage(initidRet, args, result, length, is_null, error);
    char * ret2 = new char[*length];
    memcpy(ret2, ret, *length);

    char * ret3 = getLanguage(initidRet, args, result, length, is_null, error);
    char * ret4 = new char[*length];
    memcpy(ret4, ret3, *length);

    getLanguage_deinit(initidRet);
    freeUDFARG(args);

    delete error;
    delete is_null;
    delete result;
    delete length;
    delete message;

    return ret2;
}


TEST(fo_language)
{
    char *  chr1;
    FOString *ret1 = selectSetLanguage(NULL,  "asdfasdf[LANG=UK_]England[/LANG]asdfasdf[LANG=FI_]Finland[/LdANG][LANG=SE_]Sverige[/LANG][LANG=FR_]Frankrike NEW[/LANG]", "", "FR_");
    //chr1 = selectGetLanguage(ret1, NULL, NULL, 0);


    //FOString *ret1 = selectSetLanguage(NULL,  "", "", NULL);

    FOString *ret2 = selectSetLanguage(ret1, "Daniel är en apa", "SE_", NULL);
    FOString *ret3 = selectSetLanguage(ret2, "Daniel är en apa2", "SE_", NULL);

    FOString *ret4 = selectSetLanguage(ret3, "Daniel is monkey", "UK_", "FI_");
    FOString *ret5 = selectSetLanguage(ret4,  "Daniel is Moimukulat", "FI_", "UK_");
    FOString *ret6 = selectSetLanguage(ret5,  "Daniel lé apé", "FR_", NULL);
    FOString *ret7 = selectSetLanguage(ret6,  "", "SE_", NULL);
    FOString *ret8 = selectSetLanguage(ret7,  "Vissa åldersrestriktioner finns vid vissa uthyrningstationer och i vissa fall tillkommer en kostnad för förare vilken betalas lokalt. För varje extra förare (utöver vad som är inkluderat i priset) debiteras en avgift som varierar från land till land. Övrig information kan fås vid bokninstillfället. English", "US_", NULL);


    chr1 = selectGetLanguage(ret8, "US_", NULL, 1);

    chr1 = selectGetLanguage(ret8, "SE_", NULL, 0);
    chr1 = selectGetLanguage(ret8, "SE_", NULL, 1);
    chr1 = selectGetLanguage(ret8, "SE_", NULL, 2);
    chr1 = selectGetLanguage(ret8, "SE_", NULL, 3);



    chr1 = selectGetLanguage(ret8, "US_", NULL, 0);

    chr1 = selectGetLanguage(ret8, "FI_", NULL, 0);
    chr1 = selectGetLanguage(ret8, "IS_", NULL, 0);
    chr1 = selectGetLanguage(ret8, "IS_", "UK_", 0);
    chr1 = selectGetLanguage(ret8, "IS_", "MI_", 1);
    chr1 = selectGetLanguage(ret8, "IS_", "MI_", 0);


    chr1 = selectGetLanguage(ret8, NULL, NULL, 0);
    chr1 = selectGetLanguage(ret8, NULL, NULL, 1);
    chr1 = selectGetLanguage(ret8, NULL, NULL, 2);
    chr1 = selectGetLanguage(ret8, NULL, NULL, 3);

    chr1 = selectGetLanguage(ret8, "SE_", "FI_", 0);
    chr1 = selectGetLanguage(ret8, "IS_", "FI_", 0);
    chr1 = selectGetLanguage(ret8, "IS_", "MI_", 1);
    chr1 = selectGetLanguage(ret8, "IS_", "", 1);
    chr1 = selectGetLanguage(ret8, "IS_", NULL, 1);
    chr1 = selectGetLanguage(ret8, "IS_", "FR_", 1);
    chr1 = selectGetLanguage(ret8, "IS_", "FR_", 1);
    chr1 = selectGetLanguage(ret8, "FR_", "FR_", 1);

    CHECK_EQUAL((unsigned int)4, sizeof(int));

    delete ret1;
    delete ret2;
    delete ret3;
    delete ret4;
    delete ret5;
    delete ret6;
    delete ret7;
    delete ret8;
    delete chr1;
}
