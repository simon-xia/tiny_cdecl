#include "tiny_cdecl.h"

#define TEST_SIZE	30

int main()
{
	/* It's essential to initialize token set and token stack */
	Token_set *T;
	Token_stack *S;
	token_set_init(&T, TEST_SIZE);
	token_stack_init(&S, TEST_SIZE);

	/* more test case */
	//token_parser(T, "char *str[5][10] ");
	//token_parser(T, "char *(*fp)(int, float *)");
	//token_parser(T, "char *(*fp)(int, float *, short* )");
	//token_parser(T, "int (*(*foo)(void ))[3]");

	token_parser(T, "void (*signal(int sig, void (*func)(int)))(int)");

	/* Uncomment this to display the token set after token parser */
	//token_set_display(T);

	declaration_parser(T,S);

	/* some clean work */
	token_stack_destroy(S);
	token_set_destroy(T);

	return 0;
}
