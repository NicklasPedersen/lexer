# lexer

This is a more generalized lexer for use in assemblers/compilers

# How to get started

```shell
git clone https://github.com/nicklaspedersen/lexer
git submodule update --init --recursive
```

Then use the lexer like so:

```C
/* lex_with_str expects a null-terminated string */
struct TokenList tkl = lex_with_str(string_that_has_code);
/* you can print all tokens to debug */
print_tokens(tkl);
/* you can loop over the token list */
int current = 0;
while (current < tkl.num) { ... }
```

# TODOs
- [ ] Handle comments properly
- [ ] Decide if we should allow newline as a token since assemblers use it for instruction seperation
- [ ] Dynamically allocate and resize tokenlist buffer as necessary
- [ ] Handling of escape characters
- [ ] Proper handling of other bases than base 10
- [ ] Look into "immutable lexer"
- [ ] Decice if we should allow other tokens into names (for name mangling)

# Interface explanation

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
