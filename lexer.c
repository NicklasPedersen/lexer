#include "lex.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "../util/utilf.h"

#define DEBUG
#ifdef DEBUG
#include <stdio.h>
#include <assert.h>
#else
#define printf (void)
#define assert (void)
#endif

struct Lexer {
    int curr;
    char *buf;
    int len;
    int line;
};

struct Lexer lexer;

struct Tuple {
    char *a;
    enum TokenKind kind;
};

struct Tuple multi_char_tokens[] = {
    {"++", TOKEN_PLUS_PLUS},
    {"--", TOKEN_MINUS_MINUS},
    {"(", TOKEN_LPAREN},
    {")", TOKEN_RPAREN},
    {"[", TOKEN_LBRACKET},
    {"]", TOKEN_RBRACKET},
    {".", TOKEN_DOT},
    {"->", TOKEN_ARROW},
    
    {"==", TOKEN_EQ_EQ},
    {"!=", TOKEN_BANG_EQ},
    {"<=", TOKEN_LT_EQ},
    {">=", TOKEN_GT_EQ},
    {"<", TOKEN_LT},
    {">", TOKEN_GT},

    {"=", TOKEN_EQ},

    {"&&", TOKEN_AMP_AMP},
    {"||", TOKEN_PIPE_PIPE},
    {"!", TOKEN_BANG},

    {"&", TOKEN_AMP},
    {"|", TOKEN_PIPE},
    {"^", TOKEN_CARET},
    {"+", TOKEN_PLUS},
    {"-", TOKEN_MINUS},
    {"*", TOKEN_STAR},
    {"/", TOKEN_SLASH},
    {"~", TOKEN_TILDE},

    {",", TOKEN_COMMA},
    {";", TOKEN_SEMICOLON},
    {":", TOKEN_COLON},
};

char *TokenKindName[SIZE_TOKEN_KIND] = {
    [TOKEN_UNKNOWN] = "TOKEN_UNKNOWN",
    [TOKEN_NAME] = "TOKEN_NAME",
    [TOKEN_NUM] = "TOKEN_NUM",
    [TOKEN_STR] = "TOKEN_STR",
    [TOKEN_CHAR] = "TOKEN_CHAR",
    [TOKEN_SEMICOLON] = "TOKEN_SEMICOLON",
    [TOKEN_EQ] = "TOKEN_EQ",
    [TOKEN_EQ_EQ] = "TOKEN_EQ_EQ",
    [TOKEN_LT] = "TOKEN_LT",
    [TOKEN_GT] = "TOKEN_GT",
    [TOKEN_LT_EQ] = "TOKEN_LT_EQ",
    [TOKEN_GT_EQ] = "TOKEN_GT_EQ",
    [TOKEN_PLUS_PLUS] = "TOKEN_PLUS_PLUS",
    [TOKEN_MINUS_MINUS] = "TOKEN_MINUS_MINUS",
    [TOKEN_PLUS] = "TOKEN_PLUS",
    [TOKEN_MINUS] = "TOKEN_MINUS",
    [TOKEN_LPAREN] = "TOKEN_LPAREN",
    [TOKEN_RPAREN] = "TOKEN_RPAREN",
    [TOKEN_LBRACKET] = "TOKEN_LBRACKET",
    [TOKEN_RBRACKET] = "TOKEN_RBRACKET",
    [TOKEN_DOT] = "TOKEN_DOT",
    [TOKEN_ARROW] = "TOKEN_ARROW",
    [TOKEN_BANG] = "TOKEN_BANG",
    [TOKEN_BANG_EQ] = "TOKEN_BANG_EQ",
    [TOKEN_TILDE] = "TOKEN_TILDE",
    [TOKEN_STAR] = "TOKEN_STAR",
    [TOKEN_SLASH] = "TOKEN_SLASH",
    [TOKEN_AMP] = "TOKEN_AMP",
    [TOKEN_AMP_AMP] = "TOKEN_AMP_AMP",
    [TOKEN_AMP] = "TOKEN_AMP",
    [TOKEN_PIPE] = "TOKEN_PIPE",
    [TOKEN_PIPE_PIPE] = "TOKEN_PIPE_PIPE",
    [TOKEN_CARET] = "TOKEN_CARET",
    [TOKEN_COMMA] = "TOKEN_COMMA",
    [TOKEN_COLON] = "TOKEN_COLON",
};

void print_err(char *fmt, ...) {
    printf("\033[1;31merror: ");
    va_list list;
    va_start(list, fmt);
    vprintf(fmt, list);
    va_end(list);
    printf("\033[0;0m");
    exit(1);
}

void print_info(char *fmt, ...) {
    printf("\033[1;36minfo: ");
    va_list list;
    va_start(list, fmt);
    vprintf(fmt, list);
    va_end(list);
    printf("\033[0;0m");
}

void checkifeverythingsalrightbro(void) {
    for (int i = 0; i < sizeof(TokenKindName)/sizeof(*TokenKindName); i++) {
        if (!TokenKindName[i]) {
            print_err("%d missing in TokenKindName\n", i);
        }
    }
}

char const *gettokenkindname(enum TokenKind kind) {
    if (kind < 0 || kind > sizeof(TokenKindName)/sizeof(*TokenKindName)) {
        print_err("unknown TokenKind: %d\n", kind);
    }
    return TokenKindName[kind];
}

int is_whitespace(char c) {
    return c == ' ' || c == '\n' || c == '\r';
}

