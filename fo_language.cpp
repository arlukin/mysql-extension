/***************************************************************************
  Language functions
  ******************

	Alternative name: Fareoffice Dynamic Columns.

	Used to set multiple language texts in one mysql column. Now called
	foLanguageColumn.

	//

	Format of string saved in mysql

	NAME										SIZE		Description
	---------------------------
	validationcode					word		A word that defines that this is a foLanguageColumn. IE: 655536.
	version									byte		The current version of the format.
	numOfLanguage						byte		Number of different languages.
	indexpos								dword		Position where the index are saved.
	datapos									dword		Position where the texts are saved.
	defaultLanguage					word		Two character representation of default language.


	- Index data, repeated for each language -
	The first language in the index part are the default language.

	language								word		Two character representation of language.
	startpos								dword		Position where the text for this language is started. 0 is datapos. Excluding [LANG TAG.
	length									dword		Length of the text. Excluding [LANG TAG.

	datasträngen						blob		The maximum length of the mysql field.

	//
	setLanguage

	//
	getLanguage

	//
	getLanguageProperties(rule.description, 'validationcode');

	//
	delLanguage(rule.description, $language);

	// Examples/Tests

create table `foMaster`.`daniel` (    `id` int   NOT NULL AUTO_INCREMENT ,  `description` blob   NULL  , PRIMARY KEY ( `id` )  )

select setLanguage(NULL, '[LANG=UK_]JAG ÄR EN GAMMAL KRÅKA.[/LANG]', NULL, 'SE_') as productruleheading_value

truncate daniel;

insert into daniel (id, description) values(1, setLanguage(NULL, "Sverige", "SE_", ''));
update daniel set description = setLanguage(description, "England", "UK_", "FI_") where id = 1 ;
update daniel set description = setLanguage(description, "Finland", "FI_", "UK_") where id = 1;
update daniel set description = setLanguage(description, "Frankrike", "FR_", "") where id = 1;
update daniel set description = setLanguage(description, "Frankrike NEW", "FR_", "FI_") where id = 1;
update daniel set description = setLanguage(description, "Vissa åldersrestriktioner finns vid vissa uthyrningstationer och i vissa fall tillkommer en kostnad för förare vilken betalas lokalt. För varje extra förare (utöver vad som är inkluderat i priset) debiteras en avgift som varierar från land till land. Övrig information kan fås vid bokninstillfället. English", "US_", "") where id = 1;
update daniel set description = setLanguage(description, "", "EM_", "") where id = 1;
update daniel set description = setLanguage(description, "TEST", "EP_", NULL) where id = 1;

update daniel set description = setLanguage(description, "asdfasdf[LANG=UK_]England[/LADDNG][LANG=FI_]Finland[/LANG][LANG=SE_]Sverige[/LANG]
asdf
[LANG=FR_]Frankrike NEW[/LANG]asdfasdf", NULL, 'FR_') where id = 1;

select getLanguage(description, "SE_", "UK_",	0), description as cow from daniel
select getLanguage(description, "FI_", "UK_", 1) as cow from daniel
select getLanguage(description, "FR_", "UK_", 1) as cow from daniel
select getLanguage(description, "IS_", "UK_", 1) as cow from daniel
select getLanguage(description, "IS_", "MI_", 1) as cow from daniel
select getLanguage(description, "EM_", "UK_", 1) as cow from daniel
select getLanguage(description, "US_", "UK_", 1) as cow from daniel

select getLanguage(description, "", "",     	0) as cow from daniel
select getLanguage(description, "", "",     	1) as cow from daniel
select getLanguage(description, "", "",     	2) as cow from daniel
select getLanguage(description, "", "",     	3) as cow from daniel

select getLanguage(description, "SE_", NULL, 	0) as cow from daniel
select getLanguage(description, "SE_", NULL, 	1) as cow from daniel
select getLanguage(description, "SE_", NULL, 	2) as cow from daniel
select getLanguage(description, "SE_", NULL, 	3) as cow from daniel

select getLanguage(description, "NON", NULL, 	0) as cow from daniel
select getLanguage(description, "NON", NULL, 	1) as cow from daniel
select getLanguage(description, "NON", NULL, 	2) as cow from daniel
select getLanguage(description, "NON", NULL, 	3) as cow from daniel

select getLanguage(description, "NON", 'UK_', 	0) as cow from daniel
select getLanguage(description, "NON", 'UK_', 	1) as cow from daniel
select getLanguage(description, "NON", 'UK_', 	2) as cow from daniel
select getLanguage(description, "NON", 'UK_', 	3) as cow from daniel

select getLanguage(description, "NON", 'NON', 	0) as cow from daniel
select getLanguage(description, "NON", 'NON', 	1) as cow from daniel
select getLanguage(description, "NON", 'NON', 	2) as cow from daniel
select getLanguage(description, "NON", 'NON', 	3) as cow from daniel

select getLanguage(description, "FI_", NULL, 	0) as cow from daniel
select getLanguage(description, "FR_", NULL, 	0) as cow from daniel
select getLanguage(description, "IS_", NULL, 	3) as cow from daniel


SELECT BENCHMARK(100000,getLanguage(setLanguage('', "England", "UK", "FI"), "UK", NULL, 0));
SELECT BENCHMARK(100000,ENCODE("hello","goodbye"));

insert into daniel (description) values(setLanguage(NULL, "England\neng", "UK", ''));
select getLanguage(description, "UK", 'UK', 'N') as description from daniel order by description


----

update daniel set description = '1' where id = 1;
update daniel set description = '12' where id = 2;
update daniel set description = '123' where id = 3;
update daniel set description = '1234' where id = 4;
update daniel set description = '12345' where id = 5;
update daniel set txtdesc = description;

update daniel set description = setLanguage(NULL, description, "UK_", "UK_") ;
--where id=2;
SELECT * from daniel;

---

update productruledesc set description = setLanguage(NULL, description, "UK_", "UK_") where productrule_id = 1432
select getLanguage(description, "UK_", "UK_", 0) as ASDF, description  from productruledesc where productrule_id = 1432


***************************************************************************/

