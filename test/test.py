#!/bin/env python
'''
Testing the installation of libfolanguage.

Executes several SQL queries with getLanguage and setLanguage and checks the
result.

'''

__author__     = "daniel.lindh@cybercow.se"
__copyright__  = "Copyright 2012, Fareoffice CRS AB"
__maintainer__ = "Daniel Lindh"
__status__     = "Production"


from time import time
import subprocess


class Timer():
    '''
    Calculate the time it takes to execute the code between start and stop.

    '''
    start_time = None
    elapsed = None

    @staticmethod
    def start():
        Timer.start_time = time()

    @staticmethod
    def stop():
        Mysql.elapsed = time() - Timer.start_time
        print("Test time: %s seconds" % Mysql.elapsed)


class Mysql():
    '''
    Execute an sql query through the mysql command line client.

    '''
    database = ''

    @staticmethod
    def set_database(db):
        '''
        Set the name of the database that x_sql executes it's queries in.

        '''
        Mysql.database = db

    @staticmethod
    def x_sql(query):
        '''
        Execute an sql query through the mysql command line client.

        '''
        return Mysql._x("""mysql %s -uroot -BN -e '%s'""" % (Mysql.database, query))

    @staticmethod
    def _x(cmd):
        '''
        Execute the shell command CMD.

        '''
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        (stdout, stderr) = p.communicate()

        stdout = stdout.strip()

        if not stdout:
            stdout = p.returncode
        return stdout


class Statistics():
    '''
    Collect statistics about total unittests and tests that succeded.

    '''
    total_tests = 0
    ok_tests = 0

    @staticmethod
    def write_result():
        global total_tests, ok_tests
        print("Success: %s/%s tests passed."% (
            Statistics.ok_tests, Statistics.total_tests)
        )


def vrf(query, expected):
    '''
    Execute the QUERY and compares the result with EXPECTED.

    If the comparsion fails, an error message will be printed to the screen and
    the statistics about failed tests will be increased.

    '''
    Statistics.total_tests += 1

    result = Mysql.x_sql(query)
    if result != expected:
        print("Query: %s" % query)
        print('\tReturned: %s' % result)
        print('\tExpected: %s' % expected)
        print('')
    else:
        Statistics.ok_tests += 1


