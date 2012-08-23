/*

Test the udf functions.

__author__     = "daniel.lindh@cybercow.se"
__copyright__  = "Copyright 2012, Fareoffice CRS AB"
__maintainer__ = "Daniel Lindh"
__version__    = "1.0.0"
__status__     = "Production"

*/


#include <string>
#include <UnitTest++.h>
#include <fo_string.h>
#include <fo_language.hpp>
#include <udf.hpp>
using namespace std;


UDF_ARGS create_udf_arg(int count)
{
    UDF_ARGS args;

    args.args = new char*[count];
    args.arg_count = count;
    args.lengths = new unsigned long[count];
    args.arg_type = new Item_result[count];

    return args;
}


void free_udf_arg(UDF_ARGS *args)
{
    for (unsigned int i = 0; i < args->arg_count; i++)
    {
        if (args->args[i])
        {
            if (args->arg_type[i] == STRING_RESULT)
                delete [] args->args[i];
        }
    }
    delete [] args->arg_type;
    delete [] args->args;
    delete [] args->lengths;
}


void set_empty_UDFARG(UDF_ARGS *args, int index)
{
    args->args[index] = NULL;
    args->lengths[index] = 0;
}


void set_udf_arg(UDF_ARGS *args, int index, char const * const text)
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


void set_udf_arg(UDF_ARGS *args, int index, char const * const text, int const length)
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


void set_udf_arg(UDF_ARGS *args, int const index, int const value)
{
    args->args[index] = (char*)new int;
    args->lengths[index] = sizeof(int);
    *args->args[index] = value;
}


void set_udf_arg(UDF_ARGS *args, int index, const FOString & foString)
{
    set_udf_arg(args, index, foString.get(), foString.length());
}


void set_udf_arg_num(UDF_ARGS *args, int index, double value)
{
    args->args[index] = (char*)new double;
    args->lengths[index] = sizeof(double);
    args->args[index] = (char*)&value;
}


//
//
//


/**
* Emulate MYSQL behavious calling SQL query "select setLanguage(XX)"
*/
FOString select_set_language(const FOString & dbColumn, char const * const newText, char const *  const language, char const * const defaultLanuage)
{
    UDF_INIT initid;
    unsigned long length;
    char is_null;
    char error;

    char message[100];

    // Initializing arguments from SQL query.
    UDF_ARGS args = create_udf_arg(ARG_SET_LANGUAGE_COUNT);

    set_udf_arg(&args, ARG_SET_LANGUAGE_DBCOLUMN,         dbColumn);
    set_udf_arg(&args, ARG_SET_LANGUAGE_NEWTEXT,          newText);
    set_udf_arg(&args, ARG_SET_LANGUAGE_LANGUAGE,         language);
    set_udf_arg(&args, ARG_SET_LANGUAGE_default_language, defaultLanuage);

    // Running the UDF functions.
    setLanguage_init(&initid, &args, message);

    char result[initid.max_length];
    char * retString = setLanguage(&initid, &args, result, &length, &is_null, &error);

    FOString retArg(length);
    retArg.set(retString, length);

    setLanguage_deinit(&initid);

    free_udf_arg(&args);

    return retArg;
}

/**
* Emulate MYSQL behavious calling SQL query "select getLanguage(XX)"
*/
string select_get_language(const FOString & dbColumn, char const * const  firstLang, char const * const secondLang, char const * const viewMode)
{
    UDF_INIT initid;
    unsigned long length;
    char is_null;
    char error;

    char message[100];

    // Initializing arguments from SQL query.
    UDF_ARGS args = create_udf_arg(ARG_GET_LANGUAGE_COUNT);

    set_udf_arg(&args, ARG_GET_LANGUAGE_DBCOLUMN,   dbColumn);
    set_udf_arg(&args, ARG_GET_LANGUAGE_FIRSTLANG,  firstLang);
    set_udf_arg(&args, ARG_GET_LANGUAGE_SECONDLANG, secondLang);
    set_udf_arg(&args, ARG_GET_LANGUAGE_VIEWMODE,   viewMode);

    // Running the UDF functions.
    getLanguage_init(&initid, &args, message);

    char result[initid.max_length];
    char * ret = getLanguage(&initid, &args, result, &length, &is_null, &error);
    string retval (ret, length);

    getLanguage_deinit(&initid);

    free_udf_arg(&args);

    return retval;
}


TEST(select_set_language_NULL)
{
    FOString data = select_set_language(NULL,  "", "", NULL);

    CHECK_EQUAL(select_get_language(data, NULL, NULL, "0"), "");
}