//
// foLanguageDataColumn
//
// We are assuming that a short is 2 bytes and a long is 4 bytes.
//
#define foLDC_LANGUAGESIZE 3	// Number of chars in the language code "SE_"

// Positions in the foLanguageColumn
#define foLDC_VALIDATION			 0	// Size 2 bytes
#define foLDC_VERSION					 2	// Size 1 byte
#define foLDC_NUMOFLANGUAGE		 3	// Size 2 bytes
#define foLDC_INDEXPOS				 5	// Size 1 long
#define foLDC_DATAPOS					 9	// Size 1 long
#define foLDC_DEFAULTLANGUAGE	13	// Size foLDC_LANGUAGESIZE

#define foLDC_HEADERSIZE			(13+foLDC_LANGUAGESIZE)
#define foLDC_INDEXSIZE				(8+foLDC_LANGUAGESIZE)	// 2 long, languagesize
#define foLDC_OPENTAGSIZE			(7+foLDC_LANGUAGESIZE)	// Number of bytes in [LANG=XX_]
#define foLDC_CLOSETAGSIZE		7	// Number of bytes in [/LANG]


typedef struct st_foLanguage
{
	unsigned short allocatedLanguageCount;	// Number of languages their is allocated space for.
	unsigned short languageCount;						// Number of languages
  char **language;												// The language length is foLDC_LANGUAGESIZE. 'SE_'
  char **data;
  unsigned int *dataLengths;
	unsigned int totalDataLength;	// Sum of the dataLengths
	char defaultLanguage[foLDC_LANGUAGESIZE];

} UDF_FOLANGUAGE;


// Header delen av det som sparas i databasen.
typedef struct st_foLanguageHeader
{
	unsigned short validationCode;
	char  version;
	char numOfLanguages;
	char reserved;
	unsigned int   indexPos;
	unsigned int   dataPos;
	char defaultLanguage[foLDC_LANGUAGESIZE];

} UDF_LANGUAGEHEADER;


