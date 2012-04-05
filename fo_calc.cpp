//

char calcIncrementalFormula(double &basePrice, char *calc)
{
	while (calc[0] != '\0')
	{
		switch(calc[0])
		{
		 	case '*':
				basePrice *= strtod( &calc[1], &calc );
		 		break;
		 	case '-':
				basePrice -= strtod( &calc[1], &calc );
		 		break;
		 	case '+':
				basePrice += strtod( &calc[1], &calc );
		 		break;
		 	default:
		 		return 1;
		}
	}

	return 0;
}

//

char calcBaseFormula(double &basePrice, char *calc)
{
	double value = 0;
	double multiTemp;
	while (calc[0] != '\0')
	{
		switch(calc[0])
		{
			case '*':
					multiTemp = strtod( &calc[1], &calc);

					if (multiTemp < 1)
						value -= basePrice * (1-multiTemp);
					else
						value += basePrice * (multiTemp-1);
		 		break;
		 	case '-':
				value -= strtod( &calc[1], &calc );
		 		break;
		 	case '+':
				value += strtod( &calc[1], &calc );
		 		break;

		 	default:
		 		return 1;
		}
	}

	basePrice += value;

	return 0;
}

/***************************************************************************
** calcPrice function

[0]baseprice			- 5000 (real)
[1]formula				- "*1.2+34*23*5"
[2]treatyformula	- "*1+120"
[3]routeformula		- "*1+120"
[4]decimals 			- 2 (real)

select calcPrice(100, NULL,				NULL, 						NULL) 		as  "100";
select calcPrice(100, "*1.5+50*4",		NULL, 						NULL) 		as  "500";
select calcPrice(100, "*1.5+50*4-40", NULL, 						NULL) 		as  "460";
select calcPrice(100, "*1.5+50*4-40", "*2.5+4-50", 			NULL) 				as  "1104";
select calcPrice(100, "*1.5+50*4-40", "*2.5+4-50", 			"+600*2.2+600-.1")  as  "4349";


-- sto-lon(+50)-ams(+10%)-jfk
select calcPrice(100, "+50*1.1",		NULL, 						NULL) 					as  "160";

-- sto-lon(+50)-ams(-10%)-jfk
select calcPrice(100, "+50*0.9",		NULL, 						NULL) 					as  "140";

***************************************************************************/
#define ARG_PRICE_BASEPRICE     0
#define ARG_PRICE_ROUTEFORMULA 	1
#define ARG_PRICE_FORMULA       2
#define ARG_PRICE_TREATYFORMULA 3

#define ARG_PRICE_COUNT	  		4

my_bool calcPrice_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  if (args->arg_count < ARG_PRICE_COUNT)
  {
    strcpy(message,"usage: calcPrice(baseprice, routeformula(NULL), formula(NULL), treatyformula(NULL))");
    return 1;
  }

  args->arg_type[ARG_PRICE_BASEPRICE]     = REAL_RESULT;
  args->arg_type[ARG_PRICE_ROUTEFORMULA]  = STRING_RESULT;
  args->arg_type[ARG_PRICE_FORMULA]       = STRING_RESULT;
  args->arg_type[ARG_PRICE_TREATYFORMULA] = STRING_RESULT;

  initid->decimals = 0; //@todo: Ska var 5
  initid->maybe_null = 1;
  initid->max_length = 20;
  return 0;
}

//
double calcPrice(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
	double value = 0;

	if (!emptyARG(args, ARG_PRICE_BASEPRICE))
	{
		value = *((double*) args->args[ARG_PRICE_BASEPRICE]);

		if (!emptyARG(args, ARG_PRICE_ROUTEFORMULA))
		{
			char *routeFormula = NULL;
			copyUDFARG(args, ARG_PRICE_ROUTEFORMULA, &routeFormula);
			*error = calcBaseFormula(value, routeFormula);
			delete routeFormula;
		}

		if (*error == 0 && !emptyARG(args, ARG_PRICE_FORMULA))
		{
			char *priceFormula = NULL;
			copyUDFARG(args, ARG_PRICE_FORMULA, &priceFormula);
			*error = calcIncrementalFormula(value, priceFormula);
			delete priceFormula;
		}

		if (*error == 0 && !emptyARG(args, ARG_PRICE_TREATYFORMULA))
		{
			char *treatyFormula = NULL;
			copyUDFARG(args, ARG_PRICE_TREATYFORMULA, &treatyFormula);
			*error = calcIncrementalFormula(value, treatyFormula);
			delete treatyFormula;
		}
	}
	else
	{
		*error=1;
	}

	if (*error==1)
		*is_null=1;

	return value;
}

