/*
	Including FareOffice UDF functions for MYSQL

	// DEBUG
	// Måste köras i db:n vid debug.
	CREATE FUNCTION calcCost RETURNS STRING SONAME "fo_functions.so";
	DROP function calcCost;
	CREATE FUNCTION calcCost RETURNS REAL SONAME "fo_functions.so";
*/


#ifdef STANDARD
	#include <stdio.h>
	#include <string.h>
#else
	#include <my_global.h>		// myssql 4
	//#include <global.h> 		// mysql 3
	#include <my_sys.h>
#endif


#include <mysql.h>
#include <m_ctype.h>
#include <m_string.h>

#ifdef HAVE_DLOPEN
	/* These must be right or mysqld will not find the symbol! */
	extern "C" {

		my_bool calcPrice_init(UDF_INIT *, UDF_ARGS *args, char *message);
		double calcPrice(UDF_INIT *initid, UDF_ARGS *args, char *is_null,char *error);

		my_bool calcCost_init(UDF_INIT *, UDF_ARGS *args, char *message);
		double calcCost(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
		//DEBUG
		//char *calcCost(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

		my_bool validateMinMaxDate_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
		double validateMinMaxDate(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
		//DEBUG
		//char *validateMinMaxDate(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

		my_bool setLanguage_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
		char *setLanguage(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);
		void setLanguage_deinit(UDF_INIT *initid);

		my_bool getLanguage_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
		char *getLanguage(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);
		void getLanguage_deinit(UDF_INIT *initid);

	}

	#include "cfoString.h"
	#include "fo_debug.cpp"

	#include "fo_functions.h"
	#include "fo_calc.cpp"
	#include "fo_minmax.cpp"
	#include "fo_language.cpp"

#endif /* HAVE_DLOPEN */