/////////////////////////////////////////////////////////
//
// PRIVATE:
//
/////////////////////////////////////////////////////////
void debugPrintLanguageHeader(UDF_LANGUAGEHEADER const * const headerData)
{

	fprintf(stderr,"---------- foFunction->fo_language - Debug header print -------------\n");
	fprintf(stderr,	"validationCode %d\n",  headerData->validationCode);
	fprintf(stderr,	"version %d\n", 			  headerData->version);
	fprintf(stderr,	"reserved %d\n",  			headerData->reserved);
	fprintf(stderr,	"numOfLanguages %d\n",  headerData->numOfLanguages);
	fprintf(stderr,	"indexPos %d\n", 			  headerData->indexPos);
	fprintf(stderr,	"dataPos %d\n", 			  headerData->dataPos);
	fprintf(stderr,	"defaultLanguage %s\n", headerData->defaultLanguage);

	fprintf(stderr,"\n--             Address, size\n");
	fprintf(stderr,	"validationCode  %d, 			%d\n", (&headerData->validationCode), sizeof(headerData->validationCode));
	fprintf(stderr,	"version         %d, 			%d\n", (&headerData->version), 			 sizeof(headerData->version));
	fprintf(stderr,	"reserved			   %d, 			%d\n", (&headerData->reserved), sizeof(headerData->reserved));
	fprintf(stderr,	"numOfLanguages  %d, 			%d\n", (&headerData->numOfLanguages), sizeof(headerData->numOfLanguages));
	fprintf(stderr,	"indexPos        %d, 			%d\n", (&headerData->indexPos), 			 sizeof(headerData->indexPos));
	fprintf(stderr,	"dataPos         %d, 			%d\n", (&headerData->dataPos), 			 sizeof(headerData->dataPos));
	fprintf(stderr,	"defaultLanguage %d, 			%d\n", (&headerData->defaultLanguage),sizeof(headerData->defaultLanguage));

	fprintf(stderr,"---------------------------------------------------------------------\n");
}



int deleteLanguageArr(UDF_FOLANGUAGE * const foLanguage, char const * const language)
{
	for(unsigned int i=0;i<foLanguage->languageCount;i++)
	{
		if (strncmp(foLanguage->language[i], language, foLDC_LANGUAGESIZE) == 0)
		{
			foLanguage->totalDataLength -= foLanguage->dataLengths[i];
			delete foLanguage->data[i];
			delete foLanguage->language[i];

			foLanguage->languageCount--;
			for(int j=i;j<foLanguage->languageCount;j++)
			{
				foLanguage->data[j] = foLanguage->data[j+1];
				foLanguage->dataLengths[j] = foLanguage->dataLengths[j+1];
				foLanguage->language[j] = foLanguage->language[j+1];
			}
			return i;
		}
	}

	return -1;
}

//

void setLanguageArr(UDF_FOLANGUAGE * const foLanguage, char const * const language, char const * const text, int const length)
{
	// First delete if any earlier language exist,
	// and then replace that array position.
	deleteLanguageArr(foLanguage, language);

	if (length>0)
	{
		// If no where deleted, create a new array position.
		int index = foLanguage->languageCount;

		// Language
		foLanguage->language[index] = new char[foLDC_LANGUAGESIZE];
		memcpy(foLanguage->language[index], language, foLDC_LANGUAGESIZE);

		// Data
		foLanguage->dataLengths[index] = length;
		foLanguage->data[index] = new char[length];
		memcpy(foLanguage->data[index], text, length);
		foLanguage->totalDataLength += length;

		foLanguage->languageCount++;
		// Fix: Felhantering
		//if (foLanguage->languageCount > foLanguage->allocatedLanguageCount)
		//	error
	}
}

//

//void setLanguageArr(UDF_FOLANGUAGE * const foLanguage, char const * const language, char const * const text)
//{
//	setLanguageArr(foLanguage, language, text, strlen(text));
//}

//

