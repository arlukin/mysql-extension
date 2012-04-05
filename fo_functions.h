
	//ERRORCODELIST
	//0		Resan är godkänd
	#define EC_SUCCESS 0
	#define EC_VALID_MIN_SECTION 1			// validateMinMaxDate

	//1XX Är medelanden som talar om att datumet inte ligger i rätt period.
	#define EC_NO_MATCHIN_WEEKDAY		100
	#define EC_INVALID_MAX_SECTION	101	// validateMax				- ingen valid max.
	#define EC_NO_MORE_MAX					102	// validateMax				- ingen sektion var rätt.
	#define EC_INVALID_MIN_SECTION	103	// validateMin				- ingen valid min.
	#define	EC_NO_MORE_MIN					104	// validateMin				- ingen sektion var rätt
	#define	EC_NO_MORE_WEEKDAYS			105 // validateWeekDay		- ingen sektion var rätt
	#define	EC_NO_MORE_MINORMAX			106	// validateMin				- ingen sektion var rätt

	//2xx System felsmedelande, kan bero på fel indata.
	#define EC_INVALID_DATETM 			200	// getDateTM						- mktime invalid date.
	#define EC_INVALID_DATETIMEM 		202	// getDaySektionTIME		- mktime invalid date.
	#define EC_INVALID_WEEKDAYTIME	203	// validateWeekdayStay	- mktime invalid date.
	#define EC_INVALID_UDF_ARG			204	// copyUDFARG						- Invalid udf arg.

	// Kontrollerar ifall ett UDF argument är null eller "" (blankt).
	// Så som php funktionen empty().
	//
	inline bool emptyARG(const UDF_ARGS * const args, const int index)
	{
		if (args->args[index] == NULL || args->lengths[index] == 0)
			return true;
		else
			return false;
	}

	// Skapar en sträng i buffer, och kopierar argumenten ifrån UDFARG
	// Modify: char ** buffer
	//
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
