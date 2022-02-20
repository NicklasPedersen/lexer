
enum TokenKind {
    TOKEN_UNKNOWN,
    TOKEN_NAME,
    TOKEN_NUM,
    TOKEN_STR,
    TOKEN_CHAR,
    TOKEN_SEMICOLON,
    TOKEN_EQ,
    TOKEN_EQ_EQ,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_LT_EQ,
    TOKEN_GT_EQ,
    TOKEN_PLUS_PLUS,
    TOKEN_MINUS_MINUS,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_DOT,
    TOKEN_ARROW,
    TOKEN_BANG,
    TOKEN_BANG_EQ,
    TOKEN_TILDE,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_AMP,
    TOKEN_AMP_AMP,
    TOKEN_PIPE,
    TOKEN_PIPE_PIPE,
    TOKEN_CARET,
    TOKEN_COMMA,
    TOKEN_COLON,

    SIZE_TOKEN_KIND,
};

struct Token {
    enum TokenKind kind;
    char *str;
    int line;
    int int_val;
};

#define MAX_TOKENS (1024 * 2)

struct TokenList {
    struct Token tokens[MAX_TOKENS];
    int num;
};

void checkifeverythingsalrightbro(void);
char const *gettokenkindname(enum TokenKind kind);
struct TokenList lex_with_str(char *buff);
void print_tokens(struct TokenList tkl);

struct Token tokens[MAX_TOKENS];
int num_tokens;
