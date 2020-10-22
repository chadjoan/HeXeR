#include <stdio.h>
 
typedef struct TEST_LOOP_FIXTURE {
	int completed;     // Did the enclosed code get executed?
	int normal_exit;   // Was a normal exit detected?
	int continue_used; // Was a continue-based exit/jump detected?
	int break_used;    // Was a break-based exit detected?
	int loop_count;    // Loop iteration count. It better be 1.

} test_loop_fixture;

void reset_fixture(test_loop_fixture *f)
{
	f->completed = 0;
	f->normal_exit = 0;
	f->continue_used = 0;
	f->break_used = 0;
	f->loop_count = 0;
}

void print_test_results(
	const char         *loop_name,
	const char         *exit_type,
	test_loop_fixture  *results,
	test_loop_fixture  *expected
)
{
#	define TEST_ROW(test_field) \
		results->test_field,  \
		expected->test_field, \
		(results->test_field == expected->test_field) ? "yes" : "!!NO!!"

	printf(
		"%s with %s exit:\n"
		"  completed     == %d;  expected %d;  passed? %s\n"
		"  normal_exit   == %d;  expected %d;  passed? %s\n"
		"  continue_used == %d;  expected %d;  passed? %s\n"
		"  break_used    == %d;  expected %d;  passed? %s\n"
		"  loop_count    == %d;  expected %d;  passed? %s\n",
		loop_name, exit_type,
		TEST_ROW(completed),
		TEST_ROW(normal_exit),
		TEST_ROW(continue_used),
		TEST_ROW(break_used),
		TEST_ROW(loop_count)
	);
}

int main(void) 
{
	test_loop_fixture  results;
	test_loop_fixture  expected_for_normal_exit;
	test_loop_fixture  expected_for_continue_exit;
	test_loop_fixture  expected_for_break_exit;

	expected_for_normal_exit.completed     = 1;
	expected_for_normal_exit.normal_exit   = 1;
	expected_for_normal_exit.continue_used = 0;
	expected_for_normal_exit.break_used    = 0;
	expected_for_normal_exit.loop_count    = 1;

	expected_for_continue_exit.completed     = 1;
	expected_for_continue_exit.normal_exit   = 0;
	expected_for_continue_exit.continue_used = 1;
	expected_for_continue_exit.break_used    = 0;
	expected_for_continue_exit.loop_count    = 1;

	expected_for_break_exit.completed     = 1;
	expected_for_break_exit.normal_exit   = 0;
	expected_for_break_exit.continue_used = 0;
	expected_for_break_exit.break_used    = 1;
	expected_for_break_exit.loop_count    = 1;


	const char *loop_name = "";

#define DO_WHILE_BLOCK(exit_strategy) \
	do \
	{ \
		loop_name = "do-while"; \
		results.break_used = 1; \
		results.completed = 1; \
		results.loop_count++; \
		exit_strategy; \
		\
		results.break_used = 0; \
		results.normal_exit = 1; \
		break; \
	} \
	while((results.continue_used = 1), (results.break_used = 0), 0);

#define WHILE_BLOCK(exit_strategy) \
	while((results.continue_used = 1), (results.break_used = 0), !results.completed) \
	{ \
		loop_name = "while"; \
		results.continue_used = 0; \
		results.break_used = 1; \
		results.completed = 1; \
		results.loop_count++; \
		exit_strategy; \
		\
		results.break_used = 0; \
		results.normal_exit = 1; \
		break; \
	}

#define FOR_BLOCK(exit_strategy) \
	for(; !results.completed; (results.break_used = 0), (results.continue_used = 1)) \
	{ \
		loop_name = "for-zero"; \
		results.break_used = 1; \
		results.completed = 1; \
		results.loop_count++; \
		exit_strategy; \
		\
		results.break_used = 0; \
		results.normal_exit = 1; \
		break; \
	}


#define LOOP_MACRO(exit_strategy) DO_WHILE_BLOCK(exit_strategy)

#define TEST_A_LOOP \
	reset_fixture(&results); \
	printf("\n"); \
	LOOP_MACRO((void)0); /* Normal exit. */ \
	print_test_results(loop_name, "normal", &results, &expected_for_normal_exit); \
	\
	reset_fixture(&results); \
	LOOP_MACRO(continue); /* Continue-based exit/jump. */ \
	print_test_results(loop_name, "continue-based", &results, &expected_for_continue_exit); \
	\
	reset_fixture(&results); \
	LOOP_MACRO(break); /* Break-based exit/jump. */ \
	print_test_results(loop_name, "break-based", &results, &expected_for_break_exit);

#ifdef LOOP_MACRO
#undef LOOP_MACRO
#endif

#define LOOP_MACRO(exit_strategy) DO_WHILE_BLOCK(exit_strategy)
	TEST_A_LOOP
#undef LOOP_MACRO

#define LOOP_MACRO(exit_strategy) WHILE_BLOCK(exit_strategy)
	TEST_A_LOOP
#undef LOOP_MACRO

#define LOOP_MACRO(exit_strategy) FOR_BLOCK(exit_strategy)
	TEST_A_LOOP
#undef LOOP_MACRO

	int sil_skipped;
	int lis_skipped;

#define PRINT_SWITCH_VS_LOOP_RESULTS(loop_type) \
	if ( sil_skipped == 0 ) \
		printf("  switch-in-loop: 'break' applied to the switch statement\n"); \
	else \
		printf("  switch-in-loop: 'break' applied to the " loop_type " statement\n"); \
	if ( lis_skipped == 0 ) \
		printf("  loop-in-switch: 'break' applied to the " loop_type " statement\n"); \
	else \
		printf("  loop-in-switch: 'break' applied to the switch statement\n"); \
	printf("\n");



	printf("\n");
	printf("Testing switch-case vs do-while behavior.\n");

	sil_skipped = 1;
	do
	{
		switch (1)
		{
			case 1:
				break;
		}
		sil_skipped = 0;
	} while(0);

	lis_skipped = 1;
	switch (1)
	{
		case 1:
			do {
				break;
			}
			while(0);
			lis_skipped = 0;
	}
	PRINT_SWITCH_VS_LOOP_RESULTS("do-while")

	printf("Testing switch-case vs while-loop behavior.\n");

	sil_skipped = 1;
	while(1)
	{
		switch (1)
		{
			case 1:
				break;
		}
		sil_skipped = 0;
		break;
	}

	lis_skipped = 1;
	switch (1)
	{
		case 1:
			while(1) {
				break;
			}
			lis_skipped = 0;
	}
	PRINT_SWITCH_VS_LOOP_RESULTS("while")

	printf("Testing switch-case vs for-loop behavior.\n");

	sil_skipped = 1;
	for(;1;)
	{
		switch (1)
		{
			case 1:
				break;
		}
		sil_skipped = 0;
		break;
	}

	lis_skipped = 1;
	switch (1)
	{
		case 1:
			for(;1;) {
				break;
			}
			lis_skipped = 0;
	}
	PRINT_SWITCH_VS_LOOP_RESULTS("for")

 	return 0;
}
