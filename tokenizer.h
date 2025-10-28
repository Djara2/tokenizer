#ifndef TOKENIZER_H
#define TOKENIZER_H
#define USED_FLAG_BITS 2
#ifndef __x86_64__
#define UNUSED_FLAG_BITS 62
#else 
#define UNUSED_FLAG_FITS 30
#endif
enum TokenType { 
	TOKEN_TYPE_NONE,
	TOKEN_TYPE_VAR_ID, 
	TOKEN_TYPE_FUNCTION_ID,
	TOKEN_TYPE_STRUCT_ID,
	TOKEN_TYPE_STRING_LITERAL,
	TOKEN_TYPE_INTEGER_LITERAL,
	TOKEN_TYPE_FLOAT_LITERAL,
	TOKEN_TYPE_KEYWORD,
	TOKEN_TYPE_LEFT_PARENTHESIS,
	TOKEN_TYPE_RIGHT_PARENTHESIS,
	TOKEN_TYPE_LEFT_BRACKET,
	TOKEN_TYPE_RIGHT_BRACKET, 
	TOKEN_TYPE_ASSIGNMENT,
	TOKEN_TYPE_EQUALS,
	TOKEN_TYPE_PLUS,
	TOKEN_TYPE_MINUS,
	TOKEN_TYPE_ASTERISK,
	TOKEN_TYPE_SLASH,
	TOKEN_TYPE_DOT,
	TOKEN_TYPE_LEFT_BRACE,
	TOKEN_TYPE_RIGHT_BRACE
};

enum TokenizerStateFlag {
	FLAG_IGNORE_WHITESPACE,
	FLAG_BACKSLASH_OPENED,
	FLAG_QUOTE_OPENED,
	FLAG_READING_TOKEN
};

struct TokenizerState {
	unsigned int ingest_whitespace: 1;
	unsigned int backslash_opened: 1;
	unsigned int quote_opened: 1;
	unsigned int reading_token: 1;
};

struct Token {
    char *value;
    unsigned int value_length;
    unsigned int value_capacity;
    enum TokenType type;
};

void tokens_destroy(struct Token *tokens, size_t length);
int tokens_init(struct Token *tokens, size_t length);

enum StorageClass {
    STORAGE_CLASS_UNSIGNED_INT,
    STORAGE_CLASS_SIGNED_INT,
    STORAGE_CLASS_FLOAT,
    STORAGE_CLASS_DOUBLE,
    STORAGE_CLASS_UNSIGNED_CHAR,
    STORAGE_CLASS_SIGNED_CHAR,
    STORAGE_CLASS_STRUCT,
    STORAGE_CLASS_POINTER
};

void token_print(struct Token *token);

// Used to not clutter the instructions within tokenize(). Makes sure the tokens buffer
// is resized as needed.
// Returns a pointer to the next token
struct Token* tokens_advance(struct Token **tokens, size_t *length, size_t *capacity); 

int token_add_character(struct Token *token, char c);

// Handles characters such as "(" and ")" which are treated as a whole token outright
int tokens_handle_special_character(struct Token **tokens, size_t *length, size_t *capacity, struct Token **current_token, struct TokenizerState *state, char c, size_t index);

struct Token* tokenize(char *data, size_t data_length, size_t *tokens_length, size_t *tokens_capacity);
#endif