//

/***************************************************************************
** calcCost function.

[0]baseprice			- 5000 (real)
[1]cost					  - 500 (real)
[2]formula				- "*1.2+34*23*5"
[3]commision			- 500
[4]treatyformula	- "*1+120"
[5]routeformula		- "*1+320"
[6]useBasePrice		- NULL
[7]decimals 			- 2 (real)


select calcCost(100, 200, "*2+500*1.5", NULL, 	NULL, 					NULL) as  "1000";
select calcCost(100, 200, "*2+500*1.5", "*0.8", NULL, 					NULL) as  "800";
select calcCost(100, 200, "*2+500*1.5", "*0.8", "*1.5+200*1.2",	NULL) as  "1680";
select calcCost(600, 600, "*1", 				"*1", 	"*.82", 				NULL) as  "492";

select calcCost(100, 200, "*2+500*1.5", NULL, 	NULL, 					1) as  "750";
select calcCost(100, 200, "*2+500*1.5", "*0.8", NULL, 					1) as  "600";
select calcCost(100, 200, "*2+500*1.5", "*0.8", "*1.5+200*1.2",	1) as  "1320";


***************************************************************************/
#define ARG_COST_BASEPRICE     	0
#define ARG_COST_COST     			1
#define ARG_COST_ROUTEFORMULA 	2
#define ARG_COST_FORMULA    		3
#define ARG_COST_COMMISION     	4
#define ARG_COST_USEBASEPRICE		5

#define ARG_COST_COUNT	  			6

my_bool calcCost_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  if (args->arg_count < ARG_COST_COUNT)
  {
    strcpy(message,"usage: calcCost(baseprice, cost, routeformula(NULL), formula(NULL), commision(NULL), useBasePrice(NULL))");
    return 1;
  }

  args->arg_type[ARG_COST_BASEPRICE]		 = REAL_RESULT;
  args->arg_type[ARG_COST_COST]					 = REAL_RESULT;
  args->arg_type[ARG_COST_FORMULA]			 = STRING_RESULT;
  args->arg_type[ARG_COST_COMMISION]		 = STRING_RESULT;
  args->arg_type[ARG_COST_USEBASEPRICE]	 = STRING_RESULT;


  initid->decimals = 2; //@todo: Ska var 5
  initid->maybe_null=1;
  initid->max_length = 20;
	// Debug
 	//initid->max_length=255;
  //initid->maybe_null=1;

  return 0;
}

// Debug
//char *calcCost(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
double calcCost(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
  double value;

	if (!emptyARG(args, ARG_COST_BASEPRICE) && !emptyARG(args, ARG_COST_USEBASEPRICE))
	{
		value = *((double*) args->args[ARG_COST_BASEPRICE]);
	}
	else if (!emptyARG(args, ARG_COST_COST))
	{
		value = *((double*) args->args[ARG_COST_COST]);
	}
	else
	{
		*error = 1;
	}

	if (*error == 0 && !emptyARG(args, ARG_COST_ROUTEFORMULA))
	{
		char *routeFormula = NULL;
		copyUDFARG(args, ARG_COST_ROUTEFORMULA, &routeFormula);
		*error = calcBaseFormula(value, routeFormula);
		delete routeFormula;
	}

	if (*error == 0 && !emptyARG(args, ARG_COST_FORMULA))
	{
		char *costFormula = NULL;
		copyUDFARG(args, ARG_COST_FORMULA, &costFormula);
		*error = calcIncrementalFormula(value, costFormula);
		delete costFormula;
	}

	if (*error == 0 && !emptyARG(args, ARG_COST_COMMISION))
	{
		char *costCommision = NULL;
		copyUDFARG(args, ARG_COST_COMMISION, &costCommision);
		*error = calcIncrementalFormula(value, costCommision);
		delete costCommision;
	}

	if (*error==1)
		*is_null=1;

  //return (int)value;
  return value;
}
