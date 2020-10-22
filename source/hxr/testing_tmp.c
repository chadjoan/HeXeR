#include "hxr/pp_core.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define HXR_LINKAGE_PREFIX hxr_

#define HXR__(prefix, symbol)  prefix ## symbol

/// The `HXR` macro is used to apply the `HXR_LINKAGE_PREFIX` to identifiers.
/// See the `HXR_LINKAGE_PREFIX` documentation for more details.
#define HXR(symbol)    HXR__(HXR_LINKAGE_PREFIX, symbol)

static int HXR(is_identifier_)(const char *str)
{
	// First char can be a-zA-Z or underscore, but no digits.
	char ch = *str;
	str++;
	if ( ('a' <= ch && ch <= 'z')
	||   ('A' <= ch && ch <= 'Z')
	||   ch == '_' )
		(void)0; // good
	else
		return 0; // not an identifier.

	while(1)
	{
		ch = *str;
		str++;
		if ( ch == '\0' )
			return 1;

		if ( ('a' <= ch && ch <= 'z')
		||   ('A' <= ch && ch <= 'Z')
		||   ('0' <= ch && ch <= '9')
		||   ch == '_' )
			(void)0; // good
		else
			return 0; // not an identifier.
	}
}

static size_t HXR(calculate_text_alignment_)(const char *a, const char *b, size_t *a_padding_size, size_t *b_padding_size)
{
	size_t a_len = strlen(a);
	size_t b_len = strlen(b);
	size_t align_len;
	if ( a_len > b_len )
		align_len = a_len;
	else
		align_len = b_len;

	if ( a_padding_size != NULL )
		*a_padding_size = align_len - a_len;
	if ( b_padding_size != NULL )
		*b_padding_size = align_len - b_len;

	return align_len;
}

static int HXR(assert_str_eq_)(
	const char *file, const char *func, size_t line,
	const char *a_expr,  const char *b_expr,
	const char *a_value, const char *b_value)
{
	if ( 0 == strcmp(a_value, b_value) )
		return 1;

	int iline = (int)line;
	if ( HXR(is_identifier_)(a_expr) && HXR(is_identifier_)(b_expr) )
	{
		int align_len =
			(int)HXR(calculate_text_alignment_)(
				a_expr, b_expr, NULL, NULL);

		printf("%s, %d: Strings were not equal when they should be.\n", file, iline);
		printf("%s, %d:   %*s:  \"%s\"\n", file, iline, align_len, a_expr, a_value);
		printf("%s, %d:   %*s:  \"%s\"\n", file, iline, align_len, b_expr, b_value);
	}
	else
	{
		size_t a_padding, b_padding;
		size_t align_len =
			HXR(calculate_text_alignment_)(
				a_value, b_value, &a_padding, &b_padding);

		printf("%s, %d: Strings were not equal when they should be.\n", file, iline);

		// There are two paths for printing the sides of the assertion:
		//
		// The first is for string literals, where we don't print
		// the expression that lead to the result (it would be redundant
		// and unpleasant to look at).
		//
		// The second is for more complicated expressions that, presumably,
		// return C strings. In this case we print both the expression and
		// the value it generated.
		//
		// We compare x_value against x_expr[1..$-1] to discard any quotes
		// that are likely surrounding `a_expr` or `b_expr`. This is because
		// the only way for a string result to match its own expression is for
		// it to be a string literal, and string literals will be wrapped in
		// quotes when stringized by the macro # operator (because that is
		// exactly what they looked like in the source text). If there are
		// other was for string literals to be written, then this code might
		// (currently) fail to detect those possibilities.
		//
		if ( 0 == strncmp(a_value, a_expr+1, align_len - a_padding) )
			printf("%s, %d:   \"%s\"\n", file, iline, a_value);
		else
			printf("%s, %d:   \"%s\"%*s (%s)\n", file, iline, a_value, (int)a_padding, "", a_expr);

		if ( 0 == strncmp(b_value, b_expr+1, align_len - b_padding) )
			printf("%s, %d:   \"%s\"\n", file, iline, b_value);
		else
			printf("%s, %d:   \"%s\"%*s (%s)\n", file, iline, b_value, (int)b_padding, "", b_expr);
	}
	return 0;
}

static int HXR(test_assert_str_eq_)(
	const char *file, const char *func, size_t line,
	const char *a_expr,  const char *b_expr,
	const char *a_value, const char *b_value)
{
	if ( 0 == strcmp(a_value, b_value) )
		return 1;

	printf("\n");
	return HXR(assert_str_eq_)(file, func, line, a_expr, b_expr, a_value, b_value);
}