void loadLanguageData( UDF_FOLANGUAGE * const foLanguage, char const * const dbColumn  )
{
	//
	//UDF_LANGUAGEHEADER *headerData = (UDF_LANGUAGEHEADER *)dbColumn;
	//debugPrintString(dbColumn, 22);
	//debugPrintLanguageHeader(headerData);


	// validationCode
	unsigned short * validationCode = (unsigned short*)(dbColumn+foLDC_VALIDATION);
	if (*validationCode == 65280)
	{
		// Version
		if (dbColumn[foLDC_VERSION] == 1)
		{
			if (foLanguage->languageCount+((unsigned long)dbColumn[foLDC_NUMOFLANGUAGE]) <= foLanguage->allocatedLanguageCount)
			{
				// DefaultLanguage
				memcpy(foLanguage->defaultLanguage, &dbColumn[foLDC_DEFAULTLANGUAGE], foLDC_LANGUAGESIZE);

				// Read index
				unsigned short * indexPos = (unsigned short*)(dbColumn+foLDC_INDEXPOS);
				unsigned short * dataPos  = (unsigned short*)(dbColumn+foLDC_DATAPOS);

				char const * currentReadPos = dbColumn+*indexPos;
				char const * startDataPos = (char const *)(dbColumn + *dataPos);
				char language[foLDC_LANGUAGESIZE];
				unsigned long startPos, length;
				for (int i=0;i<dbColumn[foLDC_NUMOFLANGUAGE];i++)
				{
					memcpy(&language, currentReadPos, foLDC_LANGUAGESIZE);
					currentReadPos += foLDC_LANGUAGESIZE;

					//memcpy(&startPos, currentReadPos, sizeof(unsigned long));
					startPos = *(unsigned long*)currentReadPos;
					currentReadPos += sizeof(unsigned long);

					//memcpy(&length,	currentReadPos, sizeof(unsigned long));
					length = *(unsigned long*)currentReadPos;
					currentReadPos += sizeof(unsigned long);

					setLanguageArr(foLanguage, language, startDataPos+startPos, length);
				}
			}
			// felhantering
			//else
			//{
			//}
		}
	}
}

void parseLanguageData( UDF_FOLANGUAGE * const foLanguage, char const * const newText, unsigned long length)
{
	char * newTextN = new char[length+1];
	memcpy(newTextN, newText, length);
	newTextN[length] = 0;

	char * newTextNRead = newTextN;
	char * openTagPos, *closeTagPos, * language;

	openTagPos = strstr(newTextNRead, "[LANG=");
	while(openTagPos != NULL)
	{
		newTextNRead = openTagPos+(foLDC_OPENTAGSIZE-foLDC_LANGUAGESIZE-1);
		language = newTextNRead;
		newTextNRead +=foLDC_LANGUAGESIZE;

		if (*(newTextNRead) == ']')
		{
			newTextNRead +=1;
			closeTagPos = strstr(newTextNRead, "[/LANG]");
		}
		else
			closeTagPos = NULL;

		openTagPos = strstr(newTextNRead, "[LANG=");

		if (openTagPos == NULL || (closeTagPos != NULL && closeTagPos < openTagPos))
		{
			setLanguageArr(foLanguage, language, newTextNRead, closeTagPos-newTextNRead);
		}
	}

	delete newTextN;
}


//

int getLanguagePosition(UDF_FOLANGUAGE const * const foLanguage, char const * const language)
{
	for(unsigned int i=0;i<foLanguage->languageCount;i++)
	{
		if (strncmp(foLanguage->language[i], language, foLDC_LANGUAGESIZE) == 0)
			return i;
	}

	return -1;
}

//

void writeLanguageData(cfoString * foString, int * indexWPos, int * dataWPos, int dataStartPos,
											 char const * const language, char const * const text, int const length)
{
	// IndexArea - language
	foString->modString(indexWPos, language, foLDC_LANGUAGESIZE);

	// IndexArea - startPos
	foString->modString(indexWPos, (unsigned long)(*dataWPos+foLDC_OPENTAGSIZE-dataStartPos));

	// IndexArea - length
	foString->modString(indexWPos, (unsigned long)(length));

	// DataArea - Start tag [LANG=XX]
	foString->modString(dataWPos, "[LANG=", 6);
	foString->modString(dataWPos, language, foLDC_LANGUAGESIZE);
	foString->modString(dataWPos, "]", 1);

	// DataArea - Data
	foString->modString(dataWPos, text, length);

	// DataArea - End tag [/LANG]
	foString->modString(dataWPos, "[/LANG]", 7);
}

