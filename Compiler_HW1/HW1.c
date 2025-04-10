#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char* token;
    char* type;
    struct Node* next;
} Node;

Node* createNode(const char* token, const char* type) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) return NULL;

    newNode->token = strdup(token);
    newNode->type = strdup(type);
    newNode->next = NULL;
    return newNode;
}

void freeList(Node* head) {
    while (head) {
        Node* temp = head;
        head = head->next;
        free(temp->token);
        free(temp->type);
        free(temp);
    }
}

void addToken(Node** head, Node** tail, const char* token, const char* type) {
    Node* node = createNode(token, type);
    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    if (!*head) {
        *head = *tail = node;
    } else {
        (*tail)->next = node;
        *tail = node;
    }
}

int is_space(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

int is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int is_digit(char c) {
    return (c >= '0' && c <= '9');
}

int is_alnum(char c) {
    return is_alpha(c) || is_digit(c);
}


const char* getKeywordToken(const char* word) {
    if (strcmp(word, "int") == 0) return "TYPE_TOKEN";
    if (strcmp(word, "main") == 0) return "MAIN_TOKEN";
    if (strcmp(word, "if") == 0) return "IF_TOKEN";
    if (strcmp(word, "else") == 0) return "ELSE_TOKEN";
    if (strcmp(word, "while") == 0) return "WHILE_TOKEN";
    return "ID_TOKEN";
}

void lexical_error(int c) {
    fprintf(stderr, "Lexical error: unrecognized character '%c' (0x%x)\n", c, c);
}

void scan(FILE* fp, Node** head, Node** tail) {
    int in_char, c;
    char buffer[256];
    int index;

    while ((in_char = fgetc(fp)) != EOF) {
        if (is_space(in_char)) continue;

        // Identifier / keyword
        else if (is_alpha(in_char) || in_char == '_') {
            buffer[0] = in_char;
            index = 1;

            while ((c = fgetc(fp)), is_alnum(c) || c == '_') {
                buffer[index++] = c;
            }
            buffer[index] = '\0';
            ungetc(c, fp);

            addToken(head, tail, buffer, getKeywordToken(buffer));
        }

        // Integer literal
        else if (is_digit(in_char)) {
            buffer[0] = in_char;
            index = 1;

            while (is_digit((c = fgetc(fp)))) {
                buffer[index++] = c;
            }
            buffer[index] = '\0';
            ungetc(c, fp);

            addToken(head, tail, buffer, "LITERAL_TOKEN");
        }

        // Two-char operators
        else {
            char token[3];
            token[0] = in_char;
            token[1] = '\0';

            c = fgetc(fp);
            token[1] = c;
            token[2] = '\0';

            if (in_char == '=' && c == '=') {
                addToken(head, tail, token, "EQUAL_TOKEN");
            } else if (in_char == '>' && c == '=') {
                addToken(head, tail, token, "GREATEREQUAL_TOKEN");
            } else if (in_char == '<' && c == '=') {
                addToken(head, tail, token, "LESSEQUAL_TOKEN");
            } else {
                if (c != EOF) ungetc(c, fp);
                token[1] = '\0';

                switch (in_char) {
                    case '=': addToken(head, tail, token, "ASSIGN_TOKEN"); break;
                    case '>': addToken(head, tail, token, "GREATER_TOKEN"); break;
                    case '<': addToken(head, tail, token, "LESS_TOKEN"); break;
                    case '+': addToken(head, tail, token, "PLUS_TOKEN"); break;
                    case '-': addToken(head, tail, token, "MINUS_TOKEN"); break;
                    case '(': addToken(head, tail, token, "LEFTPAREN_TOKEN"); break;
                    case ')': addToken(head, tail, token, "REFTPAREN_TOKEN"); break;
                    case '{': addToken(head, tail, token, "LEFTBRACE_TOKEN"); break;
                    case '}': addToken(head, tail, token, "REFTBRACE_TOKEN"); break;
                    case ';': addToken(head, tail, token, "SEMICOLON_TOKEN"); break;
                    case '#': addToken(head, tail, token, "HASH_TOKEN"); break;
                    case '.': addToken(head, tail, token, "DOT_TOKEN"); break;
                    case ',': addToken(head, tail, token, "COMMA_TOKEN"); break;
                    default: lexical_error(in_char); break;
                }
            }
        }
    }
}

int main() {
    FILE* fp = fopen("test.c", "r");
    if (!fp) {
        fprintf(stderr, "Cannot open source file.\n");
        return 1;
    }

    Node* head = NULL;
    Node* tail = NULL;

    scan(fp, &head, &tail);
    fclose(fp);

    for (Node* cur = head; cur != NULL; cur = cur->next) {
        printf("%s: %s\n", cur->token, cur->type);
    }

    freeList(head);
    return 0;
}
