#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    if (c >= 'a' && c <= 'z') return true;
    if (c >= 'A' && c <= 'Z') return true;
    if (c == '_') return true;
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

void append_token(token_data** arg, char* token, Token_Type Token_Type);
void display_tokens(token_data* head);
void release_token(token_data* head);

char* get_token_type(Token_Type Token_Type);

int main() {
    FILE* fp = fopen("test.c", "r");
    if(!fp) {
        fprintf(stderr, "Error: File not found\n");
        fclose(fp);
        return 1;
    }
    // if (argc == 1) {
    //     fp = fopen("test.c", "r");
    //     if (fp == NULL) {
    //         printf("File not found\n");
    //         fclose(fp);
    //         return 0;
    //     }
    // }
    // if (argc == 2) {
    //     fp = fopen(argv[1], "r");
    //     if (fp == NULL) {
    //         printf("File not found\n");
    //         fclose(fp);
    //         return 0;
    //     }
    // }

    //hash
    char chr;
    token_data* head = NULL;

    char record[100] = {0};
    int state = 0;
    int index = 0;
    //+EOF
    while (true) {
        switch (state) {
            case 0:
                chr = getc(fp);
                if (is_whitespace(chr)) {
                    state = 0; 
                    break;
                }
                else if (chr == EOF) {
                    state = 39;
                    break;
                }
                else if (chr == '+') state = 1;
                else if (chr == '-') state = 2;
                else if (chr == '=') state = 3;
                else if (chr == '<') state = 6;
                else if (chr == '>') state = 9;
                else if (is_digit(chr)) state = 12;
                else if (is_symbol(chr)) state = 14;
                else if (chr == 'e') state = 15;
                else if (chr == 'i') state = 18;
                else if (chr == 'm') state = 20;
                else if (chr == 'w') state = 23;
                else if (is_alpha(chr)) state = 37;
                
                record[index] = chr;
                index++;
                break;
            case 1:
                state = 0;
                record[index] = '\0';
                append_token(&head, record, PLUS_TOKEN);
                index = 0;
                break;
            case 2:
                state = 0;
                record[index] = '\0';
                append_token(&head, record, MINUS_TOKEN);
                index = 0;
                break;
            case 3:
                chr = getc(fp);
                if (chr == '=') state = 4;
                else state = 5;
                record[index] = chr;
                index++;
                break;
            case 4:
                state = 0;
                record[index] = '\0';
                append_token(&head, record, EQUAL_TOKEN);
                index = 0;
                break;
            case 5:
                state = 0;
                ungetc(chr, fp);
                record[--index] = '\0';
                append_token(&head, record, ASSIGN_TOKEN);
                index = 0;
                break;
            case 6:
                chr = getc(fp);
                if (chr == '=') state = 7;
                else state = 8;
                record[index] = chr;
                index++;
                break;
            case 7:
                state = 0;
                record[index] = '\0';
                append_token(&head, record, LESSEQUAL_TOKEN);
                index = 0;
                break;
            case 8:
                state = 0;
                ungetc(chr, fp);
                record[--index] = '\0';
                append_token(&head, record, LESS_TOKEN);
                index = 0;
                break;
            case 9:
                chr = getc(fp);
                if (chr == '=') state = 10;
                else state = 11;
                record[index] = chr;
                index++;
                break;
            case 10:
                state = 0;
                record[index] = '\0';
                append_token(&head, record, GREATEREQUAL_TOKEN);
                index = 0;
                break;
            case 11:
                state = 0;
                ungetc(chr, fp);
                record[--index] = '\0';
                append_token(&head, record, GREATER_TOKEN);
                index = 0;
                break;
            case 12:
                chr = getc(fp);
                if (is_digit(chr)) state = 12;
                if (is_operator(chr)) state = 13;
                if (is_special_char(chr)) state = 13;
                if (is_alpha(chr)) state = ERROR_STATE;
                record[index] = chr;
                index++;
                break;
            case 13:
                state = 0;
                ungetc(chr, fp);
                record[--index] = '\0';
                append_token(&head, record, LITERAL_TOKEN);
                index = 0;
                break;
            case 14:
                state = 0;
                record[index] = '\0';
                if (chr == '{') append_token(&head, record, LEFTBRACE_TOKEN);
                if (chr == '}') append_token(&head, record, RIGHTBRACE_TOKEN);
                if (chr == '(') append_token(&head, record, LEFTPAREN_TOKEN);
                if (chr == ')') append_token(&head, record, RIGHTPAREN_TOKEN);
                if (chr == ';') append_token(&head, record, SEMICOLON_TOKEN);
                index = 0;
                break;
            case 15:
                chr = getc(fp);
                if (chr == 'l') state = 16;
                else if (is_alpha(chr) || is_digit(chr)) state = 37;
                else if (is_special_char(chr) || is_operator(chr)) state = 38;
                record[index] = chr;
                index++;
                break;
            case 16:
                chr = getc(fp);
                if (chr == 's') state = 17;
                else if (is_alpha(chr) || is_digit(chr)) state = 37;
                else if (is_special_char(chr) || is_operator(chr)) state = 38;
                record[index] = chr;
                index++;
                break;
            case 17:
                chr = getc(fp);
                if (chr == 'e') state = 27;
                else if (is_alpha(chr) || is_digit(chr)) state = 37;
                else if (is_special_char(chr) || is_operator(chr)) state = 38;
                record[index] = chr;
                index++;
                break;
            case 18:
                chr = getc(fp);
                if (chr == 'f') state = 28;
                else if (chr == 'n') state = 19;
                else if (is_alpha(chr) || is_digit(chr)) state = 37;
                else if (is_special_char(chr) || is_operator(chr)) state = 38;
                record[index] = chr;
                index++;
                break;
            case 19:
                chr = getc(fp);
                if (chr == 't') state = 29;
                else if (is_alpha(chr) || is_digit(chr)) state = 37;
                else if (is_special_char(chr) || is_operator(chr)) state = 38;
                record[index] = chr;
                index++;
                break;
            case 20:
                chr = getc(fp);
                if (chr == 'a') state = 21;
                else if (is_alpha(chr) || is_digit(chr)) state = 37;
                else if (is_special_char(chr) || is_operator(chr)) state = 38;
                record[index] = chr;
                index++;
                break;
            case 21:
                chr = getc(fp);
                if (chr == 'i') state = 22;
                else if (is_alpha(chr) || is_digit(chr)) state = 37;
                else if (is_special_char(chr) || is_operator(chr)) state = 38;
                record[index] = chr;
                index++;
                break;
            case 22:
                chr = getc(fp);
                if (chr == 'n') state = 30;
                else if (is_alpha(chr) || is_digit(chr)) state = 37;
                else if (is_special_char(chr) || is_operator(chr)) state = 38;
                record[index] = chr;
                index++;
                break;
            case 23:
                chr = getc(fp);
                if (chr == 'h') state = 24;
                else if (is_alpha(chr) || is_digit(chr)) state = 37;
                else if (is_special_char(chr) || is_operator(chr)) state = 38;
                record[index] = chr;
                index++;
                break;
            case 24:
                chr = getc(fp);
                if (chr == 'i') state = 25;
                else if (is_alpha(chr) || is_digit(chr)) state = 37;
                else if (is_special_char(chr) || is_operator(chr)) state = 38;
                record[index] = chr;
                index++;
                break;
            case 25:
                chr = getc(fp);
                if (chr == 'l') state = 26;
                else if (is_alpha(chr) || is_digit(chr)) state = 37;
                else if (is_special_char(chr) || is_operator(chr)) state = 38;
                record[index] = chr;
                index++;
                break;
            case 26:
                chr = getc(fp);
                if (chr == 'e') state = 31;
                else if (is_alpha(chr) || is_digit(chr)) state = 37;
                else if (is_special_char(chr) || is_operator(chr)) state = 38;
                record[index] = chr;
                index++;
                break;
            case 27:
                chr = getc(fp);
                if (is_special_char(chr)) state = 32;
                if (is_operator(chr)) state = 32;
                else if (is_alpha(chr) || is_digit(chr)) state = 37;
                record[index] = chr;
                index++;
                break;
            case 28:
                chr = getc(fp);
                if (is_special_char(chr)) state = 33;
                if (is_operator(chr)) state = 33;
                else if (is_alpha(chr) || is_digit(chr)) state = 37;
                record[index] = chr;
                index++;
                break;
            case 29:
                chr = getc(fp);
                if (is_special_char(chr)) state = 34;
                if (is_operator(chr)) state = 34;
                else if (is_alpha(chr) || is_digit(chr)) state = 37;
                record[index] = chr;
                index++;
                break;
            case 30:
                chr = getc(fp);
                if (is_special_char(chr)) state = 35;
                if (is_operator(chr)) state = 35;
                else if (is_alpha(chr) || is_digit(chr)) state = 37;
                record[index] = chr;
                index++;
                break;
            case 31:
                chr = getc(fp);
                if (is_special_char(chr)) state = 36;
                if (is_operator(chr)) state = 36;
                else if (is_alpha(chr) || is_digit(chr)) state = 37;
                record[index] = chr;
                index++;
                break;
            case 32:
                state = 0;
                ungetc(chr, fp);
                record[--index] = '\0';
                append_token(&head, record, ELSE_TOKEN);
                index = 0;
                break;
            case 33:
                state = 0;
                ungetc(chr, fp);
                record[--index] = '\0';
                append_token(&head, record, IF_TOKEN);
                index = 0;
                break;
            case 34:
                state = 0;
                ungetc(chr, fp);
                record[--index] = '\0';
                append_token(&head, record, TYPE_TOKEN);
                index = 0;
                break;
            case 35:
                state = 0;
                ungetc(chr, fp);
                record[--index] = '\0';
                append_token(&head, record, MAIN_TOKEN);
                index = 0;
                break;
            case 36:
                state = 0;
                ungetc(chr, fp);
                record[--index] = '\0';
                append_token(&head, record, WHILE_TOKEN);
                index = 0;
                break;
            case 37:
                chr = getc(fp);
                if (is_alpha(chr) || is_digit(chr)) state = 37;
                else if (is_special_char(chr) || is_operator(chr)) state = 38;
                record[index] = chr;
                index++;
                break;
            case 38:
                state = 0;
                ungetc(chr, fp);
                record[--index] = '\0';
                append_token(&head, record, ID_TOKEN);
                index = 0;
                break;
            case 39:
                fclose(fp);
                display_tokens(head);
                release_token(head);
                return 0;
                break;
            default:
                printf("Error: invaild token\n");
                break;
        }
    }
    return 0;
}

