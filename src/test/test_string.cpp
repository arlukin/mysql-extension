/*

Test the string/byte modifing class.

__author__     = "daniel.lindh@cybercow.se"
__copyright__  = "Copyright 2012, Fareoffice CRS AB"
__maintainer__ = "Daniel Lindh"
__credits__    = "???"
__license__    = "???"
__version__    = "1.0.0"
__status__     = "Production"

*/


#include <UnitTest++.h>
#include <fo_string.h>


TEST(string_construction_1)
{
    FOString str;

    CHECK_EQUAL(str.length(), 0);
    CHECK(str.get() == (void*)NULL);
}


TEST(string_construction_2)
{
    FOString str(10);

    CHECK_EQUAL(str.length(), 10);
    CHECK(str.get() != (void*)NULL);
}


TEST(string_allocate_buffer)
{
    FOString str(10);

    str.allocate_buffer(11);
    CHECK_EQUAL(str.length(), 11);

    str.allocate_buffer(10);
    CHECK_EQUAL(str.length(), 11);
    CHECK(str.get() != (void*)NULL);
}


TEST(string_set)
{
    FOString str(10);

    char tst[] = "1234567890";
    str.set(tst, 10);
    CHECK_EQUAL(str.length(), 10);
    CHECK(strncmp(str.get(), "1234567890", 10) == 0);
}


TEST(string_overwrite_char)
{
    FOString str;
    int pos = 1;
    char value = 67;
    str.overwrite(pos, value);
    CHECK_EQUAL(pos, 1);
    CHECK_EQUAL(str.length(), 2);
    CHECK(memcmp(str.get(), "\0C", 2) == 0);

    str.set("1234567890", 10);

    pos = 1;
    str.overwrite(pos, value);
    CHECK_EQUAL(pos, 1);
    CHECK_EQUAL(str.length(), 10);
    CHECK(strncmp(str.get(), "1C34567890", 10) == 0);

    pos = 2;
    value = 48;
    str.overwrite(pos, value);
    CHECK_EQUAL(pos, 2);
    CHECK_EQUAL(str.length(), 10);
    CHECK(strncmp(str.get(), "1C04567890", 10) == 0);
}


TEST(string_overwrite_char_pointer)
{
    FOString str;
    int pos = 1;
    char value = 67;
    str.overwrite(&pos, value);
    CHECK_EQUAL(pos, 1+1);
    CHECK_EQUAL(str.length(), 2);
    CHECK(memcmp(str.get(), "\0C", 2) == 0);

    str.set("1234567890", 10);

    pos = 1;
    str.overwrite(&pos, value);
    CHECK_EQUAL(pos, 1+1);
    CHECK_EQUAL(str.length(), 10);
    CHECK(strncmp(str.get(), "1C34567890", 10) == 0);

    value = 48;
    str.overwrite(&pos, value);
    CHECK_EQUAL(pos, 1+2);
    CHECK_EQUAL(str.length(), 10);
    CHECK(strncmp(str.get(), "1C04567890", 10) == 0);
}


TEST(string_overwrite_unsigned_short)
{
    FOString str;
    int pos = 1;
    unsigned short value = (48 << 8)|67;
    str.overwrite(pos, value);
    CHECK_EQUAL(pos, 1);
    CHECK_EQUAL(str.length(), 3);
    CHECK(memcmp(str.get(), "\0C0", 2) == 0);

    str.set("1234567890", 10);

    pos = 1;
    str.overwrite(pos, value);
    CHECK_EQUAL(pos, 1);
    CHECK_EQUAL(str.length(), 10);
    CHECK(strncmp(str.get(), "1C04567890", 10) == 0);

    pos = 3;
    value = (69 << 8)|68;
    str.overwrite(pos, value);
    CHECK_EQUAL(pos, 3);
    CHECK_EQUAL(str.length(), 10);
    CHECK(strncmp(str.get(), "1C0DE67890", 10) == 0);
}


TEST(string_overwrite_unsigned_short_pointer)
{
    FOString str;

    int pos = 1;
    unsigned short value = (48 << 8)|67;
    str.overwrite(&pos, value);
    CHECK_EQUAL(pos, 1+2);
    CHECK_EQUAL(str.length(), 3);
    CHECK(memcmp(str.get(), "\0C0", 2) == 0);

    str.set("1234567890", 10);

    pos = 1;
    str.overwrite(&pos, value);
    CHECK_EQUAL(pos, 1+2);
    CHECK_EQUAL(str.length(), 10);
    CHECK(strncmp(str.get(), "1C04567890", 10) == 0);

    value = (69 << 8)|68;
    str.overwrite(&pos, value);
    CHECK_EQUAL(pos, 1+4);
    CHECK_EQUAL(str.length(), 10);
    CHECK(strncmp(str.get(), "1C0DE67890", 10) == 0);
}


