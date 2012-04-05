/***************************************************************************
** validateMinMaxDate function.

[0]outdate			- "2002-01-10"
[1]indate				- "2002-01-13"
[2]intervalmin	- "0,0,0"
[3]intervalmax	- "1,0,0"
[4]WeekdayFrom		- 5
[5]WeekdayTo		- 7

***************************************************************************/
#define ARG_MINMAX_OUTDATE     		0
#define ARG_MINMAX_INDATE     		1
#define ARG_MINMAX_WEEKDAYSTAYMIN 2
#define ARG_MINMAX_WEEKDAYSTAYMAX 3
#define ARG_MINMAX_WEEKDAYFROM  	4
#define ARG_MINMAX_WEEKDAYTO   		5

#define ARG_MINMAX_COUNT	  			6

my_bool validateMinMaxDate_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (args->arg_count < ARG_MINMAX_COUNT)
	{
		strcpy(message,"usage: validateMinMaxDate(outDate, inDate, weekdayStayMin(NULL), weekdayStayMax(NULL), WeekdayFrom(NULL), WeekdayTo(NULL))");
		return 1;
	}

	args->arg_type[ARG_MINMAX_OUTDATE] 				=  STRING_RESULT ;
	args->arg_type[ARG_MINMAX_INDATE]  				=  STRING_RESULT ;
	args->arg_type[ARG_MINMAX_WEEKDAYSTAYMIN] =  STRING_RESULT ;
	args->arg_type[ARG_MINMAX_WEEKDAYSTAYMAX] =  STRING_RESULT ;
	args->arg_type[ARG_MINMAX_WEEKDAYFROM] 		=  STRING_RESULT ;
	args->arg_type[ARG_MINMAX_WEEKDAYTO] 			=  STRING_RESULT ;

	initid->max_length=8;
	// Debug
 	//initid->max_length=255;
 	return 0;
}

