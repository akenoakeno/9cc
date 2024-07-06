#include "9cc.h"

//トークン
Token *token;

//入力文字列
char *user_input;

//エラー内容の報告
void error_at(char *loc, char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, " ");
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

//tokenの種類が指定したものであれば真を返して次へ進み、そうでなければ偽を返す
bool consume(char *op) {
	if (token->kind != TK_RESERVED || 
		strlen(op) != token->len ||
		memcmp(token->str, op, token->len)) {
		return false;
	}
	token = token->next;
	return true;
}

//tokenの種類が指定したものであれば次へ進み、そうでなければエラーを返す
void expect(char *op) {
	if (token->kind != TK_RESERVED || 
		strlen(op) != token->len ||
		memcmp(token->str, op, token->len)) 
		error_at(token->str, "'%c'ではありません", op);
	token = token->next;
}

//tokenが数値であればそれを返して次へ進み、そうでなければエラーを返す
int expect_number() {
	if (token->kind != TK_NUM) 
		error_at(token->str, "数値ではありません");
	int val = token->val;
	token = token->next;
	return val;
}

bool at_EOF() {
	return token->kind == TK_EOF;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "引数の個数が正しくありません\n");
		return 1;
	}

	user_input = argv[1];
	token = tokenize(argv[1]);
	Node *node = expr();

	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	gen(node);

	printf("	pop rax\n");
	printf("	ret\n");
	return 0;
} 
