#include "9cc.h"

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