/*
	Nya versionen av validateMinMax st�der
	minDayStay, maxDayStay
	Weekday Out, Weekday in

	Min, Max, Weekday in, Weekday out best�r av en eller flera sektioner.
	Varje sektion avdelas med | (or),  &(and) eller :(nya min/max). Weekday klarar bara av | och :.

	Varje sektion kan sedan vara en veckodag, eller en period. t.ex. 1 (m�ndag), 2-3 (tisdag-onsdag).
	Dessa veckodagar fungerar p� b�da weekday och min/max.

	Min/max klarar �ven av ett min eller max antal dagar 0,0,1 (1 dag), 0,1,0 (1 m�nad)
	1d/5d Betyder att man t.ex. �ker m�ndag och hem tidigast tisdag och senast hem l�rdag.

	Vid fr-su/1m.. M�ste man �ka ut senast p� torsdagen och f�r �ka hem tidigast p� s�ndagen.

	Vid en s�kning med fr-su m�ste dessa dagar finnas med
	i intervallet mellan outDate+1 och inDate.

	Man m�ste �ka ut p� dagen innan minDay och f�r �ka hem p� maxDay.

------

  F�rklaring av MIN delen i MIN/MAX. Utdatum och Hemdatum visar minsta m�jliga
  datum range.

  + i MIN ska l�sas som AND/OCH. Till Exempel su+2d, �r allts� "sunday rule" OCH minst 2 dagar.

                  Utdatum      Hemdatum   Beskrivning
-/-               2004-02-15  2004-02-15  Ingen begr�nsning
0d/-              2004-02-15  2004-02-15  Ingen begr�nsning
1d/-              2004-02-15  2004-02-16  M�ste vara borta minst en natt. Eller kan det utryckas att man �r borta
                                          minst 1 dag exclusive utresedagen.
2d/-              2004-02-15  2004-02-17  M�ste vara borta minst en natt.
1m/-              2005-01-02  2005-02-02  M�ste vara borta minst en m�nad, ber�knarutifr�n
                                          hur m�nga dagar det �r i utdatums m�naden.
                                          Januari har 31 dagar, s� man m�ste vara borta minst 31 dagar.

1m/-              2005-02-02  2005-03-02  M�ste vara borta minst en m�nad, ber�knarutifr�n
                                          hur m�nga dagar det �r i utdatums m�naden.
                                          Februari har 28 dagar, s� man m�ste vara borta minst 28 dagar.
su/-              2004-11-13  2004-11-14  Den 14:e �r en s�ndag. M�ste vara borta minst en natt mot en s�ndag.
mo-fr/-           2004-11-14  2004-11-19  Den 14:E �r en s�ndag den 19:e en fredag.
                                          M�ste vara borta minst en natt mot m�ndag till och med natt mot fredag.
mo-fr,3d/-        2004-11-15  2004-11-18  M�ste vara borta minst en natt mot m�ndag till och med natt mot fredag.
                                          ELLER
                                          Vara borta tre n�tter.
                                            Denna min/max beh�ver egentligen inte mo-fr, f�r detta kommer alltid att
                                          �verlagras av 3D.
2d,su/-           2004-11-13  2004-11-14  M�ste vara borta minst 2 n�tter ELLER en natt mot s�ndag.
2d,su/-           2004-11-15  2004-11-17  M�ste vara borta minst 2 n�tter ELLER en natt mot s�ndag.
3d,fr-sa,sa-su/-  2004-11-15  2004-11-17  M�ste vara borta minst 3 n�tter
                  2004-11-11  2004-11-13  ELLER natt mot fredag till och med natt mot l�rdag.
                  2004-11-12  2004-11-14  ELLER natt mot l�rdag till och med natt mot s�ndag.
3d,sa,su/-        2004-11-15  2004-11-17  M�ste vara borta minst 3 n�tter
                  2004-11-12  2004-11-13  ELLER natt mot natt mot l�rdag.
                  2004-11-13  2004-11-14  ELLER natt mot natt mot s�ndag.
su+1d/-           2004-11-13  2004-11-14  M�ste vara borta minst 1 natt OCH minst en natt mot en s�ndag.
                                          �r man borta en natt mot en s�ndag, �r man ocks� borta minst 1 natt.
                                          S� +1d �r meningsl�st i detta fall.
su+2d/-           2004-11-12  2004-11-14  M�ste vara borta minst 2 n�tter OCH minst en natt mot en s�ndag.
                  2004-11-13  2004-11-15  S� detta ger tv� datum m�jligheter,
                                          Kan ocks� skrivas s� h�r "sa-su, su-mo".

su,2d/-           2004-11-13  2004-11-14  M�ste vara borta minst en natt mot en s�ndag ELLER minst 2 n�tter.
                  2004-11-15  2004-11-17  M�ste vara borta minst en natt mot en s�ndag ELLER minst 2 n�tter.

sa+1d,su+1d/-     2004-11-13  2004-11-14  Detta �r samma sak som "sa, su". M�ste vara borta minst en natt mot en s�ndag
                  2004-11-12  2004-11-13  ELLER vara borta minst en natt mot en l�rdag.

sa+2d,su+2d/-     2004-11-11  2004-11-13  M�ste vara borta minst 2 n�tter OCH minst en natt mot en l�rdag.
                  2004-11-12  2004-11-14  ELLER M�ste vara borta minst 2 n�tter OCH minst en natt mot en s�ndag.    BUGG: Denna tas inte med, endast "sa+2d"
                  2004-11-13  2004-11-15  Kan ocks� skrivas s� h�r "fr-sa, sa-su, su-mo"

sa,su,2d/-        2004-11-12  2004-11-13  M�ste vara borta minst en natt till l�rdag
                  2004-11-13  2004-11-14  ELLER m�ste vara borta minst en natt till s�ndag
                  2004-11-15  2004-11-17  ELLER m�ste vara borta minst 2 n�tter.

-----
F�rklaring av MAX delen i MIN/MAX. Utdatum och Hemdatum visar h�gsta m�jliga resl�ngd.

                  Utdatum      Hemdatum    Beskrivning
-/1d              2004-11-15  2004-11-16   F�r vara borta max en natt.
-/1m              2005-01-02  2005-02-02   F�r vara borta max en m�nad, ber�knarutifr�n
                                           hur m�nga dagar det �r i utdatums m�naden.
                                           Januari har 31 dagar, s� man f�r vara borta max 31 dagar.
-/1y              2005-01-02  2006-01-02   F�r vara borta max ett �r, ber�knarutifr�n
                                           antal dagar p� ett �r.

------------

	Nedan visas valida minmaxar med tillh�rande anrop av validateMinMaxDate

-- Om ej indatum angivits.
	select validateMinMaxDate("2002-07-03", NULL, 				"1,0,0",	"0,0,1", 	"1|2|3",			 NULL); select " ";
	select validateMinMaxDate("2002-07-03", NULL, 				NULL, 	 	NULL, 		"1|2|3",			 NULL); select " ";
	select validateMinMaxDate("2002-07-03", NULL, 				NULL, 	 	NULL, 		"1|2|3",			 NULL); select " ";

	-- Null v�rden.
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,0", "1,0,0", NULL,					 NULL); select " ";
	select validateMinMaxDate("2002-07-03", "2002-07-05", NULL, 	 NULL, 		NULL,					 NULL); select " ";

	-- Weekday out/in- outDate/inDate m�ste vara en av dessa dagar.
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,0", "1,0,0", "3", 					 "5"); select " ";
	select validateMinMaxDate("2002-07-07", "2002-07-08", "0,0,0", "1,0,0", "7", 					 "1"); select " ";
	select validateMinMaxDate("2002-07-07", "2002-07-08", "0,0,0", "1,0,0", "1|2|3:4|7", 	 "4|5:1|2|3"); select " ";
	select validateMinMaxDate("2002-07-01", "2002-07-07", "0,0,0", "1,0,0", "5|2|3:4|2|1", "4|5:1|2|3|7"); select " ";
	select validateMinMaxDate("2002-07-01", "2002-07-07", "0,0,0", "1,0,0", "5|1|3:4|2",   "4|7:2|3|7"); select " ";
	select validateMinMaxDate("2002-07-02", "2002-07-07", "0,0,0", "1,0,0", "5|1|2:4|2",   "4|3:2|3|7"); select " ";

	-- Min: 2d - M�ste vara borta tv� dagar exlusive utresedatum inklusive inresedatum, ie: 2002-07-03 -- 2002-07-05..
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,2",			 		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,4",			 		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,4|0,0,2", 		"1,0,0|1,0,0", 				"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- Max: 2d - F�r inte vara borta l�ngre �n tv� dagar exlusive utresedatum inklusive inresedatum. ie: 2002-07-03 -- 2002-07-05..
	-- En sektion i max f�r ej inneh�lla or |, bara kolon : ... ie: 0,0,2:0,0,4 ej 0,0,1|0,0,5:0,0,4
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,0",			 		"0,0,2", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,0",			 		"0,0,4", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,0:0,0,0", 		"0,0,2:0,0,4", 				"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- th/1y
	select validateMinMaxDate("2002-07-03", "2002-07-04", "4", 							"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- fr-su/1y
	select validateMinMaxDate("2002-07-04", "2002-07-07", "5-7", 						"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- mo,su,tu/1y
	select validateMinMaxDate("2002-07-01", "2002-07-02", "1|7|2",					"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-06", "2002-07-07", "1|7|2",					"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- fr-su, tu-we/1y
	select validateMinMaxDate("2002-07-04", "2002-07-07", "5-7|2-3",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-01", "2002-07-03", "5-7|2-3",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- fr-su, tu-we, mo, th, tu/1y
	select validateMinMaxDate("2002-07-01", "2002-07-02", "5-7|2-3|1|4|2",	"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- 3d, mo, tu-we/1Y
	select validateMinMaxDate("2002-06-30", "2002-07-01", "0,0,3|1|2-3",		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-02", "2002-07-05", "0,0,3|1|2-3",		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- 3d+tu/1y
	select validateMinMaxDate("2002-07-01", "2002-07-04", "0,0,3&2",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-06-30", "2002-07-03", "0,0,3&2",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- 7d/1m or 3d/4m
	select validateMinMaxDate("2002-07-01", "2002-07-04", "0,0,7:0,0,3",		"1,0,0:1,0,0", 				"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- we-we/1m
	select validateMinMaxDate("2002-07-02", "2002-07-10", "3-3",		"1,0,0", 	"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";


	select "FAIL";


	-- Invalid min/max

	-- Weekday
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,0",			 		"1,0,0", 							"2", 								"4"); 						select " ";
	select validateMinMaxDate("2002-07-07", "2002-07-08", "0,0,0",			 		"1,0,0", 							"7", 								"2"); 						select " ";
	select validateMinMaxDate("2002-07-01", "2002-07-07", "0,0,0", 					"1,0,0", 							"5|2|3:4|5|1", 			"4|5:1|2|3"); 		select " ";
	select validateMinMaxDate("2002-07-01", "2002-07-07", "0,0,0", 					"1,0,0", 							"5|1|3:4|5", 				"4|5:1|2|3|7"); 	select " ";

	-- Min
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,3",			 		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,5",			 		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,5|0,0,3", 		"1,0,0|1,0,0", 				"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- Max:
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,0",			 		"0,0,1", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,0",			 		"0,0,3", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,0:0,0,0", 		"0,0,2:0,0,3", 				"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- fr-su/1y
	select validateMinMaxDate("2002-07-07", "2002-07-07", "5-7", 						"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-05", "2002-07-07", "5-7", 						"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- mo,su,tu/1y
	select validateMinMaxDate("2002-07-02", "2002-07-02", "1-1|7|2",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-07", "2002-07-07", "1-1|7|2",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- fr-su, tu-we/1y
	select validateMinMaxDate("2002-07-05", "2002-07-07", "5-7|2-3",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-03", "2002-07-04", "5-7|2-3",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- 3d, mo, tu-we/1Y
	select validateMinMaxDate("2002-07-01", "2002-07-02", "0,0,3|1|2-3",		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-02", "2002-07-04", "0,0,3|1|2-3",		"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- 3d+tu/1y
	select validateMinMaxDate("2002-07-01", "2002-07-03", "0,0,3&2",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-06-30", "2002-07-02", "0,0,3&2",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";
	select validateMinMaxDate("2002-07-02", "2002-07-05", "0,0,3&2",				"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- 7d/1m or 3d/4m
	select validateMinMaxDate("2002-07-01", "2002-07-02", "0,0,7:0,0,3",		"1,0,0:1,0,0", 				"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

	-- we-we/1m
	select validateMinMaxDate("2002-07-02", "2002-07-03", "3-3",						"1,0,0", 							"1|2|3|4|5|6|7", 		"1|2|3|4|5|6|7"); select " ";

-- Om ej indatum angivits
	select validateMinMaxDate("2002-07-03", NULL, 				NULL, 	 NULL, 		"1|2",			 NULL); select " ";

	-- NOT SUPPORTED
	-- 7d/1m + 3d/4m
	-- Vi skriver idag ist�llet
	-- 7d/1m or 3d/4m. men detta hanterar inte c funktionen.

*/