//

void saveLanguageData(UDF_INIT *initid, unsigned long *length, UDF_FOLANGUAGE *foLanguage)
{
	if (foLanguage->languageCount == 0)
	{
		*length = 0;
	}
	else
	{
		unsigned long dataStartPos = foLDC_HEADERSIZE+(foLanguage->languageCount*foLDC_INDEXSIZE);
		*length = dataStartPos+foLanguage->totalDataLength+(foLanguage->languageCount*(foLDC_OPENTAGSIZE+foLDC_CLOSETAGSIZE));

		cfoString * foString = (cfoString*)initid->ptr;
		foString->allocateBuffer(*length);

		//
		// buildHeader
		//
		foString->modString(foLDC_VALIDATION, (unsigned short)65280);
		foString->modString(foLDC_VERSION, (char)1);
		foString->modString(foLDC_NUMOFLANGUAGE, foLanguage->languageCount);
		foString->modString(foLDC_INDEXPOS, (unsigned long)foLDC_HEADERSIZE);
		foString->modString(foLDC_DATAPOS, (unsigned long)dataStartPos);

		//
		// buildIndex
		//
		int indexWPos = foLDC_HEADERSIZE;
		int dataWPos = dataStartPos;

		if (foLanguage->defaultLanguage[0] != 0)
		{
			int langPos = getLanguagePosition(foLanguage, foLanguage->defaultLanguage);

			// Language didnt exist.
			if (langPos == -1)
				foLanguage->defaultLanguage[0] = 0;
			else
				writeLanguageData(foString, &indexWPos, &dataWPos, dataStartPos, foLanguage->defaultLanguage, foLanguage->data[langPos], foLanguage->dataLengths[langPos]);
		}

		for(unsigned int i=0;i<foLanguage->languageCount;i++)
		{
			if (strncmp(foLanguage->language[i], foLanguage->defaultLanguage, foLDC_LANGUAGESIZE) != 0)
				writeLanguageData(foString, &indexWPos, &dataWPos, dataStartPos,  foLanguage->language[i], foLanguage->data[i], foLanguage->dataLengths[i]);

			// Take first language as default language.
			if (foLanguage->defaultLanguage[0] == 0)
				memcpy(foLanguage->defaultLanguage, foLanguage->language[i], foLDC_LANGUAGESIZE);
		}

		//defaultLanguage
		foString->modString(foLDC_DEFAULTLANGUAGE, foLanguage->defaultLanguage, foLDC_LANGUAGESIZE);
	}
}

//

void initFoLanguage(UDF_FOLANGUAGE * foLanguage)
{
	foLanguage->allocatedLanguageCount = 100;	// Number of languages to allocate space for.
	foLanguage->languageCount = 0;

	foLanguage->data = new char*[foLanguage->allocatedLanguageCount];
	foLanguage->dataLengths = new unsigned int[foLanguage->allocatedLanguageCount];
	foLanguage->language = new char*[foLanguage->allocatedLanguageCount];
	foLanguage->totalDataLength = 0;
	foLanguage->defaultLanguage[0] = 0;	// No default language.
}

//

void deInitFoLanguage(UDF_FOLANGUAGE * foLanguage)
{
	foLanguage->allocatedLanguageCount = 0;
	foLanguage->languageCount = 0;
	delete foLanguage->data;
	delete foLanguage->dataLengths;
	delete foLanguage->language;
	foLanguage->totalDataLength = 0;
	foLanguage->defaultLanguage[0] = 0;
}

