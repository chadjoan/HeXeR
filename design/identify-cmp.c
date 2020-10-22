
#include <stdio.h>

#define IS_GTE(op) printf(#op " identified as >=\n")
#define IS_LTE(op) printf(#op " identified as <=\n")
#define IS_EQ(op)  printf(#op " identified as ==\n")
#define IS_GT(op)  printf(#op " identified as >\n")
#define IS_LT(op)  printf(#op " identified as <\n")
#define IS_NE(op)  printf(#op " identified as !=\n")

#define IDENTIFY_STMT(op) \
	if ( (1 op 1) ) { \
		if ( (1 op 0) ) \
			IS_GTE(op); \
		else \
		if ( (0 op 1) ) \
			IS_LTE(op); \
		else \
			IS_EQ(op); \
	} else { \
		if ( (1 op 0) ) \
		{ \
			if ( (0 op 1) ) \
				IS_NE(op); \
			else \
				IS_GT(op); \
		} \
		else \
			IS_LT(op); \
	}

// How gawd I have to make it expand as an expression. Ugh.
#define IDENTIFY_EXPR(op) \
	( (1 op 1) ? ( \
		( (1 op 0) ? (\
			IS_GTE(op) \
		) : ( \
		( (0 op 1) ? ( \
			IS_LTE(op) \
		) : ( \
			IS_EQ(op) \
		)))) \
	) : ( \
		( (1 op 0) ? ( \
			( (0 op 1) ? ( \
				IS_NE(op) \
			) : ( \
				IS_GT(op) \
			)) \
		) : ( \
			IS_LT(op) \
		)) \
	))

#define IDENTIFY(op) IDENTIFY_EXPR(op)

int main( int argc, const char *argv[] )
{
	IDENTIFY(==);
	IDENTIFY(<);
	IDENTIFY(>);
	IDENTIFY(!=);
	IDENTIFY(<=);
	IDENTIFY(>=);
	return 0;
}