// Modify: tm &dateTM, tm *dateTime
// M�naden �r zero based, allts� Januari �r nr 0, feb nr 1 osv.
//
int getDateTM(const UDF_ARGS * const args, const int &argIndex, tm **dateTM, time_t &dateTIME)
{
	int errCode;

	char *argPointerStart = NULL;

	if ((errCode = copyUDFARG(args, argIndex, &argPointerStart)) == EC_SUCCESS)
	{
		char *argPointer = argPointerStart;

		*dateTM = new tm;

		(*dateTM)->tm_sec   = 0;
		(*dateTM)->tm_min   = 0;
		(*dateTM)->tm_hour  = 0;
		(*dateTM)->tm_wday  = 0;
		(*dateTM)->tm_yday  = 0;
		(*dateTM)->tm_isdst = 0;
		(*dateTM)->tm_year = strtol( &argPointer[0], &argPointer, 10 )-1900;	// mktime tycker att tm_year = 0 �r �r 1900.
		(*dateTM)->tm_mon  = strtol( &argPointer[1], &argPointer, 10 )-1;
		(*dateTM)->tm_mday = strtol( &argPointer[1], &argPointer, 10 );

		if ((dateTIME = mktime(*dateTM)) == (time_t) -1)
			errCode = EC_INVALID_DATETM;
		else
			errCode = EC_SUCCESS;
	}

	delete argPointerStart;

	return errCode;
}