//
// Modify: initid->ptr, initid->max_length, textLength
//
inline void createReturnString(UDF_INIT * const initid, char const * const dataAreaPos,
															 unsigned long const * const textStartPos, unsigned long &textLength,
															 char const * const firstLanguage, char const * const foundLanguage)
{
	cfoString * foString = (cfoString*)initid->ptr;

	if (textStartPos == NULL)
	{
		foString->setString(dataAreaPos, textLength);
	}
	else
	{
		unsigned long orginalTextLength = textLength;

		if (foundLanguage != NULL && textStartPos != NULL)
			textLength += (foLDC_OPENTAGSIZE+foLDC_CLOSETAGSIZE)*2;

		foString->allocateBuffer(textLength);
		int initidWritePos = 0;

		if (foundLanguage != NULL)
		{
			foString->modString(&initidWritePos, "[LANG=", 6);

			foString->modString(&initidWritePos, firstLanguage, foLDC_LANGUAGESIZE);

			foString->modString(&initidWritePos, "]", 1);

			foString->modString(&initidWritePos, "[/LANG]", foLDC_CLOSETAGSIZE);

			foString->modString(&initidWritePos, "[LANG=", 6);

			foString->modString(&initidWritePos, foundLanguage, foLDC_LANGUAGESIZE);

			foString->modString(&initidWritePos, "]", 1);
		}

		foString->modString(&initidWritePos, dataAreaPos+*textStartPos, orginalTextLength);

		if (foundLanguage != NULL)
		{
			foString->modString(&initidWritePos, "[/LANG]", foLDC_CLOSETAGSIZE);
		}
	}
}

/////////////////////////////////////////////////////////
//
// PUBLIC:
//
/////////////////////////////////////////////////////////

/***************************************************************************
	char* = setLanguage(dbColumn, newText, language, defaultLanguage)
	***********
	Concates a foLanguageColumn with a new language.

	Argument format
		[0]dbColumn					- "[LANGINDEX][LANG=SE]Hej världen[/LANG][LANG=UK]Hello world[/LANG]"
													can be null
		[1]newText					- "Moi mukulat" can be null, will reset the language in [2]language.
		[2]language					- "FI"
														If null, the [1]newText field must include the [LANG=xx] tag.
		[3]defaultLanguage	- "UK" can be null

	Return
		The concated string with the index.
		[...LANGINDEX....][LANG=UK]Hello world[/LANG][LANG=SE]Hej världen[/LANG][LANG=FI]Moi mukulat[/LANG]

	Example
		select setLanguage(NULL, "Moi mukulat]", "FI", "UK") as moo;

***************************************************************************/
#define ARG_SET_LANGUAGE_DBCOLUMN 		   	0
#define ARG_SET_LANGUAGE_NEWTEXT 					1
#define ARG_SET_LANGUAGE_LANGUAGE       	2
#define ARG_SET_LANGUAGE_DEFAULTLANGUAGE	3

#define ARG_SET_LANGUAGE_COUNT	4
my_bool setLanguage_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  if (args->arg_count < ARG_SET_LANGUAGE_COUNT)
  {
		// FIX: MYSQL_ERRMSG_SIZE så lång är message
    strcpy(message,"usage: char* = setLanguage(dbColumn, newText, language, defaultLanguage)");
    return 1;
  }
	else
	{
		args->arg_type[ARG_SET_LANGUAGE_DBCOLUMN] 				= STRING_RESULT;
		args->arg_type[ARG_SET_LANGUAGE_NEWTEXT] 					= STRING_RESULT;
		args->arg_type[ARG_SET_LANGUAGE_LANGUAGE]       	= STRING_RESULT;
		args->arg_type[ARG_SET_LANGUAGE_DEFAULTLANGUAGE] 	= STRING_RESULT;

		initid->decimals = 0;
		initid->maybe_null = 1;
		initid->max_length = 16000000;	// Can return a maximum of this.
		initid->ptr = (char *)new cfoString(255);	// Allocate 255 byte to begin with.

		return 0;
	}
}

//

void setLanguage_deinit(UDF_INIT *initid)
{
	delete initid->ptr;
}

//

