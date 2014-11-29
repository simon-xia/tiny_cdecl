/*
 * a tiny parser for basic C declaration using "Spiral Rule"
 *
 *												Simon Xia
 *											Nov. 12, 2014 at UESTC
 */
#include "tiny_cdecl.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

char basic_type [MAX_TOKEN_CNT][MAX_TOKEN_LEN] = {
	"int", 
	"unsigned", 
	"signed", 
	"short", 
	"long", 
	"char", 
	"float", 
	"double",
	"void"
};

void _declaration_parser(Token_set *set, Token_stack *st, int set_index);

// return the special operator's type
static int is_special(char a)
{
	switch (a)
	{
		case '*':	return ASTERISK;
		case '(':	return LEFT_PARENTHESES;
		case ')':	return RIGHT_PARENTHESES;
		case '[':	return LEFT_SQUARE_BRACKETS;
		case ']':	return RIGHT_SQUARE_BRACKETS;
		case ',':	return COMMA;
		case ' ':	return 0;
		default:	return -1;
	}
}

// determine whether the str is basic type
static int is_basic_type(char *str)
{
	int i = 0;
	while (i < MAX_TOKEN_CNT)
		if (!strcmp(basic_type[i++], str))
			return 1;
	return 0;
}

// determine whether the str is pure number
static int is_number(char *str)
{
	int i = 0, len = strlen(str);
	while (i < len)
		if (!isdigit(str[i++]))
			return 0;
	return 1;
}

/****************************
 *	token set operation
 ****************************/

int token_set_init(Token_set **set, int cap)
{
	*set = (Token_set*)malloc(sizeof(Token_set));
	memset(*set, 0, sizeof(Token_set));
	if (!*set) {
		fprintf(stderr, "Init token set error, not enough memory!\n");
		return -1;
	}

	(*set)->size = 0;
	if (!cap)
		cap = DEFAULT_TOKEN_SET_CAP;
	(*set)->cap = cap;
	(*set)->set = (Token*)malloc(sizeof(Token) * cap);

	if (!(*set)->set) {
		fprintf(stderr, "Init token set error, not enough memory!\n");
		return -1;
	}

	memset((*set)->set, 0, sizeof(Token) * cap);
	return 0;
}

void token_set_destroy(Token_set *set)
{
	free(set->set);
	free(set);
}

int token_set_insert(Token_set *set, int type, char *token)
{
	if (set->size == set->cap)
		return -1;

	set->set[set->size].type = type;
	memcpy(set->set[set->size].token, token, strlen(token));
	set->size++;

	return 0;
}

void token_set_display(Token_set *set)
{
	int i;
	printf("***Tokens***\n");
	for (i = 0; i < set->size; i++)
		printf("%d\t%s\n", set->set[i].type, set->set[i].token);
	printf("************\n");
}

void token_set_reset(Token_set *set)
{
	set->size = 0;
	memset(set->set, 0, sizeof(Token)*(set->cap));
}


/****************************
 *	token stack operation
 ****************************/
int token_stack_init(Token_stack **st, int cap)
{
	*st = (Token_stack*)malloc(sizeof(Token_stack));
	memset(*st, 0, sizeof(Token_stack));
	if (!*st) {
		fprintf(stderr, "Init stack error, not enough memory!\n");
		return -1;
	}

	(*st)->size = 0;
	if (!cap)
		cap = DEFAULT_TOKEN_SET_CAP;
	(*st)->cap = cap;
	(*st)->stack = (Token*)malloc(sizeof(Token) * cap);
	if (!(*st)->stack) {
		fprintf(stderr, "Init stack error, not enough memory!\n");
		return -1;
	}

	memset((*st)->stack, 0, sizeof(Token) * cap);
	return 0;
}

void token_stack_destroy(Token_stack *st)
{
	free(st->stack);
	free(st);
}

int token_stack_push(Token_stack *st, Token t)
{
	if (st->size == st->cap)
		return -1;

	st->stack[st->size].type = t.type;
	memcpy(st->stack[st->size].token, t.token, strlen(t.token));
	st->size++;
	return 0;
}

// pop return void
void token_stack_pop(Token_stack *st)
{
	st->size--;
}


// cut the input string into tokens and classify them, then store them into token set
int token_parser(Token_set *set, char *str)
{
	if (!set)
		return -1;

	int len = strlen(str), i, j = 0, ret;
	char tmp[MAX_TOKEN_LEN] = {0};
	for (i = 0; i < len; i++)
	{
		ret = is_special(str[i]);
		if (-1 == ret) {
			tmp[j++] = str[i];
			continue;
		}
		else if (0 == ret) {
			if (tmp[0] == 0)
				continue;
			if (is_basic_type(tmp))	
				token_set_insert(set, BASIC_TYPE, tmp);
			else if (is_number(tmp))
				token_set_insert(set, NUMBER, tmp);
			else
				token_set_insert(set, IDENTIFIER, tmp);
			memset(tmp, 0, MAX_TOKEN_LEN);
			j = 0;
		}
		else {
			if (tmp[0] != 0) {
				if (is_basic_type(tmp))	
					token_set_insert(set, BASIC_TYPE, tmp);
				else if (is_number(tmp))
					token_set_insert(set, NUMBER, tmp);
				else
					token_set_insert(set, IDENTIFIER, tmp);
				memset(tmp, 0, MAX_TOKEN_LEN);
				j = 0;
			}
			token_set_insert(set, ret, tmp);
		}
	}
	return 0;
}