def unittest():
    '''
    Test getLanguage and setLanguage in all possible way.

    '''
    print("Running unit tests...")
    Timer.start()

    #
    # Test without database table
    #
    vrf(
        'SELECT getLanguage(setLanguage(NULL, "[LANG=UK_]Hello world.[/LANG]", NULL, "SE_"), NULL, NULL, 3)',
        '[LANG=UK_]Hello world.[/LANG]'
    )

    #
    # Create database
    #
    Mysql.set_database("")
    vrf('DROP DATABASE IF EXISTS foTest', 0)
    vrf('CREATE DATABASE foTest', 0)

    Mysql.set_database("foTest")
    vrf('DROP TABLE IF EXISTS foLanguageTest', 0)
    vrf('CREATE TABLE foLanguageTest (id int NOT NULL AUTO_INCREMENT, `description` text NULL, PRIMARY KEY (id))', 0)
    vrf('TRUNCATE foLanguageTest', 0)

    #
    # Fill database with test data.
    #
    vrf('INSERT into foLanguageTest (id, description) values(1, setLanguage(NULL, "Sverige", "SE_", ""))', 0)
    vrf('UPDATE foLanguageTest SET description = setLanguage(description, "England", "UK_", "FI_") WHERE id = 1', 0)
    vrf('UPDATE foLanguageTest SET description = setLanguage(description, "Finland", "FI_", "UK_") WHERE id = 1', 0)
    vrf('UPDATE foLanguageTest SET description = setLanguage(description, "test",    "FR_", "") WHERE id = 1', 0)
    vrf('UPDATE foLanguageTest SET description = setLanguage(description, "France",  "FR_", "FI_") WHERE id = 1', 0)
    vrf('UPDATE foLanguageTest SET description = setLanguage(description, "American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American", "US_", "") WHERE id = 1', 0)
    vrf('UPDATE foLanguageTest SET description = setLanguage(description, "", "SE_", "") WHERE id = 1', 0)

    #
    # Check the test data
    #
    vrf('SELECT getLanguage(description, "FI_", "UK_", 1) FROM foLanguageTest', 'Finland')
    vrf('SELECT getLanguage(description, "FR_", "UK_", 1) FROM foLanguageTest', 'France')
    vrf('SELECT getLanguage(description, "US_", "UK_", 1) FROM foLanguageTest', 'American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American')

    # Return default from second arg.
    vrf('SELECT getLanguage(description, "SE_", "UK_", 0) FROM foLanguageTest', 'England')
    vrf('SELECT getLanguage(description, "IS_", "UK_", 1) FROM foLanguageTest', 'England')

    # Return default from first lang stored in db column.
    vrf('SELECT getLanguage(description, "IS_", "MI_", 1) FROM foLanguageTest', 'Finland')
    vrf('SELECT getLanguage(description, "IS_", NULL, 1) FROM foLanguageTest', 'Finland')

    # Return full tag
    vrf('SELECT getLanguage(description, "", "",        0) FROM foLanguageTest', '[LANG=FI_]Finland[/LANG][LANG=UK_]England[/LANG][LANG=FR_]France[/LANG][LANG=US_]American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American[/LANG]')
    vrf('SELECT getLanguage(description, "", "",        1) FROM foLanguageTest', '[LANG=FI_]Finland[/LANG][LANG=UK_]England[/LANG][LANG=FR_]France[/LANG][LANG=US_]American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American[/LANG]')
    vrf('SELECT getLanguage(description, "", "",        2) FROM foLanguageTest', '[LANG=FI_]Finland[/LANG][LANG=UK_]England[/LANG][LANG=FR_]France[/LANG][LANG=US_]American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American[/LANG]')
    vrf('SELECT getLanguage(description, "", "",        3) FROM foLanguageTest', '[LANG=FI_]Finland[/LANG][LANG=UK_]England[/LANG][LANG=FR_]France[/LANG][LANG=US_]American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American[/LANG]')

    vrf('SELECT getLanguage(description, "FR_", NULL,   0) FROM foLanguageTest', 'France')
    vrf('SELECT getLanguage(description, "FR_", NULL,   1) FROM foLanguageTest', 'France')
    vrf('SELECT getLanguage(description, "FR_", NULL,   2) FROM foLanguageTest', 'France')
    vrf('SELECT getLanguage(description, "FR_", NULL,   3) FROM foLanguageTest', 'France')

    vrf('SELECT getLanguage(description, "FR_", "UK_",  0) FROM foLanguageTest', 'France')
    vrf('SELECT getLanguage(description, "FR_", "UK_",  1) FROM foLanguageTest', 'France')
    vrf('SELECT getLanguage(description, "FR_", "UK_",  2) FROM foLanguageTest', 'France')
    vrf('SELECT getLanguage(description, "FR_", "UK_",  3) FROM foLanguageTest', 'France')

    vrf('SELECT getLanguage(description, NULL, "UK_",   0) FROM foLanguageTest', '[LANG=FI_]Finland[/LANG][LANG=UK_]England[/LANG][LANG=FR_]France[/LANG][LANG=US_]American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American[/LANG]')
    vrf('SELECT getLanguage(description, NULL, "UK_",   1) FROM foLanguageTest', '[LANG=FI_]Finland[/LANG][LANG=UK_]England[/LANG][LANG=FR_]France[/LANG][LANG=US_]American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American[/LANG]')
    vrf('SELECT getLanguage(description, NULL, "UK_",   2) FROM foLanguageTest', '[LANG=FI_]Finland[/LANG][LANG=UK_]England[/LANG][LANG=FR_]France[/LANG][LANG=US_]American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American[/LANG]')
    vrf('SELECT getLanguage(description, NULL, "UK_",   3) FROM foLanguageTest', '[LANG=FI_]Finland[/LANG][LANG=UK_]England[/LANG][LANG=FR_]France[/LANG][LANG=US_]American. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. This is a long text. American[/LANG]')

    vrf('SELECT getLanguage(description, "NON", NULL,   0) FROM foLanguageTest', 'NULL')
    vrf('SELECT getLanguage(description, "NON", NULL,   1) FROM foLanguageTest', 'Finland')
    vrf('SELECT getLanguage(description, "NON", NULL,   2) FROM foLanguageTest', 'NULL')
    vrf('SELECT getLanguage(description, "NON", NULL,   3) FROM foLanguageTest', '[LANG=NON][/LANG][LANG=FI_]Finland[/LANG]')

    vrf('SELECT getLanguage(description, "NON", "UK_",  0) FROM foLanguageTest', 'England')
    vrf('SELECT getLanguage(description, "NON", "UK_",  1) FROM foLanguageTest', 'England')
    vrf('SELECT getLanguage(description, "NON", "UK_",  2) FROM foLanguageTest', '[LANG=NON][/LANG][LANG=UK_]England[/LANG]')
    vrf('SELECT getLanguage(description, "NON", "UK_",  3) FROM foLanguageTest', '[LANG=NON][/LANG][LANG=UK_]England[/LANG]')

    vrf('SELECT getLanguage(description, "NON", "NON",  0) FROM foLanguageTest', 'NULL')
    vrf('SELECT getLanguage(description, "NON", "NON",  1) FROM foLanguageTest', 'Finland')
    vrf('SELECT getLanguage(description, "NON", "NON",  2) FROM foLanguageTest', 'NULL')
    vrf('SELECT getLanguage(description, "NON", "NON",  3) FROM foLanguageTest', '[LANG=NON][/LANG][LANG=FI_]Finland[/LANG]')

    # Tests new line "\n"
    vrf('INSERT into foLanguageTest (description) values(setLanguage(NULL, "England\neng", "UK", ""))', 0)
    vrf('SELECT getLanguage(description, "UK", "UK", 0) FROM foLanguageTest WHERE id = 2', "England\\neng")

    # Translate a regulare column to a foLanguageColumn.
    vrf('INSERT into foLanguageTest (id, description) values(3, "1")', 0)
    vrf('INSERT into foLanguageTest (id, description) values(4, "12")', 0)
    vrf('INSERT into foLanguageTest (id, description) values(5, "123")', 0)
    vrf('INSERT into foLanguageTest (id, description) values(6, "1234")', 0)
    vrf('INSERT into foLanguageTest (id, description) values(7, "12345")', 0)

    vrf('UPDATE foLanguageTest SET description = setLanguage(NULL, description, "UK_", "UK_") WHERE id in (3, 4, 5, 6, 7)', 0)

    vrf('SELECT getLanguage(description, NULL, "UK_",   3) FROM foLanguageTest WHERE id in (3)', '[LANG=UK_]1[/LANG]')
    vrf('SELECT getLanguage(description, NULL, "UK_",   3) FROM foLanguageTest WHERE id in (4)', '[LANG=UK_]12[/LANG]')
    vrf('SELECT getLanguage(description, NULL, "UK_",   3) FROM foLanguageTest WHERE id in (5)', '[LANG=UK_]123[/LANG]')
    vrf('SELECT getLanguage(description, NULL, "UK_",   3) FROM foLanguageTest WHERE id in (6)', '[LANG=UK_]1234[/LANG]')
    vrf('SELECT getLanguage(description, NULL, "UK_",   3) FROM foLanguageTest WHERE id in (7)', '[LANG=UK_]12345[/LANG]')

    vrf('DROP TABLE IF EXISTS foLanguageTest', 0)

    Timer.stop()
    Statistics.write_result()


def benchmark():
    '''
    Test how fast the function is on this server.

    '''
    print
    print("Test speed of function. Should be less than 10 seconds.")
    Timer.start()
    vrf('SELECT BENCHMARK(10000000, getLanguage(setLanguage("", "England", "UK", "FI"), "UK", NULL, 0))', '0');
    Timer.stop()


if __name__ == "__main__":
    unittest()
    benchmark()