//
// FIX: dbColumn innehåller 0, kanske stränghanteringen i mysql får spelet.
//
char *setLanguage(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	UDF_FOLANGUAGE foLanguage;
	initFoLanguage(&foLanguage);

	if (!emptyARG(args, ARG_SET_LANGUAGE_DBCOLUMN))
		loadLanguageData(&foLanguage, args->args[ARG_SET_LANGUAGE_DBCOLUMN]);

	if (!emptyARG(args, ARG_SET_LANGUAGE_LANGUAGE))
		setLanguageArr(&foLanguage, args->args[ARG_SET_LANGUAGE_LANGUAGE], args->args[ARG_SET_LANGUAGE_NEWTEXT], args->lengths[ARG_SET_LANGUAGE_NEWTEXT]);
	else
		parseLanguageData(&foLanguage, args->args[ARG_SET_LANGUAGE_NEWTEXT], args->lengths[ARG_SET_LANGUAGE_NEWTEXT]);

	if (!emptyARG(args, ARG_SET_LANGUAGE_DEFAULTLANGUAGE))
		memcpy(foLanguage.defaultLanguage, args->args[ARG_SET_LANGUAGE_DEFAULTLANGUAGE], foLDC_LANGUAGESIZE);

	saveLanguageData(initid, length, &foLanguage);
	deInitFoLanguage(&foLanguage);

	cfoString * foString = (cfoString*)initid->ptr;

	return foString->getString();

	/*
	FIX: Är nog snyggare så här.
	rule_value.value har idag '' som värde, så dom försvinner inte riktigt.
	Skriv också skript för att tabort/uppdatera dessa till null.

	if (*length == 0)
	{
		*is_null = true;
		return NULL;
	}
	else
	{
		return foString->getString();
	}
	*/
}

/***************************************************************************
	$text = getLanguage(rule.description, $firstLang, $secondLang, $bDefaultLang);
	***********
	Geta language from a foLanguageColumn.

	Argument format
		[0]dbColumn			- "[LANGINDEX][LANG=SE]Hej världen[/LANG][LANG=UK]Hello world[/LANG]"
										 	Can be null.
		[1]firstLang		- "SE"	Look for this lang first.
											If null, return all the languages with tags.
		[2]secondLang		- "FI"	If not firstLang are found, try to find this.
											Can be null, won't search for this lang if null
		[3]viewMode			- 0 - Only firstLang and secondLang
											1 - If none of the above are found and this is true
													look for the first langouage in the foLanguageColumn.
													return NULL if false.
											2 - Like mode 0, but if only secondLang is found return with tags [LANG=SE][/LANG][LANG=UK]English[/LANG]
											3 - Like mode 1, but if onlye secondLang or defaultLang is found return with tags.

	Return
		The wanted string, or with tags ie. [LANG=SE]COW[/LANG]

	Example
		select setLanguage(rule.rule_name, "FI", "UK", TRUE) as moo;

***************************************************************************/
#define ARG_GET_LANGUAGE_DBCOLUMN 		0
#define ARG_GET_LANGUAGE_FIRSTLANG 		1
#define ARG_GET_LANGUAGE_SECONDLANG   2
#define ARG_GET_LANGUAGE_VIEWMODE			3

#define ARG_GET_LANGUAGE_COUNT	4

my_bool getLanguage_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  if (args->arg_count < ARG_GET_LANGUAGE_COUNT)
  {
		// FIX: MYSQL_ERRMSG_SIZE så lång är message
    strcpy(message,"usage: char* = getLanguage(dbColumn, firstLanguage, secondLanguage, defaultLanguage)");
    return 1;
  }
	else
	{
		args->arg_type[ARG_GET_LANGUAGE_DBCOLUMN] 		= STRING_RESULT;
		args->arg_type[ARG_GET_LANGUAGE_FIRSTLANG] 		= STRING_RESULT;
		args->arg_type[ARG_GET_LANGUAGE_SECONDLANG]   = STRING_RESULT;
		args->arg_type[ARG_GET_LANGUAGE_VIEWMODE]			= INT_RESULT;

		initid->decimals = 0;
		initid->maybe_null = 1;
		initid->max_length = 16000000;	// Can return a maximum of this.
		initid->ptr = (char *)new cfoString(255);	// Allocate 255 byte to begin with.
		return 0;
	}
}

