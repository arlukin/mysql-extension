#ifndef FO_UDF_H
#define FO_UDF_H

//ERRORCODELIST
//0     Resan är godkänd
#define EC_SUCCESS 0
#define EC_VALID_MIN_SECTION 1          // validateMinMaxDate

//2xx System felsmedelande, kan bero på fel indata.
#define EC_INVALID_UDF_ARG          204 // copyUDFARG - Invalid udf arg.

//
// Used by mysld to find the symbbol in the .so file.
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

#endif