//

int validateWeekDay(char **bufferPointer, tm *dateTM)
{
	if (*bufferPointer != NULL && dateTM != NULL)
	{
		char dateDay;
		dateDay = (char)(dateTM->tm_wday+48);	// ascii 48 = char 0.

		// S�ndag i tm_wday = 0, M�ndag = 1. Men datum ifr�n
		// anropet till validateMinMax �r s�ndag = 7
		if (dateTM->tm_wday == 0)
			dateDay+=7;

		// G�r igenom en s�n h�r str�ng "1|3|5|6|7:3|5|6" som matas in i prices s� h�r "1,3,5-7/XXXX or 2,5-6/XXXX"..
		while(true)
		{
			switch (**bufferPointer)
			{
				case 0:
				case ':':
					return EC_NO_MATCHIN_WEEKDAY;

				case '|':
					break;

				default:
					if (**bufferPointer == dateDay)
						return EC_SUCCESS;
			}

			(*bufferPointer)++;
		}
	}
	else
	{
		return EC_SUCCESS;
	}
}

//

my_bool fetchNextSection(char **min, char **max)
{
	if (*min != NULL)
	{
		for(;**min != ':' && **min != 0;(*min)++);

		if (**min == ':')
			(*min)++;

		if (**min == 0)
			return false;
	}

	if (*max != NULL)
	{
		for(;**max != ':' && **max != 0;(*max)++);

		if (**max == ':' )
			(*max)++;

		if (**max == 0)
			return false;
	}

	return true;
}