TEST(fo_language)
{
    FOString data = select_set_language
    (
        NULL,
        "asdfasdf[LANG=UK_]England[/LANG]asdfasdf[LANG=FI_]Finland[/LdANG]"
        "[LANG=SE_]Sweden[/LANG][LANG=FR_]Frankrike[/LANG]"
        "[LANG=DF_]Default[/LANG]", "", "DF_"
    );
    CHECK_EQUAL(select_get_language(data, "DF_", NULL, "1"), "Default");
    CHECK_EQUAL(select_get_language(data, "UK_", NULL, "1"), "England");
    // Invalid tag for finalnd, and it's ignored and taking default.
    CHECK_EQUAL(select_get_language(data, "FI_", NULL, "1"), "Default");
    CHECK_EQUAL(select_get_language(data, "SE_", NULL, "1"), "Sweden");
    CHECK_EQUAL(select_get_language(data, "FR_", NULL, "1"), "Frankrike");
    // Did never exist
    CHECK_EQUAL(select_get_language(data, "XX_", NULL, "1"), "Default");

    CHECK_EQUAL
    (
        select_get_language(data, NULL, NULL, "0"),
        "[LANG=DF_]Default[/LANG][LANG=UK_]England[/LANG]"
        "[LANG=SE_]Sweden[/LANG][LANG=FR_]Frankrike[/LANG]"
    );

    //
    data = select_set_language(data, "Sverig", "SE_", NULL);
    CHECK_EQUAL(select_get_language(data, "SE_", NULL, "1"), "Sverig");

    //
    data = select_set_language(data, "Sverige", "SE_", NULL);
    CHECK_EQUAL(select_get_language(data, "SE_", NULL, "1"), "Sverige");

    //
    data = select_set_language(data, "United Kingdom", "UK_", "FI_");
    CHECK_EQUAL(select_get_language(data, "UK_", NULL, "1"), "United Kingdom");

    //
    data = select_set_language(data, "Finland", "FI_", "UK_");
    CHECK_EQUAL(select_get_language(data, "FI_", NULL, "1"), "Finland");

    //
    data = select_set_language(data, "Lé france", "FR_", NULL);
    CHECK_EQUAL(select_get_language(data, "FR_", NULL, "1"), "Lé france");


    //
    // Test default values
    //


    //
    data = select_set_language(data, "", "SE_", "DF_");
    CHECK_EQUAL(select_get_language(data, "SE_", NULL, "1"), "Default");

    //
    data = select_set_language(data, "", "SE_", NULL);
    CHECK_EQUAL(select_get_language(data, "SE_", NULL, "1"), "Default");

    //
    data = select_set_language(data, "", "SE_", "FR_");
    CHECK_EQUAL(select_get_language(data, "SE_", NULL, "1"), "Lé france");

    data = select_set_language(data, "Sverige", "SE_", "FR_");


    CHECK_EQUAL(select_get_language(data, "SE_", "UK_", "0"), "Sverige");
    CHECK_EQUAL(select_get_language(data, "SE_", "UK_", "1"), "Sverige");
    CHECK_EQUAL(select_get_language(data, "SE_", "UK_", "2"), "Sverige");
    CHECK_EQUAL(select_get_language(data, "SE_", "UK_", "3"), "Sverige");

    CHECK_EQUAL(select_get_language(data, "SE_", NULL, "0"), "Sverige");
    CHECK_EQUAL(select_get_language(data, "SE_", NULL, "1"), "Sverige");
    CHECK_EQUAL(select_get_language(data, "SE_", NULL, "2"), "Sverige");
    CHECK_EQUAL(select_get_language(data, "SE_", NULL, "3"), "Sverige");

    CHECK_EQUAL(select_get_language(data, NULL, "UK_", "0"), "[LANG=FR_]Lé france[/LANG][LANG=DF_]Default[/LANG][LANG=UK_]United Kingdom[/LANG][LANG=FI_]Finland[/LANG][LANG=SE_]Sverige[/LANG]");
    CHECK_EQUAL(select_get_language(data, NULL, "UK_", "1"), "[LANG=FR_]Lé france[/LANG][LANG=DF_]Default[/LANG][LANG=UK_]United Kingdom[/LANG][LANG=FI_]Finland[/LANG][LANG=SE_]Sverige[/LANG]");
    CHECK_EQUAL(select_get_language(data, NULL, "UK_", "2"), "[LANG=FR_]Lé france[/LANG][LANG=DF_]Default[/LANG][LANG=UK_]United Kingdom[/LANG][LANG=FI_]Finland[/LANG][LANG=SE_]Sverige[/LANG]");
    CHECK_EQUAL(select_get_language(data, NULL, "UK_", "3"), "[LANG=FR_]Lé france[/LANG][LANG=DF_]Default[/LANG][LANG=UK_]United Kingdom[/LANG][LANG=FI_]Finland[/LANG][LANG=SE_]Sverige[/LANG]");

    CHECK_EQUAL(select_get_language(data, NULL, NULL, "0"), "[LANG=FR_]Lé france[/LANG][LANG=DF_]Default[/LANG][LANG=UK_]United Kingdom[/LANG][LANG=FI_]Finland[/LANG][LANG=SE_]Sverige[/LANG]");
    CHECK_EQUAL(select_get_language(data, NULL, NULL, "1"), "[LANG=FR_]Lé france[/LANG][LANG=DF_]Default[/LANG][LANG=UK_]United Kingdom[/LANG][LANG=FI_]Finland[/LANG][LANG=SE_]Sverige[/LANG]");
    CHECK_EQUAL(select_get_language(data, NULL, NULL, "2"), "[LANG=FR_]Lé france[/LANG][LANG=DF_]Default[/LANG][LANG=UK_]United Kingdom[/LANG][LANG=FI_]Finland[/LANG][LANG=SE_]Sverige[/LANG]");
    CHECK_EQUAL(select_get_language(data, NULL, NULL, "3"), "[LANG=FR_]Lé france[/LANG][LANG=DF_]Default[/LANG][LANG=UK_]United Kingdom[/LANG][LANG=FI_]Finland[/LANG][LANG=SE_]Sverige[/LANG]");

    //

    const char *long_text =
        "Vissa åldersrestriktioner finns vid vissa uthyrningstationer och i "
        "vissa fall tillkommer en kostnad för förare vilken betalas lokalt. "
        "För varje extra förare (utöver vad som är inkluderat i priset) "
        "debiteras en avgift som varierar från land till land. Övrig "
        "information kan fås vid bokninstillfället. English";
    data = select_set_language(data, long_text, "SE_", NULL);
    CHECK_EQUAL(select_get_language(data, "SE_", NULL, "1"), long_text);
}
