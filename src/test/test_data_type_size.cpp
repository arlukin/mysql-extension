#include <UnitTest++.h>

// Check if the length of c variable types are what we expect them to be.
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
