// hw02.c
// 完全獨立，內含 HW01.c 的所有函式定義，並在程式尾部以遞迴下降方式解析硬編字串，
// 並用 box‐drawing 字元印出 parse tree。執行 ./hw02 即可。

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ==== 以下為 HW01.c 的內容（原封不動引入 hw02） ==== */

#define true 1
#define false 0
#define ERROR_STATE 666

typedef enum {
    TYPE_TOKEN,
    MAIN_TOKEN,
    LEFTPAREN_TOKEN,
    RIGHTPAREN_TOKEN,
    LEFTBRACE_TOKEN,
    ID_TOKEN,
    ASSIGN_TOKEN,
    LITERAL_TOKEN,
    SEMICOLON_TOKEN,
    IF_TOKEN,
    ELSE_TOKEN,
    WHILE_TOKEN,
    EQUAL_TOKEN,
    GREATEREQUAL_TOKEN,
    LESSEQUAL_TOKEN,
    GREATER_TOKEN,
    LESS_TOKEN,
    PLUS_TOKEN,
    MINUS_TOKEN,
    RIGHTBRACE_TOKEN
} Token_Type;

typedef struct token {
    char token[81];
    Token_Type token_type;
    struct token* next;
} token_data;

/* 判別字元類型 */
int is_whitespace(char c) {
    if (c == ' ')  return true;
    if (c == '\t') return true;
    if (c == '\n') return true;
    return false;
}

int is_special_char(char c) {
    if (c == '{' || c == '}' ||
        c == '(' || c == ')' ||
        c == ';' || c == EOF ||
        is_whitespace(c)) return true;
    return false;
}

int is_symbol(char c) {
    if (c == '{' || c == '}' ||
        c == '(' || c == ')' ||
        c == ';') return true;
    return false;
}

