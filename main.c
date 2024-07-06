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

static bool startswith(char *p, char *q) {
  return strncmp(p, q, strlen(q)) == 0;
}

//新しいトークンを作成して現在のトークンcurにつなげる
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->len = len;
	cur->next = tok;
	return tok;
} 

//入力文字列pをトークナイズして文字、数字の連結リストにする
Token *tokenize(char *p) {
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while (*p) {
		//空白文字はスキップ
		if (isspace(*p)) {
			p++;
			continue;
		}

		if (strchr("+-*/()<>", *p)) {
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}

		if (startswith(p, "==") || startswith(p, "!=") ||
			startswith(p, "<=") || startswith(p, ">=")) {
			cur = new_token(TK_RESERVED, cur, p, 2);
			p = p + 2;
			continue;
		}

		if (isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p, 1);
			cur->val = strtol(p, &p, 10);
			continue;
		}

		error_at(p++, "トークナイズできません");
	}

	cur = new_token(TK_EOF, cur, p, 1);
	return head.next;
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