//

int validateWeekDayPeriod(const UDF_ARGS * const args, tm *outTM, tm *inTM)
{
	int errCode;
	char *weekdayFromPointerStart = NULL;
	char *weekdayToPointerStart = NULL;

	// Weekday out
	if (emptyARG(args, ARG_MINMAX_WEEKDAYFROM) || (errCode = copyUDFARG(args, ARG_MINMAX_WEEKDAYFROM, &weekdayFromPointerStart)) == EC_SUCCESS)
	{
		// Weekday in
		if (emptyARG(args, ARG_MINMAX_WEEKDAYTO) || (errCode = copyUDFARG(args, ARG_MINMAX_WEEKDAYTO, &weekdayToPointerStart)) == EC_SUCCESS)
		{
			if (weekdayFromPointerStart != NULL || weekdayToPointerStart != NULL)
			{
				char *weekdayFromPointer = weekdayFromPointerStart;
				char *weekdayToPointer = weekdayToPointerStart;

				do
				{
					if ((errCode = validateWeekDay(&weekdayFromPointer, outTM)) == EC_SUCCESS)
						if ((errCode = validateWeekDay(&weekdayToPointer, inTM)) == EC_SUCCESS)
							break;

					errCode = fetchNextSection(&weekdayFromPointer, &weekdayToPointer) ? EC_SUCCESS : EC_NO_MORE_WEEKDAYS;
				} while (errCode == EC_SUCCESS);
			}
			else
				errCode = EC_SUCCESS;
		}
	}

	delete weekdayFromPointerStart;
	delete weekdayToPointerStart;

	return errCode;
}

//

// F�ruts�tter att det kommer in en day sektion "0,1,0", det g�r inget om det finns
// fler sektioner efter ("0,1,0|2-3&0,0,2").
//
// Modify: minMaxTIME modiferas till outTm + inmatat v�rde ifr�n validateMinMax.
//
int getDaySektionTIME(char **bufferPointer, const tm &outTM, time_t &minMaxTIME)
{
	int errCode;
	tm minMaxTM;

	minMaxTM.tm_sec   = 0;
	minMaxTM.tm_min   = 0;
	minMaxTM.tm_hour  = 0;
	minMaxTM.tm_wday  = 0;
	minMaxTM.tm_yday  = 0;
	minMaxTM.tm_isdst = 0;
	minMaxTM.tm_year  = outTM.tm_year + strtol( *bufferPointer, 		bufferPointer, 10 );
	minMaxTM.tm_mon   = outTM.tm_mon  + strtol( (*bufferPointer)+1, bufferPointer, 10 );
	minMaxTM.tm_mday  = outTM.tm_mday + strtol( (*bufferPointer)+1, bufferPointer, 10 );

	if ((minMaxTIME = mktime( &minMaxTM )) == (time_t)-1 )
		errCode = EC_INVALID_DATETIMEM;
	else
		errCode = EC_SUCCESS;

	return errCode;
}