int is_alpha(char c) {
    if ((c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        (c == '_')) return true;
    return false;
}

int is_digit(char c) {
    if (c >= '0' && c <= '9') return true;
    return false;
}

int is_operator(char c) {
    if (c == '+' || c == '-' ||
        c == '=' || c == '<' ||
        c == '>') return true;
    return false;
}

/* token linked list 操作 */
void append_token(token_data** arg, char* token, Token_Type Token_Type) {
    token_data* new_token = malloc(sizeof(token_data));
    strcpy(new_token->token, token);
    new_token->token_type = Token_Type;
    new_token->next = NULL;
    if (*arg == NULL) {
        *arg = new_token;
    } else {
        token_data* cur = *arg;
        while (cur->next) cur = cur->next;
        cur->next = new_token;
    }
}

void display_tokens(token_data* head) {
    token_data* cur = head;
    while (cur) {
        printf("%s : %s\n",
               cur->token,
               /* below 為 get_token_type() 回傳的字串 */
               (cur->token_type==TYPE_TOKEN?"TYPE_TOKEN":
                cur->token_type==MAIN_TOKEN?"MAIN_TOKEN":
                cur->token_type==LEFTPAREN_TOKEN?"LEFTPAREN_TOKEN":
                cur->token_type==RIGHTPAREN_TOKEN?"RIGHTPAREN_TOKEN":
                cur->token_type==LEFTBRACE_TOKEN?"LEFTBRACE_TOKEN":
                cur->token_type==ID_TOKEN?"ID_TOKEN":
                cur->token_type==ASSIGN_TOKEN?"ASSIGN_TOKEN":
                cur->token_type==LITERAL_TOKEN?"LITERAL_TOKEN":
                cur->token_type==SEMICOLON_TOKEN?"SEMICOLON_TOKEN":
                cur->token_type==IF_TOKEN?"IF_TOKEN":
                cur->token_type==ELSE_TOKEN?"ELSE_TOKEN":
                cur->token_type==WHILE_TOKEN?"WHILE_TOKEN":
                cur->token_type==EQUAL_TOKEN?"EQUAL_TOKEN":
                cur->token_type==GREATEREQUAL_TOKEN?"GREATEREQUAL_TOKEN":
                cur->token_type==LESSEQUAL_TOKEN?"LESSEQUAL_TOKEN":
                cur->token_type==GREATER_TOKEN?"GREATER_TOKEN":
                cur->token_type==LESS_TOKEN?"LESS_TOKEN":
                cur->token_type==PLUS_TOKEN?"PLUS_TOKEN":
                cur->token_type==MINUS_TOKEN?"MINUS_TOKEN":
                cur->token_type==RIGHTBRACE_TOKEN?"RIGHTBRACE_TOKEN":
                "UNKNOWN"));
        cur = cur->next;
    }
}

void release_token(token_data* head) {
    token_data* cur = head;
    while (cur) {
        token_data* nx = cur->next;
        free(cur);
        cur = nx;
    }
}

/* ==== HW01.c 內容結束 ==== */

/* ==== 以下為遞迴下降解析器＋parse‐tree 印出功能 ==== */

#define NUM_TOK 256

/* 建樹用的 Node 結構 */
typedef struct Node {
    char *label;
    int nchildren;
    struct Node **children;
} Node;

static Node* make_node(const char *lbl) {
    Node *n = malloc(sizeof(Node));
    n->label = strdup(lbl);
    n->nchildren = 0;
    n->children = NULL;
    return n;
}
static void add_child(Node *p, Node *c) {
    p->children = realloc(p->children, sizeof(Node*)*(p->nchildren+1));
    p->children[p->nchildren++] = c;
}
static void free_tree(Node *n) {
    for (int i = 0; i < n->nchildren; i++) free_tree(n->children[i]);
    free(n->children);
    free(n->label);
    free(n);
}

/* box‐drawing 風格印樹 */
void print_tree(Node *node, const char *prefix, int is_last) {
    printf("%s", prefix);
    printf(is_last ? "└── " : "├── ");
    printf("%s\n", node->label);
    char buf[1024];
    for (int i = 0; i < node->nchildren; i++) {
        strcpy(buf, prefix);
        strcat(buf, is_last ? "    " : "│   ");
        print_tree(node->children[i], buf, i == node->nchildren-1);
    }
}

/* 硬編要解析的字串 */
static const char *src = "(1+2+(3+4))+5";
static int pos = 0;
static int token, tokval;

/* 由 src 讀下一個 token */
void next_token() {
    while (src[pos]==' '||src[pos]=='\t'||src[pos]=='\n') pos++;
    char c = src[pos];
    if (c=='\0') {
        token = EOF;
    }
    else if (is_digit(c)) {
        int v = 0;
        while (is_digit(src[pos])) {
            v = v*10 + (src[pos]-'0');
            pos++;
        }
        tokval = v;
        token = NUM_TOK;
    }
    else if (c=='+'||c=='('||c==')') {
        token = c;
        pos++;
    }
    else {
        fprintf(stderr, "Invalid '%c' at %d\n", c, pos);
        exit(1);
    }
}

/* 文法： S→E S' | S'→ε|+S | E→num|(S) */
Node* parse_S();
Node* parse_Sprime();
Node* parse_E();

Node* parse_S() {
    Node *n = make_node("S");
    if (token==NUM_TOK || token=='(') {
        add_child(n, parse_E());
        add_child(n, parse_Sprime());
        return n;
    }
    fprintf(stderr, "Parse error in S, tok=%d\n", token);
    exit(1);
}

Node* parse_Sprime() {
    Node *n = make_node("S'");
    if (token=='+') {
        add_child(n, make_node("+"));
        next_token();
        add_child(n, parse_S());
    }
    else {
        /* token==')' or EOF → ε */
        add_child(n, make_node("ε"));
    }
    return n;
}

Node* parse_E() {
    Node *n = make_node("E");
    if (token==NUM_TOK) {
        char buf[32];
        sprintf(buf, "%d", tokval);
        add_child(n, make_node(buf));
        next_token();
    }
    else if (token=='(') {
        add_child(n, make_node("("));
        next_token();
        add_child(n, parse_S());
        if (token!=')') {
            fprintf(stderr, "Missing ')', tok=%d\n", token);
            exit(1);
        }
        add_child(n, make_node(")"));
        next_token();
    }
    else {
        fprintf(stderr, "Parse error in E, tok=%d\n", token);
        exit(1);
    }
    return n;
}

/* 程式進入點 */
int main() {
    next_token();
    Node *root = parse_S();
    if (token!=EOF) {
        fprintf(stderr, "Extra after expr, tok=%d\n", token);
        free_tree(root);
        return 1;
    }
    print_tree(root, "", 1);
    free_tree(root);
    return 0;
}