//print node
void display_tokens(token_data* head) {
    token_data* current = head;
    while (current != NULL) {
        printf("%s: %s\n", current->token, get_token_type(current->token_type));
        current = current->next;
    }
}

//free node
void release_token(token_data* head) {
    token_data* current = head;
    token_data* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

void append_token(token_data** arg, char* token,Token_Type Token_Type) {
    token_data* new_token = (token_data*)malloc(sizeof(token_data));
    strcpy(new_token->token, token);
    new_token->token_type = Token_Type;
    new_token->next = NULL;

    if (*arg == NULL) {
        *arg = new_token;
    } else {
        token_data* current = *arg;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_token;
    }
}

char* get_token_type(Token_Type Token_Type) {
    switch (Token_Type) {
        case TYPE_TOKEN:
            return "TYPE_TOKEN";
        case MAIN_TOKEN:
            return "MAIN_TOKEN";
        case LEFTPAREN_TOKEN:
            return "LEFTPAREN_TOKEN";
        case RIGHTPAREN_TOKEN:
            return "RIGHTPAREN_TOKEN";
        case LEFTBRACE_TOKEN:
            return "LEFTBRACE_TOKEN";
        case ID_TOKEN:
            return "ID_TOKEN";
        case ASSIGN_TOKEN:
            return "ASSIGN_TOKEN";
        case LITERAL_TOKEN:
            return "LITERAL_TOKEN";
        case SEMICOLON_TOKEN:
            return "SEMICOLON_TOKEN";
        case IF_TOKEN:
            return "IF_TOKEN";
        case ELSE_TOKEN:
            return "ELSE_TOKEN";
        case WHILE_TOKEN:
            return "WHILE_TOKEN";
        case EQUAL_TOKEN:
            return "EQUAL_TOKEN";
        case GREATEREQUAL_TOKEN:
            return "GREATEREQUAL_TOKEN";
        case LESSEQUAL_TOKEN:
            return "LESSEQUAL_TOKEN";
        case GREATER_TOKEN:
            return "GREATER_TOKEN";
        case LESS_TOKEN:
            return "LESS_TOKEN";
        case PLUS_TOKEN:
            return "PLUS_TOKEN";
        case MINUS_TOKEN:
            return "MINUS_TOKEN";
        case RIGHTBRACE_TOKEN:
            return "RIGHTBRACE_TOKEN";
    }
}