/*
TODO: once HeXeR's core interop features are available, these tests should
move into hexer.c and use things like debugf instead of relying on assert.h/stdio.h.
*/
int main(int argc, const char *argv[])
{
	const char *expected_str;
	const char *actual_str;
	int exec_count = 0;

#define HXR_ASSERT_STR_EQ(a,b) \
	do { \
		if ( !HXR(test_assert_str_eq_)(__FILE__, __func__, __LINE__, #a, #b, a, b) ) \
			assert(0 == strcmp(a, b)); \
	} while(0)

	printf("Testing basic macro operations.\n");
	printf("  Concatenation...");

#define HXR_TEST_CONCAT(expect, ...) \
	do { \
		actual_str = HXR_STRINGIZE(HXR_CONCAT(__VA_ARGS__)); \
		expected_str = (expect); \
		HXR_ASSERT_STR_EQ(actual_str, expected_str); \
	} while (0)
	HXR_TEST_CONCAT(               "a",                               a);
	HXR_TEST_CONCAT(              "a1",                             a,1);
	HXR_TEST_CONCAT(             "a1b",                           a,1,b);
	HXR_TEST_CONCAT(            "a1b2",                         a,1,b,2);
	HXR_TEST_CONCAT(           "a1b2c",                       a,1,b,2,c);
	HXR_TEST_CONCAT(          "a1b2c3",                     a,1,b,2,c,3);
	HXR_TEST_CONCAT(         "a1b2c3d",                   a,1,b,2,c,3,d);
	HXR_TEST_CONCAT(        "a1b2c3d4",                 a,1,b,2,c,3,d,4);
	HXR_TEST_CONCAT(       "a1b2c3d4e",               a,1,b,2,c,3,d,4,e);
	HXR_TEST_CONCAT(      "a1b2c3d4e5",             a,1,b,2,c,3,d,4,e,5);
	HXR_TEST_CONCAT(     "a1b2c3d4e5f",           a,1,b,2,c,3,d,4,e,5,f);
	HXR_TEST_CONCAT(    "a1b2c3d4e5f6",         a,1,b,2,c,3,d,4,e,5,f,6);
	HXR_TEST_CONCAT(   "a1b2c3d4e5f6g",       a,1,b,2,c,3,d,4,e,5,f,6,g);
	HXR_TEST_CONCAT(  "a1b2c3d4e5f6g7",     a,1,b,2,c,3,d,4,e,5,f,6,g,7);
	HXR_TEST_CONCAT( "a1b2c3d4e5f6g7h",   a,1,b,2,c,3,d,4,e,5,f,6,g,7,h);
	HXR_TEST_CONCAT("a1b2c3d4e5f6g7h8", a,1,b,2,c,3,d,4,e,5,f,6,g,7,h,8);

#define HXR_TEST_MACRO_0  0
	HXR_TEST_CONCAT("X0", X,HXR_TEST_MACRO_0);
#undef HXR_TEST_MACRO_0

	printf(" passed\n");
#undef HXR_TEST_CONCAT

	printf("Testing comma-separated-digit arithmatic.\n");

	printf("  CSD construction and token generation...");
	exec_count = 0;
HXR_CONCAT(label_,HXR_GENERATE_TOKEN_FROM_CSD(1,0)):;
	exec_count++;
	if ( exec_count < 2 )
		goto label_10;
	assert(exec_count == 2);

	exec_count = 0;
HXR_CONCAT(label_,HXR_GENERATE_TOKEN_FROM_CSD(HXR_MAKE_CSD10(0))):;
	exec_count++;
	if ( exec_count < 2 )
		goto label_0000000000;
	assert(exec_count == 2);
	printf(" passed.\n");

	printf("  CSD trimming...");
	
#define HXR_TEST_TRIM_BY_01(expect,_1,_0) \
	do { \
		actual_str = HXR_STRINGIZE(HXR_GENERATE_TOKEN_FROM_CSD(HXR_TRIM_CSD_BY_01(_1,_0))); \
		expected_str = (expect); \
		HXR_ASSERT_STR_EQ(actual_str, expected_str); \
	} while(0)

	HXR_TEST_TRIM_BY_01( "0",0,0);
	HXR_TEST_TRIM_BY_01("10",1,0);
	HXR_TEST_TRIM_BY_01("20",2,0);
	HXR_TEST_TRIM_BY_01("30",3,0);
	HXR_TEST_TRIM_BY_01("40",4,0);
	HXR_TEST_TRIM_BY_01("50",5,0);
	HXR_TEST_TRIM_BY_01("60",6,0);
	HXR_TEST_TRIM_BY_01("70",7,0);
	HXR_TEST_TRIM_BY_01("80",8,0);
	HXR_TEST_TRIM_BY_01("90",9,0);

	HXR_TEST_TRIM_BY_01( "9",0,9);
	HXR_TEST_TRIM_BY_01("19",1,9);
	HXR_TEST_TRIM_BY_01("29",2,9);
	HXR_TEST_TRIM_BY_01("39",3,9);
	HXR_TEST_TRIM_BY_01("49",4,9);
	HXR_TEST_TRIM_BY_01("59",5,9);
	HXR_TEST_TRIM_BY_01("69",6,9);
	HXR_TEST_TRIM_BY_01("79",7,9);
	HXR_TEST_TRIM_BY_01("89",8,9);
	HXR_TEST_TRIM_BY_01("99",9,9);

#undef HXR_TEST_TRIM_BY_01

#if defined(_MSVC_TRADITIONAL) && _MSVC_TRADITIONAL
#else
#define HXR_TEST_TRIM_BY_02(expect, ...) \
	do { \
		actual_str = HXR_STRINGIZE(HXR_GENERATE_TOKEN_FROM_CSD(HXR_TRIM_CSD_BY_02(__VA_ARGS__))); \
		expected_str = (expect); \
		HXR_ASSERT_STR_EQ(actual_str, expected_str); \
	} while(0)

	HXR_TEST_TRIM_BY_02(  "0",0,0,0);
	HXR_TEST_TRIM_BY_02(  "1",0,0,1);
	HXR_TEST_TRIM_BY_02(  "2",0,0,2);
	HXR_TEST_TRIM_BY_02(  "3",0,0,3);
	HXR_TEST_TRIM_BY_02(  "4",0,0,4);
	HXR_TEST_TRIM_BY_02(  "5",0,0,5);
	HXR_TEST_TRIM_BY_02(  "6",0,0,6);
	HXR_TEST_TRIM_BY_02(  "7",0,0,7);
	HXR_TEST_TRIM_BY_02(  "8",0,0,8);
	HXR_TEST_TRIM_BY_02(  "9",0,0,9);

	HXR_TEST_TRIM_BY_02(  "0",0,0,0);
	HXR_TEST_TRIM_BY_02( "10",0,1,0);
	HXR_TEST_TRIM_BY_02( "20",0,2,0);
	HXR_TEST_TRIM_BY_02( "30",0,3,0);
	HXR_TEST_TRIM_BY_02( "40",0,4,0);
	HXR_TEST_TRIM_BY_02( "50",0,5,0);
	HXR_TEST_TRIM_BY_02( "60",0,6,0);
	HXR_TEST_TRIM_BY_02( "70",0,7,0);
	HXR_TEST_TRIM_BY_02( "80",0,8,0);
	HXR_TEST_TRIM_BY_02( "90",0,9,0);

	HXR_TEST_TRIM_BY_02(  "0",0,0,0);
	HXR_TEST_TRIM_BY_02("100",1,0,0);
	HXR_TEST_TRIM_BY_02("200",2,0,0);
	HXR_TEST_TRIM_BY_02("300",3,0,0);
	HXR_TEST_TRIM_BY_02("400",4,0,0);
	HXR_TEST_TRIM_BY_02("500",5,0,0);
	HXR_TEST_TRIM_BY_02("600",6,0,0);
	HXR_TEST_TRIM_BY_02("700",7,0,0);
	HXR_TEST_TRIM_BY_02("800",8,0,0);
	HXR_TEST_TRIM_BY_02("900",9,0,0);

	HXR_TEST_TRIM_BY_02( "0",0,0);
	HXR_TEST_TRIM_BY_02("10",1,0);
	HXR_TEST_TRIM_BY_02("20",2,0);
	HXR_TEST_TRIM_BY_02("30",3,0);
	HXR_TEST_TRIM_BY_02("40",4,0);
	HXR_TEST_TRIM_BY_02("50",5,0);
	HXR_TEST_TRIM_BY_02("60",6,0);
	HXR_TEST_TRIM_BY_02("70",7,0);
	HXR_TEST_TRIM_BY_02("80",8,0);
	HXR_TEST_TRIM_BY_02("90",9,0);

	HXR_TEST_TRIM_BY_02( "9",0,9);
	HXR_TEST_TRIM_BY_02("19",1,9);
	HXR_TEST_TRIM_BY_02("29",2,9);
	HXR_TEST_TRIM_BY_02("39",3,9);
	HXR_TEST_TRIM_BY_02("49",4,9);
	HXR_TEST_TRIM_BY_02("59",5,9);
	HXR_TEST_TRIM_BY_02("69",6,9);
	HXR_TEST_TRIM_BY_02("79",7,9);
	HXR_TEST_TRIM_BY_02("89",8,9);
	HXR_TEST_TRIM_BY_02("99",9,9);

#undef HXR_TEST_TRIM_BY_02

	// And one last test to make sure it generalizes to shortening wide numbers:
	actual_str = HXR_STRINGIZE(HXR_GENERATE_TOKEN_FROM_CSD(HXR_TRIM_CSD(HXR_MAKE_CSD10(1))));
	expected_str = "1";
	HXR_ASSERT_STR_EQ(actual_str, expected_str);
	printf(" passed.\n");


	printf("  CSD incrementation...");


	actual_str = HXR_STRINGIZE((HXR_CONCAT_02(HXR_ADD_DIGITS_1, 0)));
	expected_str = "(0,1)";
	HXR_ASSERT_STR_EQ(actual_str, expected_str);
	actual_str = HXR_STRINGIZE((HXR_CONCAT_02(HXR_ADD_DIGITS_1, 1)));
	expected_str = "(0,2)";
	HXR_ASSERT_STR_EQ(actual_str, expected_str);
	actual_str = HXR_STRINGIZE((HXR_CONCAT_02(HXR_ADD_DIGITS_1, 9)));
	expected_str = "(1,0)";
	HXR_ASSERT_STR_EQ(actual_str, expected_str);
	
	actual_str = HXR_STRINGIZE((HXR_INCR_AND_CARRY_CSD01(0)));
	expected_str = "(0,1)";
	HXR_ASSERT_STR_EQ(actual_str, expected_str);
	actual_str = HXR_STRINGIZE((HXR_INCR_AND_CARRY_CSD01(9)));
	expected_str = "(1,0)";
	HXR_ASSERT_STR_EQ(actual_str, expected_str);

	actual_str = HXR_STRINGIZE((HXR_INCR_AND_WRAP_CSD01(0)));
	expected_str = "(1)";
	HXR_ASSERT_STR_EQ(actual_str, expected_str);
	actual_str = HXR_STRINGIZE((HXR_INCR_AND_WRAP_CSD01(9)));
	expected_str = "(0)";
	HXR_ASSERT_STR_EQ(actual_str, expected_str);

	actual_str = HXR_STRINGIZE((HXR_INCR_CSD01(0)));
	expected_str = "(1)";
	HXR_ASSERT_STR_EQ(actual_str, expected_str);
	actual_str = HXR_STRINGIZE((HXR_INCR_CSD01(9)));
	expected_str = "(1,0)";
	HXR_ASSERT_STR_EQ(actual_str, expected_str);

	actual_str = HXR_STRINGIZE((HXR_CONCAT_03(HXR_ADD_DIGITS_, 0, 1),1));
	expected_str = "(0,1,1)";
	HXR_ASSERT_STR_EQ(actual_str, expected_str);
	
	actual_str = HXR_STRINGIZE((HXR_INCR_AND_CARRY_CSD02(0,9)));
	expected_str = "(0,1,0)";
	HXR_ASSERT_STR_EQ(actual_str, expected_str);
	actual_str = HXR_STRINGIZE((HXR_INCR_AND_CARRY_CSD02(1,0)));
	expected_str = "(0,1,1)";
	HXR_ASSERT_STR_EQ(actual_str, expected_str);
	actual_str = HXR_STRINGIZE((HXR_INCR_AND_CARRY_CSD02(1,9)));
	expected_str = "(0,2,0)";
	HXR_ASSERT_STR_EQ(actual_str, expected_str);

#define HXR_TEST_INCREMENT(expect, ...) \
	do { \
		actual_str = HXR_STRINGIZE(HXR_GENERATE_TOKEN_FROM_CSD(HXR_INCR_CSD(__VA_ARGS__))); \
		expected_str = (expect); \
		HXR_ASSERT_STR_EQ(actual_str, expected_str); \
	} while (0)

	HXR_TEST_INCREMENT( "1",   0);
	HXR_TEST_INCREMENT( "2",   1);
	HXR_TEST_INCREMENT( "3",   2);
	HXR_TEST_INCREMENT( "4",   3);
	HXR_TEST_INCREMENT( "5",   4);
	HXR_TEST_INCREMENT( "6",   5);
	HXR_TEST_INCREMENT( "7",   6);
	HXR_TEST_INCREMENT( "8",   7);
	HXR_TEST_INCREMENT( "9",   8);
	HXR_TEST_INCREMENT("10",   9);
	HXR_TEST_INCREMENT("11", 1,0);

	HXR_TEST_INCREMENT("01", 0,0);
	HXR_TEST_INCREMENT("02", 0,1);
	HXR_TEST_INCREMENT("03", 0,2);
	HXR_TEST_INCREMENT("04", 0,3);
	HXR_TEST_INCREMENT("05", 0,4);
	HXR_TEST_INCREMENT("06", 0,5);
	HXR_TEST_INCREMENT("07", 0,6);
	HXR_TEST_INCREMENT("08", 0,7);
	HXR_TEST_INCREMENT("09", 0,8);
	HXR_TEST_INCREMENT("10", 0,9);
	HXR_TEST_INCREMENT("11", 1,0);
	HXR_TEST_INCREMENT("12", 1,1);
	HXR_TEST_INCREMENT("13", 1,2);
	HXR_TEST_INCREMENT("14", 1,3);
	HXR_TEST_INCREMENT("15", 1,4);
	HXR_TEST_INCREMENT("16", 1,5);
	HXR_TEST_INCREMENT("17", 1,6);
	HXR_TEST_INCREMENT("18", 1,7);
	HXR_TEST_INCREMENT("19", 1,8);
	HXR_TEST_INCREMENT("20", 1,9);
	HXR_TEST_INCREMENT("30", 2,9);
	HXR_TEST_INCREMENT("40", 3,9);
	HXR_TEST_INCREMENT("50", 4,9);
	HXR_TEST_INCREMENT("60", 5,9);
	HXR_TEST_INCREMENT("70", 6,9);
	HXR_TEST_INCREMENT("80", 7,9);
	HXR_TEST_INCREMENT("90", 8,9);
	HXR_TEST_INCREMENT("99", 9,8);

	HXR_TEST_INCREMENT(       "100",                 9,9);
	HXR_TEST_INCREMENT(       "200",               1,9,9);
	HXR_TEST_INCREMENT(      "1000",               9,9,9);
	HXR_TEST_INCREMENT(      "2000",             1,9,9,9);
	HXR_TEST_INCREMENT(     "10000",             9,9,9,9);
	HXR_TEST_INCREMENT(     "20000",           1,9,9,9,9);
	HXR_TEST_INCREMENT(    "100000",           9,9,9,9,9);
	HXR_TEST_INCREMENT(    "200000",         1,9,9,9,9,9);
	HXR_TEST_INCREMENT(   "1000000",         9,9,9,9,9,9);
	HXR_TEST_INCREMENT(   "2000000",       1,9,9,9,9,9,9);
	HXR_TEST_INCREMENT(  "10000000",       9,9,9,9,9,9,9);
	HXR_TEST_INCREMENT(  "20000000",     1,9,9,9,9,9,9,9);
	HXR_TEST_INCREMENT( "100000000",     9,9,9,9,9,9,9,9);
	HXR_TEST_INCREMENT( "200000000",   1,9,9,9,9,9,9,9,9);
	HXR_TEST_INCREMENT("1000000000",   9,9,9,9,9,9,9,9,9);
	HXR_TEST_INCREMENT("2000000000", 1,9,9,9,9,9,9,9,9,9);
	HXR_TEST_INCREMENT("9999999999", 9,9,9,9,9,9,9,9,9,8);
	HXR_TEST_INCREMENT("0000000000", 9,9,9,9,9,9,9,9,9,9);

	// Test the ability to increment more than once at a time.
	actual_str = HXR_STRINGIZE((HXR_INCR_CSD(HXR_INCR_CSD(0))));
	expected_str = "(2)";
	HXR_ASSERT_STR_EQ(actual_str, expected_str);
#undef HXR_TEST_INCREMENT
#endif
	printf(" passed.\n");
	return 0;
}
