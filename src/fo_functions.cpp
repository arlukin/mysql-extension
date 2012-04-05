/*
	Including FareOffice UDF functions for MYSQL
*/


#ifdef STANDARD
	#include <stdio.h>
	#include <string.h>
#else
	#include <my_global.h>		// myssql 4
	#include <my_sys.h>
#endif


#include <mysql.h>
#include <m_ctype.h>
#include <m_string.h>

#ifdef HAVE_DLOPEN
	/* These must be right or mysqld will not find the symbol! */
	extern "C"
	{
		my_bool calcPrice_init(UDF_INIT *, UDF_ARGS *args, char *message);
		double calcPrice(UDF_INIT *initid, UDF_ARGS *args, char *is_null,char *error);

		my_bool calcCost_init(UDF_INIT *, UDF_ARGS *args, char *message);
		double calcCost(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);

		my_bool validateMinMaxDate_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
		double validateMinMaxDate(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
	}

	#include "FoString.h"
	#include "FoDebug.cpp"
	#include "FoUdfFunctions.h"

	#include "FoCalc.cpp"
	#include "FoMinMax.cpp"

#endif /* HAVE_DLOPEN */