//find the first matched right parentheses in set from given index
static int find_pair_right_parentheses(Token_set *set, int index)
{
	int i = index;
	int left_parentheses_cnt = 0; // count the nested parentheses 

	for (; i < set->size; i++)
	{
		if (set->set[i].type == LEFT_PARENTHESES)
			left_parentheses_cnt++;
		if (set->set[i].type == RIGHT_PARENTHESES) {
			if (!left_parentheses_cnt)
				return i;
			else
				left_parentheses_cnt--;
		}
	}
	return -1;
}

// index is the '['
static void parser_for_array(Token_set *set, int index)
{
	int array_size = 0;

	printf("array ");
	while (set->set[index].type == LEFT_SQUARE_BRACKETS) 
	{
		if (set->set[++index].type == NUMBER) {
			array_size = atoi(set->set[index].token);
			index++;
		}

		if (set->set[index++].type == RIGHT_SQUARE_BRACKETS)
			printf("[%s]", array_size ? set->set[index - 2].token: " ");
		else 
			fprintf(stderr, "parsing array error\n");
	}
	printf(" of ");
}

// index is the first token of arg
static int parser_for_function_arg(Token_set *set, int index)
{
	Token_stack *st;
	int i = index, left_parentheses_cnt = 0, start = index, end = -1;

	//mark the end of each arg, it is ',' or ')'
	for ( ; start < set->size; start++)
	{
		if (set->set[start].type == LEFT_PARENTHESES)
			left_parentheses_cnt++;
		else if (set->set[start].type == RIGHT_PARENTHESES) {
			if (!left_parentheses_cnt) {
				end = start;
				break;
			}
			else
				left_parentheses_cnt--;
		}
		else if (set->set[start].type == COMMA) {
			if (!left_parentheses_cnt) {
				end = start;
				break;
			}
		}
	}

	//recursively parser the identifier in args
	token_stack_init(&st, 10);
	while (set->set[i].type != IDENTIFIER && i != end)
	{
		token_stack_push(st, set->set[i++]);
	}

	if (set->set[i].type == IDENTIFIER) {
		printf(" \"%s\" a ", set->set[i].token);
		i++;
	}
	else
		i--;

	_declaration_parser(set, st, i);

	if (set->set[end].type == COMMA) {
		printf(", ");
	}
	else if (set->set[end].type == RIGHT_PARENTHESES) {
		end--;  //match i in loop of parser function
	}
	token_stack_destroy(st);
	return end;
}

// index is '('
void parser_for_function(Token_set *set, int index)
{
	int i;
	printf("function ( ");
	for (i = index + 1; set->set[i].type != RIGHT_PARENTHESES; i++)
	{
		i = parser_for_function_arg(set, i);
	}
	printf(") returning ");
}

/**
 * a recursive function to parser the declaration
 * @param set:		a pointer to Token set which is used to store the classified token
 * @param stack:	a pointer to Token stack which is used to help parser
 * @set_index:		index of token after the IDENTIFIER in Token set
 */
void _declaration_parser(Token_set *set, Token_stack *st, int set_index)
{
	if (set->set[set_index].type == LEFT_SQUARE_BRACKETS)
		parser_for_array(set, set_index);
	else if (set->set[set_index].type == LEFT_PARENTHESES)
		parser_for_function(set, set_index);
	
	while (st->size != 0)
	{
		if (st->stack[st->size - 1].type == LEFT_PARENTHESES) {
			token_stack_pop(st);
			_declaration_parser(set, st, find_pair_right_parentheses(set, set_index) + 1);
		}
		else if (st->stack[st->size - 1].type == ASTERISK) {
			token_stack_pop(st);
			printf("pointer to ");
		}
		else if (st->stack[st->size - 1].type == BASIC_TYPE) {
			printf("%s ", st->stack[st->size-1].token);
			token_stack_pop(st);
		}
	}
}


// parser the declaration
int declaration_parser(Token_set *set, Token_stack *st)
{
	int i = 0;
	while (set->set[i].type != IDENTIFIER)
		token_stack_push(st, set->set[i++]);
	printf("declare %s as a ", set->set[i].token);
	_declaration_parser(set, st, ++i);
	printf("\n");
	return 0;
}
