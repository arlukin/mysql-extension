#ifndef FO_UDF_H
#define FO_UDF_H

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

#endif
