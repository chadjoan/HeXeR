#include <stddef.h>
#include <stdio.h>

#define __func__  (check_func_override(override_func, __func__))

const char *check_func_override(const char *override_func, const char *current_func)
{
	if ( override_func != NULL )
		return override_func;
	else
		return current_func;
}

void main()
{
	const char *override_func = NULL;

	printf("1st __func__ == %s\n", __func__);
	override_func = "Hello world!";

	printf("2nd __func__ == %s\n", __func__);

	override_func = NULL;
	printf("3rd __func__ == %s\n", __func__);
}