void skip_whitespace(struct Lexer *l) {
    while (l->curr < l->len && is_whitespace(l->buf[l->curr])) {
        if (l->buf[l->curr] == '\n') {
            l->line++;
        }
        l->curr++;
    }
}

struct Token lex_str(struct Lexer *l) {
    assert(l->buf[l->curr] == '"');
    l->curr++;
    int start = l->curr;
    while (l->curr < l->len && l->buf[l->curr] != '"') {
        l->curr++;
    }
    if (l->buf[l->curr] != '"') {
        print_err("string not terminated\n");
    }
    l->curr++;
    int strlen = l->curr - start - 1;

    char *str = malloc(strlen + 1);
    for (int i = 0; i < strlen; i++) {
        str[i] = l->buf[start + i];
    }
    str[strlen] = '\0';
    return (struct Token){TOKEN_STR, .str = str};
}

struct Token get_next_token(struct Lexer *l) {
    switch (l->buf[l->curr]) {
    case '\'':
        l->curr++;
        if (l->buf[l->curr] == '\\') {
            l->curr++;
            if (l->buf[l->curr] == '\\') {
                l->curr++;
                if (l->buf[l->curr] == '\'') {
                    l->curr++;
                    return (struct Token){TOKEN_CHAR, .int_val = '\\'};
                }
                print_err("forgot to terminate character escape '\\\\'\n");
            }
            print_err("lexer does not support \\escape characters\n");
        }
        struct Token t = {TOKEN_CHAR, .int_val = l->buf[l->curr]};
        l->curr++;
        if (l->buf[l->curr] != '\'') {
            print_err("character value must be delimited with a \"'\"\n");
        } else {
            l->curr++;
        }
        return t;
    case '"':
        return lex_str(l);
    default:
        for (int i = 0; i < sizeof(multi_char_tokens)/sizeof(*multi_char_tokens); i++) {
            size_t len = strlen(multi_char_tokens[i].a);
            if (l->curr + len <= l->len && strncmp(multi_char_tokens[i].a, l->buf + l->curr, len) == 0) {
                l->curr += len;
                return (struct Token){multi_char_tokens[i].kind};
            }
        }
        if (is_num_f(l->buf[l->curr])) {
            int val = 0;
            while (is_num_f(l->buf[l->curr])) {
                val = val * 10 + l->buf[l->curr] - '0';
                l->curr++;
            }

            struct Token t = (struct Token){TOKEN_NUM, .int_val = val};
            return t;
        } else if (is_id_start_f(l->buf[l->curr])) {
            int start = l->curr;
            while (is_valid_id_f(l->buf[l->curr])) {
                l->curr++;
            }
            int namelen = l->curr - start;

            char *name = malloc(namelen + 1);
            for (int i = 0; i < namelen; i++) {
                name[i] = l->buf[start + i];
            }
            name[namelen] = '\0';
            return (struct Token){TOKEN_NAME, .str = name};
        } else {
            printf("unknown = '%d'\n", l->buf[l->curr]);
            struct Token t = (struct Token){TOKEN_UNKNOWN, .int_val = l->buf[l->curr]};
            l->curr++;
            return t;
        }
    }
}

struct TokenList lex_with_str_debug(char *buff) {
    struct TokenList tl = {0};
    struct Lexer l = {0, buff, strlen(buff)};
    skip_whitespace(&l);
    while (l.curr < l.len && l.buf[l.curr]) {
        print_info("curr %d, tok: %d\n", l.curr, tl.num);
        if (tl.num == MAX_TOKENS) {
            print_err("not enough tokens\n");
        }
        tl.tokens[tl.num] = get_next_token(&l);
        tl.tokens[tl.num].line = l.line;
        printf("line: %d\n", l.line);
        tl.num++;
        skip_whitespace(&l);
    }
    return tl;
}

struct TokenList lex_with_str(char *buff) {
    struct TokenList tl = {0};
    struct Lexer l = {0, buff, strlen(buff)};
    skip_whitespace(&l);
    while (l.curr < l.len && l.buf[l.curr]) {
        if (tl.num == MAX_TOKENS) {
            print_err("not enough tokens\n");
        }
        tl.tokens[tl.num] = get_next_token(&l);
        tl.tokens[tl.num].line = l.line;
        tl.num++;
        skip_whitespace(&l);
    }
    return tl;
}

char *get_multi_char_str(enum TokenKind kind) {
    for (size_t i = 0; i < sizeof(multi_char_tokens)/sizeof(*multi_char_tokens); i++)
    {
        if (kind == multi_char_tokens[i].kind) {
            return multi_char_tokens[i].a;
        }
    }
    return NULL;
}

void print_tokens(struct TokenList tkl) {
    for (int i = 0; i < tkl.num; i++) {
        struct Token t = tkl.tokens[i];
        printf("\"%s\": ", gettokenkindname(t.kind));
        switch (t.kind)
        {
        case TOKEN_NUM:
            printf("%d", t.int_val);
            break;
        case TOKEN_NAME:
            printf("%s", t.str);
            break;
        case TOKEN_STR:
            printf("\"%s\"", t.str);
            break;
        case TOKEN_UNKNOWN:
            printf("[c: %c, d: %d]", t.int_val, t.int_val);
            break;
        case TOKEN_CHAR:
            printf("'%c'", t.int_val);
            break;
        default: {
                char *str = get_multi_char_str(t.kind);
                if (str)
                {
                    printf("%s", str);
                }
                else
                {
                    printf("something went wrong");
                }
            }
            break;
        }
        printf(",\n");
    }
}