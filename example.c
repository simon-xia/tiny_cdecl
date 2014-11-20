#include "tiny_cdecl.h"

int main()
{
	Token_set *T;
	Token_stack *S;
	token_set_init(&T, 30);
	token_stack_init(&S, 30);

	//token_parser(T, "char *str[5][10] ");
//	token_parser(T, "char *(*fp)(int, float *)");
//	token_parser(T, "char *(*fp)(int, float *, short* )");
	//token_parser(T, "void (*signal(int, void (*fp)(int)))(int) ");
	//token_parser(T, "int (*(*foo)(void ))[3]");
	token_parser(T, "void (*signal(int sig, void (*func)(int)))(int)");

	//token_set_display(T);

	declaration_parser(T,S);

	token_stack_destroy(S);
	token_set_destroy(T);
	return 0;
}