//

void getLanguage_deinit(UDF_INIT *initid)
{
	delete initid->ptr;
}

//

//
// FIX: dbColumn innehåller 0, kanske stränghanteringen i mysql får spelet.
//
char *getLanguage(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	if (!emptyARG(args, ARG_GET_LANGUAGE_DBCOLUMN))
	{
		//
		// Get arguments from the function call.
		//
		char *dbColumn = args->args[ARG_GET_LANGUAGE_DBCOLUMN];

		unsigned short * validationCode = (unsigned short*)(dbColumn+foLDC_VALIDATION);
		if (*validationCode == 65280)
		{
			char *firstLanguage  = NULL;
			char *secondLanguage = NULL;

			if (!emptyARG(args, ARG_GET_LANGUAGE_FIRSTLANG))
				firstLanguage = args->args[ARG_GET_LANGUAGE_FIRSTLANG];

			if (!emptyARG(args, ARG_GET_LANGUAGE_SECONDLANG))
				secondLanguage = args->args[ARG_GET_LANGUAGE_SECONDLANG];

			char *defaultLanguage = NULL;

			// Should we use the default value from the foLanuageColumn.
			if (*args->args[ARG_GET_LANGUAGE_VIEWMODE] == 1 || *args->args[ARG_GET_LANGUAGE_VIEWMODE] == 3)
				defaultLanguage = dbColumn+foLDC_DEFAULTLANGUAGE;

			// Set easier to user variables
			unsigned long * dbColumnDataPos = (unsigned long*)(dbColumn+foLDC_DATAPOS);
			unsigned long * dbColumnIndexPos = (unsigned long *)(dbColumn+foLDC_INDEXPOS);

			unsigned long * textStartPos = NULL;
			unsigned long textLength = 0;
			bool languageFound = false;
			char * foundLanguage = NULL;

			//
			// Read the data
			//
			if (firstLanguage == NULL)
			{
				textStartPos = NULL;
				textLength = args->lengths[ARG_GET_LANGUAGE_DBCOLUMN]-*dbColumnDataPos;
				languageFound = true;
			}
			else
			{
				char * indexReadPos = dbColumn+*dbColumnIndexPos;
				for (int i=0;i<(char)(dbColumn[foLDC_NUMOFLANGUAGE]);i++)
				{
					// indexReadPos points to language
					if (firstLanguage != NULL && strncmp(indexReadPos, firstLanguage, foLDC_LANGUAGESIZE) == 0)
					{
						textStartPos = (unsigned long*)(indexReadPos+foLDC_LANGUAGESIZE);
						textLength = *((unsigned long*)(indexReadPos+foLDC_LANGUAGESIZE+4));
						foundLanguage = NULL;
						languageFound = true;
						break;
					}
					else if ((secondLanguage != NULL && strncmp(indexReadPos, secondLanguage, 2) == 0) ||
									 (defaultLanguage != NULL && languageFound == false && strncmp(indexReadPos, defaultLanguage, 2) == 0))
					{
						textStartPos = (unsigned long*)(indexReadPos+foLDC_LANGUAGESIZE);
						textLength = *((unsigned long*)(indexReadPos+foLDC_LANGUAGESIZE+4));
						languageFound = true;

						if (*args->args[ARG_GET_LANGUAGE_VIEWMODE] == 2 || *args->args[ARG_GET_LANGUAGE_VIEWMODE] == 3)
							foundLanguage = indexReadPos;
					}

					// Jump to next index "row"
					indexReadPos+=foLDC_INDEXSIZE;
				}
			}

			if (languageFound == true)
				createReturnString(initid, dbColumn+*dbColumnDataPos, textStartPos, textLength, firstLanguage, foundLanguage);

			*length = textLength;
		}
		else
		{
			*length = args->lengths[ARG_GET_LANGUAGE_DBCOLUMN];

			createReturnString(initid, dbColumn, 0, *length, NULL, NULL);
		}
	}
	else
	{
		*length = 0;
	}

	cfoString * foString = (cfoString*)initid->ptr;
	return foString->getString();
}