// Det kommer endast att komma in "0,1,0/0,1,0" sektions typen.
// Obs. bara en dagsperiod per / sektion.
//
// Modify: bufferPointer kommer flyttas fram till n�sta /.
//
int validateMax(char **bufferPointer, const tm *outTM, const time_t &inTIME)
{
	int errCode;
	time_t maxTIME;

	if(**bufferPointer != 0)
	{
		if ((errCode = getDaySektionTIME(bufferPointer, *outTM, maxTIME)) == EC_SUCCESS)
		{
			// (utdatum + intervalMax) >= indatum
			if (maxTIME >= inTIME)
				errCode = EC_SUCCESS;
			else
				errCode = EC_INVALID_MAX_SECTION;
		}
	}
	else
	{
		errCode = EC_NO_MORE_MAX;
	}

	return errCode;
}

//

enum sectionType {ST_DAYS = 1, ST_WEEKDAYS=2, ST_ONEWEEKDAY = 3};

// outTm �r ej referens p� denna funktion, d� den modifieras.
//
int validateWeekdayStay(const int &WeekdayFrom, const int &WeekdayTo, tm outTM, const time_t &inTIME)
{
	// R�kna ut datum f�r WeekdayStayFrom genom att plussa p�
	// antal dagar mellan utDatum och WeekdayStayFrom (exlusive utDatum dagen)

	// fredag till fredag = 0 (5-5=0)
	if (outTM.tm_wday == WeekdayFrom)
		outTM.tm_mday += 7;

	// torsdag till fredag = 1 (5-4=1)
	else if (outTM.tm_wday < WeekdayFrom)
		outTM.tm_mday += WeekdayFrom - outTM.tm_wday;

	// torsdag till tisdag = 5 (7-4+2)
	else
		outTM.tm_mday += 7-outTM.tm_wday+WeekdayFrom;

	// R�kna ut datum f�r WeekdayStayTo genom att plussa p�
	// antal dagar mellan WeekdayStayFrom och WeekdayStayTo
	if (WeekdayTo != -1)
	{
		// fredag till fredag = 7
		if (WeekdayFrom == WeekdayTo)
			outTM.tm_mday += 7;

		// torsdag till fredag = 1 (5-4=1)
		else if (WeekdayFrom < WeekdayTo)
			outTM.tm_mday += WeekdayTo - WeekdayFrom;

		// fredag till tisdag = 4 (7-5+2=4)
		else
			outTM.tm_mday += 7-WeekdayFrom+WeekdayTo;
	}

	int errCode;
	time_t outTime;
	if ((outTime = mktime( &outTM )) == (time_t)-1)
	{
		errCode = EC_INVALID_WEEKDAYTIME;
	}
	else
	{
		if (outTime <= inTIME)
			errCode = EC_VALID_MIN_SECTION;
		else
			errCode = EC_INVALID_MIN_SECTION;
	}

	return errCode;
}

