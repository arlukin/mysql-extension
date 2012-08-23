/*

Check if the length of c variable types are what we expect them to be.

__author__     = "daniel.lindh@cybercow.se"
__copyright__  = "Copyright 2012, Fareoffice CRS AB"
__maintainer__ = "Daniel Lindh"
__version__    = "1.0.0"
__status__     = "Production"

*/


#include <UnitTest++.h>


TEST(string_data_type_size)
{
    CHECK_EQUAL((unsigned int)4, sizeof(int));
    CHECK_EQUAL((unsigned int)8, sizeof(long));
    CHECK_EQUAL((unsigned int)1, sizeof(char));
    CHECK_EQUAL((unsigned int)2, sizeof(short));

    CHECK_EQUAL((unsigned int)4, sizeof(unsigned int));
    CHECK_EQUAL((unsigned int)8, sizeof(unsigned long));
    CHECK_EQUAL((unsigned int)1, sizeof(unsigned char));
    CHECK_EQUAL((unsigned int)2, sizeof(unsigned short));
}
