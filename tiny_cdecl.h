#ifndef _SIMON_CDECL_H_
#define _SIMON_CDECL_H_

#define MAX_TOKEN_LEN			32
#define DEFAULT_TOKEN_SET_CAP	32
#define MAX_TOKEN_CNT			9

typedef enum _token_type_tag {
		IDENTIFIER	= 1111,			
		BASIC_TYPE,				// int, char etc
		ASTERISK,				// *
		LEFT_PARENTHESES,		// (
		RIGHT_PARENTHESES,		// )
		LEFT_SQUARE_BRACKETS,	// {
		RIGHT_SQUARE_BRACKETS,	// }
		COMMA,					// ,
		NUMBER					// 0-9
}Token_type;

//hold the reserved key words, such as int etc
extern char basic_type [MAX_TOKEN_CNT][MAX_TOKEN_LEN];

typedef struct _token_tag {
	Token_type		type;
	char	token[MAX_TOKEN_LEN];
}Token;

//structure to hold tokens
typedef struct _token_set_tag {
	int		cap;
	int		size;
	Token	*set;
}Token_set;

//structure stack to help to parser the token
typedef struct _token_stack_tag {
	int cap;
	int size;
	Token *stack;
}Token_stack;

/**
 * cut the input string into tokens and classify them, then store them into token set
 * @param set:	a pointer to Token set which is used to store the classified token
 * @param str:	a pointer to input string
 * return:	0 if success, -1 if failed
 */
int token_parser(Token_set *set, char *str);

/**
 * parser the declaration, a wrapper of _declaration_parser
 * @param set:		a pointer to Token set which has stored the classified token
 * @param stack:	a pointer to Token stack which is used to help parser and has been initialized
 * return:			0 if success, -1 if failed
 */
int declaration_parser(Token_set *set, Token_stack *st);

/**
 * token set operation
 */
int token_set_init(Token_set **set, int cap);
void token_set_destroy(Token_set *set);
int token_set_insert(Token_set *set, int type, char *token);
void token_set_display(Token_set *set);
void token_set_reset(Token_set *set);


/**
 * token stack operation
 */
int token_stack_init(Token_stack **st, int cap);
void token_stack_destroy(Token_stack *st);
int token_stack_push(Token_stack *st, Token t);
void token_stack_pop(Token_stack *st);

#endif
