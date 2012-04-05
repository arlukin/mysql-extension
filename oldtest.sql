select calcPrice(100, "*1.5+50*4",		NULL, 						NULL) 					as  "700";
select calcPrice(100, "*1.5+50*4-40", NULL, 						NULL) 					as  "660";
select calcPrice(100, "*1.5+50*4-40", "*2.5+4-50/0.8", 	NULL) 					as  "2005";
select calcPrice(100, "*1.5+50*4-40", "*2.5+4-50/0.8", 	"+600*2.2+600") as  "6331";



select calcCost(100, 200, "*2+500*1.5", NULL, 	NULL, 					NULL, 			NULL) as  "1400";
select calcCost(100, 200, "*2+500*1.5", "*0.8", NULL, 					NULL, 			NULL) as  "1120";
select calcCost(100, 200, "*2+500*1.5", "*0.8", "*1.5+200*1.2",	NULL, 			NULL) as  "2256";
select calcCost(100, 200, "*2+500*1.5", "*0.8", "*1.5+200*1.2",	"+400*1.5", NULL) as  "3984";

select calcCost(100, 200, "*2+500*1.5", NULL, 	NULL, 					NULL, 			1) as  "950";
select calcCost(100, 200, "*2+500*1.5", "*0.8", NULL, 					NULL, 			1) as  "760";
select calcCost(100, 200, "*2+500*1.5", "*0.8", "*1.5+200*1.2",	NULL, 			1) as  "1608";
select calcCost(100, 200, "*2+500*1.5", "*0.8", "*1.5+200*1.2",	"+400*1.5", 1) as  "3012";



	-- Weekday out/in- outDate/inDate måste vara en av dessa dagar.
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,0",			 		"1,0,0", 				"3", 					"5"); select " ";
	select validateMinMaxDate("2002-07-07", "2002-07-08", "0,0,0",			 		"1,0,0", 				"7", 					"1"); select " ";

	-- Min: 2d - Måste vara borta två dagar exlusive utresedatum inklusive inresedatum, ie: 2002-07-03 -- 2002-07-05..
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,2",			 		"1,0,0", 				"1234567", 		"1234567"); select " ";
	select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,4",			 		"1,0,0", 				"1234567", 		"1234567"); select " ";
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,4|0,0,2", 		"1,0,0|1,0,0", 	"1234567", 		"1234567"); select " ";

	-- Max: 2d - Får inte vara borta längre än två dagar exlusive utresedatum inklusive inresedatum. ie: 2002-07-03 -- 2002-07-05..
	-- En sektion i max får ej innehålla or |, bara slash /... ie: 0,0,2/0,0,4 ej 0,0,1|0,0,5/0,0,4
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,0",			 		"0,0,2", 				"1234567", 		"1234567"); select " ";
	select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,0",			 		"0,0,4", 				"1234567", 		"1234567"); select " ";
	select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,0/0,0,0", 		"0,0,2/0,0,4", 	"1234567", 		"1234567"); select " ";

	-- th/1y
	select validateMinMaxDate("2002-07-03", "2002-07-05", "4", 							"1,0,0", 	"1234567", 		"1234567"); select " ";

	-- fr-su/1y
	select validateMinMaxDate("2002-07-04", "2002-07-07", "5-7", 						"1,0,0", 				"1234567", 		"1234567"); select " ";

	-- mo,su,ti/1y
	select validateMinMaxDate("2002-07-01", "2002-07-02", "1|7|2",					"1,0,0", 				"1234567", 		"1234567"); select " ";
	select validateMinMaxDate("2002-07-06", "2002-07-07", "1|7|2",					"1,0,0", 				"1234567", 		"1234567"); select " ";

	-- fr-su, ti-we/1y
	select validateMinMaxDate("2002-07-04", "2002-07-07", "5-7|2-3",				"1,0,0", 				"1234567", 		"1234567"); select " ";
	select validateMinMaxDate("2002-07-01", "2002-07-03", "5-7|2-3",				"1,0,0", 				"1234567", 		"1234567"); select " ";

	-- fr-su, ti-we, mo, th, TU/1y
	select validateMinMaxDate("2002-07-01", "2002-07-02", "5-7|2-3|1|4|2",	"1,0,0", 	"1234567", 		"1234567"); select " ";

	-- 3d, mo, ti-we/1Y
	select validateMinMaxDate("2002-06-30", "2002-07-01", "0,0,3|1|2-3",		"1,0,0", 				"1234567", 		"1234567"); select " ";
	select validateMinMaxDate("2002-07-02", "2002-07-05", "0,0,3|1|2-3",		"1,0,0", 				"1234567", 		"1234567"); select " ";

	-- 3d+ti/1y
	select validateMinMaxDate("2002-07-01", "2002-07-04", "0,0,3&2",				"1,0,0", 				"1234567", 		"1234567"); select " ";
	select validateMinMaxDate("2002-06-30", "2002-07-03", "0,0,3&2",				"1,0,0", 				"1234567", 		"1234567"); select " ";

	-- 7d/1m or 3d/4m
	select validateMinMaxDate("2002-07-01", "2002-07-04", "0,0,7/0,0,3",		"1,0,0/1,0,0", 	"1234567", 		"1234567"); select " ";

	-- we-we/1m
	select validateMinMaxDate("2002-07-02", "2002-07-10", "3-3",		"1,0,0", 	"1234567", 		"1234567"); select " ";


	select "FAIL";


	-- Invalid min/max

	-- Weekday
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,0",			 		"1,0,0", 				"2", 					"4"); select " ";
	select validateMinMaxDate("2002-07-07", "2002-07-08", "0,0,0",			 		"1,0,0", 				"7", 					"2"); select " ";

	-- Min
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,3",			 		"1,0,0", 				"1234567", 		"1234567"); select " ";
	select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,5",			 		"1,0,0", 				"1234567", 		"1234567"); select " ";
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,5|0,0,3", 		"1,0,0|1,0,0", 	"1234567", 		"1234567"); select " ";

	-- Max:
	select validateMinMaxDate("2002-07-03", "2002-07-05", "0,0,0",			 		"0,0,1", 				"1234567", 		"1234567"); select " ";
	select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,0",			 		"0,0,3", 				"1234567", 		"1234567"); select " ";
	select validateMinMaxDate("2002-07-14", "2002-07-18", "0,0,0/0,0,0", 		"0,0,2/0,0,3", 	"1234567", 		"1234567"); select " ";

	-- fr-su/1y
	select validateMinMaxDate("2002-07-07", "2002-07-07", "5-7", 						"1,0,0", 				"1234567", 		"1234567"); select " ";

	-- mo,su,ti/1y
	select validateMinMaxDate("2002-07-02", "2002-07-02", "1-1|7|2",				"1,0,0", 				"1234567", 		"1234567"); select " ";
	select validateMinMaxDate("2002-07-07", "2002-07-07", "1-1|7|2",				"1,0,0", 				"1234567", 		"1234567"); select " ";

	-- fr-su, ti-we/1y
	select validateMinMaxDate("2002-07-05", "2002-07-07", "5-7|2-3",				"1,0,0", 				"1234567", 		"1234567"); select " ";
	select validateMinMaxDate("2002-07-03", "2002-07-04", "5-7|2-3",				"1,0,0", 				"1234567", 		"1234567"); select " ";

	-- 3d, mo, ti-we/1Y
	select validateMinMaxDate("2002-07-01", "2002-07-02", "0,0,3|1|2-3",		"1,0,0", 				"1234567", 		"1234567"); select " ";
	select validateMinMaxDate("2002-07-02", "2002-07-04", "0,0,3|1|2-3",		"1,0,0", 				"1234567", 		"1234567"); select " ";

	-- 3d+ti/1y
	select validateMinMaxDate("2002-07-01", "2002-07-03", "0,0,3&2",				"1,0,0", 				"1234567", 		"1234567"); select " ";
	select validateMinMaxDate("2002-06-30", "2002-07-02", "0,0,3&2",				"1,0,0", 				"1234567", 		"1234567"); select " ";

	-- 7d/1m or 3d/4m
	select validateMinMaxDate("2002-07-01", "2002-07-02", "0,0,7/0,0,3",		"1,0,0/1,0,0", 	"1234567", 		"1234567"); select " ";

	-- we-we/1m
	select validateMinMaxDate("2002-07-02", "2002-07-03", "3-3",		"1,0,0", 	"1234567", 		"1234567"); select " ";

