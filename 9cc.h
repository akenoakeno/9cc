#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//記号の種類
typedef enum {
	ND_ADD,
	ND_SUB,
	ND_MUL,
	ND_DIV,
	ND_EQ,
	ND_NE,
	ND_LT,
	ND_LE,
	ND_NUM,
} NodeKind;

typedef struct Node Node;

//ノードの型
struct Node {
	NodeKind kind;
	Node *lhs;
	Node *rhs;
	int val;
};

//トークンの種類
typedef enum {
	TK_RESERVED, //記号トークン
	TK_NUM,      //整数トークン
	TK_EOF,      //入力の終了
} TokenKind;

typedef struct Token Token;

struct Token {
	TokenKind kind;
	Token *next;
	int val;
	char *str;
	int len;
};

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();