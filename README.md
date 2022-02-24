# lexer

This is a more generalized lexer for use in assemblers/compilers

There are some TODOs:
- [ ] Handle comments properly
- [ ] Decide if we should allow newline as a token since assemblers use it for instruction sepperation
- [ ] Dynamically allocate and resize tokenlist buffer as necessary
- [ ] Handling of escape characters
- [ ] Proper handling of other bases than base 10

```C
// enum that contains all the possible token kinds
// a "TOKEN_NAME" is a "one size fits all" and you have to resovle these after the lexing stage
enum TokenKind { ... };

// The fields of this Token struct is determined by the token kind, except the line number field
// the str field is used by things which needs many-byte buffer to store the information such as a name or string
// int_val are used for integers and chars
struct Token {
    enum TokenKind kind;
    char *str;
    int line;
    int int_val;
};

// This a test function to test if we have added all tokens to the tokenkindname array
void check_tokenkindname_array(void);

// returns the name of the TokenKind value
char const *get_token_kind_name(enum TokenKind kind);

// lexes a standard C-string, it is expected that the string is zero terminated
struct TokenList lex_with_str(char *buff);

// prints_the tokens out in a nice token format
void print_tokens(struct TokenList tkl);
```