TEST(string_overwrite_unsigned_int)
{
    FOString str;
    int pos = 1;
    unsigned int value = (69 << 24)|68 << 16|(48 << 8)|67;
    str.overwrite(pos, value);
    CHECK_EQUAL(pos, 1);
    CHECK_EQUAL(str.length(), 5);
    CHECK(memcmp(str.get(), "\0C0DE", 5) == 0);

    str.set("1234567890", 10);

    pos = 1;
    str.overwrite(pos, value);
    CHECK_EQUAL(pos, 1);
    CHECK_EQUAL(str.length(), 10);
    CHECK(strncmp(str.get(), "1C0DE67890", 10) == 0);

    pos = 5;
    value = (69 << 24)|68 << 16|(48 << 8)|67;
    str.overwrite(pos, value);
    CHECK_EQUAL(pos, 5);
    CHECK_EQUAL(str.length(), 10);
    CHECK(strncmp(str.get(), "1C0DEC0DE0", 10) == 0);
}


TEST(string_overwrite_unsigned_int_pointer)
{
    FOString str;
    int pos = 1;
    unsigned int value = (69 << 24)|68 << 16|(48 << 8)|67;
    str.overwrite(&pos, value);
    CHECK_EQUAL(pos, 5);
    CHECK_EQUAL(str.length(), 5);
    CHECK(memcmp(str.get(), "\0C0DE", 5) == 0);

    str.set("1234567890", 10);

    pos = 1;
    str.overwrite(&pos, value);
    CHECK_EQUAL(pos, 5);
    CHECK_EQUAL(str.length(), 10);
    CHECK(strncmp(str.get(), "1C0DE67890", 10) == 0);

    value = (69 << 24)|68 << 16|(48 << 8)|67;
    str.overwrite(&pos, value);
    CHECK_EQUAL(pos, 9);
    CHECK_EQUAL(str.length(), 10);
    CHECK(strncmp(str.get(), "1C0DEC0DE0", 10) == 0);
}


TEST(string_overwrite_unsigned_long)
{
    FOString str;
    int pos = 1;
    unsigned long value = (69 << 24)|68 << 16|(48 << 8)|67;
    value = value << 32 | value;
    str.overwrite(pos, value);
    CHECK_EQUAL(pos, 1);
    CHECK_EQUAL(str.length(), 9);
    CHECK(memcmp(str.get(), "\0C0DEC0DE", 9) == 0);

    str.set("12345678901234567890", 20);

    pos = 1;
    str.overwrite(pos, value);
    CHECK_EQUAL(pos, 1);
    CHECK_EQUAL(str.length(), 20);
    CHECK(strncmp(str.get(), "1C0DEC0DE01234567890", 20) == 0);

    pos = 9;
    str.overwrite(pos, value);
    CHECK_EQUAL(pos, 9);
    CHECK_EQUAL(str.length(), 20);
    CHECK(strncmp(str.get(), "1C0DEC0DEC0DEC0DE890", 20) == 0);
}


TEST(string_overwrite_unsigned_long_pointer)
{
    FOString str;
    int pos = 1;
    unsigned long value = (69 << 24)|68 << 16|(48 << 8)|67;
    value = value << 32 | value;
    str.overwrite(&pos, value);
    CHECK_EQUAL(pos, 9);
    CHECK_EQUAL(str.length(), 9);
    CHECK(memcmp(str.get(), "\0C0DEC0DE", 9) == 0);

    str.set("12345678901234567890", 20);

    pos = 1;
    str.overwrite(&pos, value);
    CHECK_EQUAL(pos, 9);
    CHECK_EQUAL(str.length(), 20);
    CHECK(strncmp(str.get(), "1C0DEC0DE01234567890", 20) == 0);

    str.overwrite(&pos, value);
    CHECK_EQUAL(pos, 17);
    CHECK_EQUAL(str.length(), 20);
    CHECK(strncmp(str.get(), "1C0DEC0DEC0DEC0DE890", 20) == 0);
}


TEST(string_overwrite_const_char_pointer)
{
    FOString str;
    int pos = 1;
    char const * const value = "C0DE";
    str.overwrite(pos, value, 4);
    CHECK_EQUAL(pos, 1);
    CHECK_EQUAL(str.length(), 5);
    CHECK(memcmp(str.get(), "\0C0DE", 5) == 0);

    str.set("12345678901234567890", 20);

    pos = 1;
    str.overwrite(pos, value, 4);
    CHECK_EQUAL(pos, 1);
    CHECK_EQUAL(str.length(), 20);
    CHECK(strncmp(str.get(), "1C0DE678901234567890", 20) == 0);

    pos = 5;
    str.overwrite(pos, value, 4);
    CHECK_EQUAL(pos, 5);
    CHECK_EQUAL(str.length(), 20);
    CHECK(strncmp(str.get(), "1C0DEC0DE01234567890", 20) == 0);
}


TEST(string_overwrite_const_char_pointer_pointer)
{
    FOString str;
    int pos = 1;
    char const * const value = "C0DE";
    str.overwrite(&pos, value, 4);
    CHECK_EQUAL(pos, 5);
    CHECK_EQUAL(str.length(), 5);
    CHECK(memcmp(str.get(), "\0C0DE", 5) == 0);

    str.set("12345678901234567890", 20);

    pos = 1;
    str.overwrite(&pos, value, 4);
    CHECK_EQUAL(pos, 5);
    CHECK_EQUAL(str.length(), 20);
    CHECK(strncmp(str.get(), "1C0DE678901234567890", 20) == 0);

    str.overwrite(&pos, value, 4);
    CHECK_EQUAL(pos, 9);
    CHECK_EQUAL(str.length(), 20);
    CHECK(strncmp(str.get(), "1C0DEC0DE01234567890", 20) == 0);
}