// Modiferar sectionStart
//
int validateSektion(const sectionType &type, char *sectionStart, const tm &outTM, const time_t &inTIME)
{
	int errCode;

	switch (type)
	{
		case ST_ONEWEEKDAY:
		{
			int Weekday;
			Weekday = strtol( &sectionStart[0], &sectionStart, 10 );

			errCode = validateWeekdayStay(Weekday, -1, outTM, inTIME);
		}
		break;

		case ST_WEEKDAYS:
		{
			int WeekdayFrom, WeekdayTo;
			WeekdayFrom = strtol( &sectionStart[0], &sectionStart, 10 );
			WeekdayTo   = strtol( &sectionStart[1], &sectionStart, 10 );

			errCode = validateWeekdayStay(WeekdayFrom, WeekdayTo, outTM, inTIME);
		}
		break;

		case ST_DAYS:
		{
			time_t sektionTIME;

			if ((errCode = getDaySektionTIME(&sectionStart, outTM, sektionTIME)) == EC_SUCCESS)
			{
				//  (utdatum + intervalMin) <= indatum
				if (sektionTIME <= inTIME)
					errCode = EC_VALID_MIN_SECTION;
				else
					errCode = EC_INVALID_MIN_SECTION;
			}
		}
		break;
	}

	return errCode;
}

//
// Inkommande arg typ "0,1,0|1|5|7-7|3-2&4|0,1,2/0,1,0"
int validateMin(char **bufferPointer, tm *outTM, time_t inTIME)
{
	int errCode;
	char *sectionStart = *bufferPointer;

	sectionType type = ST_ONEWEEKDAY;

	while(true)
	{
		switch (**bufferPointer)
		{
			case ',':
				type = ST_DAYS;
				break;

			case '-':
				type = ST_WEEKDAYS;
				break;

			case 0:
			case ':':
			case '&':
			case '|':
				errCode = validateSektion(type, sectionStart, *outTM, inTIME);

				if (errCode==EC_VALID_MIN_SECTION && **bufferPointer != '&')
				{
					return EC_SUCCESS;
				}
				else if (errCode != EC_VALID_MIN_SECTION && **bufferPointer == '&')
				{
					return EC_INVALID_MIN_SECTION;
				}
				else if (**bufferPointer == 0)
				{
					return EC_NO_MORE_MIN;
				}
				else if (**bufferPointer == ':')
				{
					return EC_INVALID_MIN_SECTION;
				}
				else
				{
					type = ST_ONEWEEKDAY;
					sectionStart = *bufferPointer+1;
				}
				break;
		}

		(*bufferPointer)++;
	}
}

// Debug
//char *validateMinMaxDate(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
double validateMinMaxDate(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	int errCode;
	tm *outTM = NULL;
	tm *inTM = NULL;
	time_t dummyTIME, inTIME;

	// outDate
	if ((errCode = getDateTM(args, ARG_MINMAX_OUTDATE, &outTM, dummyTIME)) == EC_SUCCESS)
	{
		// inDate
		if (emptyARG(args, ARG_MINMAX_INDATE) || ((errCode = getDateTM(args, ARG_MINMAX_INDATE, &inTM, inTIME)) == EC_SUCCESS))
		{
			if ((errCode = validateWeekDayPeriod(args, outTM, inTM)) == EC_SUCCESS)
			{
				if (inTM != NULL && !emptyARG(args, ARG_MINMAX_WEEKDAYSTAYMIN) && !emptyARG(args, ARG_MINMAX_WEEKDAYSTAYMAX))
				{
					char *minPointerStart = NULL;
					char *maxPointerStart = NULL;

					// intervall min
					if ((errCode = copyUDFARG(args, ARG_MINMAX_WEEKDAYSTAYMIN, &minPointerStart)) == EC_SUCCESS)
					{
						// intervall max
						if ((errCode = copyUDFARG(args, ARG_MINMAX_WEEKDAYSTAYMAX, &maxPointerStart)) == EC_SUCCESS)
						{
							char *minPointer = minPointerStart;
							char *maxPointer = maxPointerStart;
							do
							{
								if ((errCode = validateMax(&maxPointer, outTM, inTIME)) == EC_SUCCESS)
									if ((errCode = validateMin(&minPointer, outTM, inTIME)) == EC_SUCCESS)
										break;

								errCode = fetchNextSection(&minPointer, &maxPointer) == true ? EC_SUCCESS : EC_NO_MORE_MINORMAX;
							} while (errCode == EC_SUCCESS);
						}
					}
					delete minPointerStart;
					delete maxPointerStart;
				}
				else
					errCode = EC_SUCCESS;
			}
		}
	}

	delete outTM;
	delete inTM;
	return errCode;
}
