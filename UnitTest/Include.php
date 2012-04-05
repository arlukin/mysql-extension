<?php

	//define('DB_HOST', '212.37.7.134'); // SUSE mysql server
	//define('DB_HOST', '212.37.7.135'); // sql1.fareoffice.com
	define('DB_HOST', 'localhost');

	define('DB_USER', 'faredbuser');
	define('DB_PASSWORD', 'COWUPSIDEDOWN');
	define('DB_DATABASE', 'foMaster');

	$tests['ok'] = 0;
	$tests['tests'] = 0;

	function executeOnDbReturnOneColumn($query)
	{
		global
			$link;

		if (!(isset($link)))
		{
			if ($link = mysql_connect(DB_HOST, DB_USER, DB_PASSWORD, false))
			{
				mysql_select_db(DB_DATABASE);
			}
			else
			{
				echo("Error connecting to $dbName!");
				exit;
			}
		}

		if (isset($link))
		{
			if (!($result = mysql_query($query, $link)))
			{
				echo('error<br>');
				exit;
			}
			else
			{
				if (is_resource($result))
				{
					$row = mysql_fetch_row($result);
					$columnValue = $row[0];

					mysql_free_result($result);
				}
				else
				{
					$columnValue = $result;
				}
			}
		}

		return $columnValue;
	}

	function verifySqlResult($query, $expectedResult)
	{
		global
			$tests;

		$tests['total']++;

		$result = executeOnDbReturnOneColumn($query);
		if ($result != $expectedResult)
		{
			echo('Error in: ' . $query);
			echo('<br>Returned: ' . $result);
			echo('<br>Instead of: ' . $expectedResult);
			echo('<br><br>');
		}
		else
		{
			$tests['ok']++;
		}
	}

	//

	function writeStat()
	{
		global
			$tests;
		echo('Ok tests ' . $tests['ok'] . ' of ' . $tests['total'].'<br><br>');
	}

	//

	function getMicrotime()
	{
		list($usec, $sec) = explode(" ",microtime());
		return ((float)$usec + (float)$sec);
	}
?>