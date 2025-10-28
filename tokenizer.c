#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tokenizer.h"
#define DEFAULT_TOKENS_AMOUNT 128

void tokens_destroy(struct Token *tokens, size_t length) {
	if (tokens == NULL) {
		fprintf(stderr, "[%s] ERROR: Provided parameter `struct Token *tokens` is a NULL pointer.\n", __func__);
		return;
	}

	if (length == 0) {
		fprintf(stderr, "[%s] ERROR: Provided parameter `size_t length` is 0.\n", __func__);
		return;
	}

	struct Token *current;
	for (size_t i = 0; i < length; i++) {
		current = &(tokens[i]);
		if (current == NULL)
			continue;

		if (current->value != NULL) 
			free(current->value);
	}

	free(tokens);
}

int tokens_init(struct Token *tokens, size_t length) {
	if (tokens == NULL) {
		fprintf(stderr, "[%s] ERROR: Provided parameter `struct Tokens *tokens` is a NULL pointer.\n", __func__);
		return EXIT_FAILURE;
	}

	if (length == 0) {
		fprintf(stderr, "[%s] ERROR: Provided parameter `size_t length` is 0.\n", __func__);
		return EXIT_FAILURE;
	}
	
	struct Token *current;
	struct TokenMetadata *metadata;
	for (size_t i = 0; i < length; i++) {
		current = &(tokens[i]);
		metadata = &(current->metadata);
		
		// Give the token 32 characters of space for now.
		current->type = TOKEN_TYPE_NONE;
		current->value_length   = 0;
		current->value_capacity = 32;
		current->value = malloc(current->value_capacity);
		if (current->value == NULL) {
			tokens_destroy(tokens, length);
			return EXIT_FAILURE;
		}

		metadata->numeric_digits = 0;
		metadata->dots = 0;
	}

	return EXIT_SUCCESS;
}

void token_print(struct Token *token) {
	if (token == NULL) {
		fprintf(stderr, "[%s] ERROR: Provided value for argument `struct Token *token` is a NULL pointer.\n", __func__);
		return;
	}

	if (token->value != NULL)
		printf("Value: \"%s\"\n", token->value);
	else 
		printf("Value: <NULL (maybe something went wrong?)>\n");
	
	printf("Type: ");
	switch (token->type) {
		case TOKEN_TYPE_NONE:
			printf("TOKEN_TYPE_NONE\n");
			break;

		case TOKEN_TYPE_VAR_ID: 
			printf("TOKEN_TYPE_VAR_ID\n");
			break; 

		case TOKEN_TYPE_FUNCTION_ID: 
			printf("TOKEN_TYPE_FUNCTION_ID\n");	
			break;

		case TOKEN_TYPE_STRUCT_ID: 
			printf("TOKEN_TYPE_STRUCT_ID\n");
			break;

		case TOKEN_TYPE_STRING_LITERAL:
			printf("TOKEN_TYPE_STRING_LITERAL\n");	
			break;

		case TOKEN_TYPE_INTEGER_LITERAL: 
			printf("TOKEN_TYPE_INTEGER_LITERAL\n");	
			break;

		case TOKEN_TYPE_FLOAT_LITERAL:
			printf("TOKEN_TYPE_FLOAT_LITERAL\n");	
			break;

		case TOKEN_TYPE_KEYWORD:
			printf("TOKEN_TYPE_KEYWORD\n");	
			break;

		case TOKEN_TYPE_LEFT_PARENTHESIS:
			printf("TOKEN_TYPE_LEFT_PARENTHESIS\n");	
			break;

		case TOKEN_TYPE_RIGHT_PARENTHESIS:
			printf("TOKEN_TYPE_RIGHT_PARENTHESIS\n");	
			break;

		case TOKEN_TYPE_LEFT_BRACKET: 
			printf("TOKEN_TYPE_LEFT_BRACKET\n");	
			break;

		case TOKEN_TYPE_RIGHT_BRACKET: 
			printf("TOKEN_TYPE_RIGHT_BRACKET\n"); 	
			break; 

		case TOKEN_TYPE_ASSIGNMENT:
			printf("TOKEN_TYPE_ASSIGNMENT\n");	
			break;

		case TOKEN_TYPE_EQUALS:
			printf("TOKEN_TYPE_EQUALS\n");
			break;

		case TOKEN_TYPE_PLUS:
			printf("TOKEN_TYPE_PLUS\n");
			break;

		case TOKEN_TYPE_MINUS:
			printf("TOKEN_TYPE_MINUS\n");
			break;

		case TOKEN_TYPE_ASTERISK:
			printf("TOKEN_TYPE_ASTERISK\n");
			break;

		case TOKEN_TYPE_SLASH: 
			printf("TOKEN_TYPE_SLASH\n");
			break;
			
		case TOKEN_TYPE_DOT: 
			printf("TOKEN_TYPE_DOT\n");
			break;

		case TOKEN_TYPE_LEFT_BRACE: 
			printf("TOKEN_TYPE_LEFT_BRACE\n");
			break;

		case TOKEN_TYPE_RIGHT_BRACE:
			printf("TOKEN_TYPE_RIGHT_BRACE\n");
			break;

		default:
			printf("<UNKNOWN TYPE>\n");	
	}
	printf("\n");
}

int token_add_character(struct Token *token, char c) {
	if (token == NULL) {
		fprintf(stderr, "[%s] ERROR: Provided argument `struct Token *token` is a NULL pointer.\n", __func__);
		return EXIT_FAILURE;
	} 
	
	// Allocate more space for the token's value buffer if necessary
	void *realloc_ptr = NULL;
	if (token->value_length >= token->value_capacity) {
		token->value_capacity *= 2;
		printf("[%s] INFO: Token length (%u) has reached or exceeded capacity. Capacity has been doubled to %u.\n", __func__, token->value_length, token->value_capacity);

		realloc_ptr = realloc(token->value, token->value_capacity);
		if (realloc_ptr == NULL) {
			fprintf(stderr, "[%s] ERROR: Failed to reallocate the `char *value` buffer of the provided token.\n", __func__);
			return EXIT_FAILURE;
		}

		token->value = (char*) realloc_ptr;	
	}
	
	// Add the character
	token->value[token->value_length] = c;
	token->value_length++;
	
	return EXIT_SUCCESS;
}

// Updates the metadata of a token (e.g. sets the proper token type attribute)
int lex(struct Token *token) {
	if (token == NULL) {
		fprintf(stderr, "[%s] ERROR: Provided argument `struct Token *token` is a NULL pointer.\n", __func__);
		return EXIT_FAILURE;
	}
	
	if (token->type != TOKEN_TYPE_NONE) {
		fprintf(stderr, "[%s] INFO: Received a token which has already had its type inferred (condition `token->type != TOKEN_TYPE_NONE` was true).\n", __func__);
		return EXIT_FAILURE;
	}

	struct TokenMetadata *metadata = &(token->metadata);
	if (metadata->numeric_digits == token->value_length) {
		token->type = TOKEN_TYPE_INTEGER_LITERAL;
		return EXIT_SUCCESS;
	}

	if (metadata->dots == 1 && metadata->numeric_digits == (token->value_length) - 1) {
		token->type = TOKEN_TYPE_FLOAT_LITERAL;
		return EXIT_SUCCESS;
	}
	
	// Replace this with a hashset later. It's 2:30AM and I'm a little tired
	if (strcmp(token->value, "int") == 0) {
		token->type = TOKEN_TYPE_KEYWORD;
		return EXIT_SUCCESS;
	}
	else if (strcmp(token->value, "float") == 0) {
		token->type = TOKEN_TYPE_KEYWORD;
		return EXIT_SUCCESS;
	}
	else if (strcmp(token->value, "field") == 0) {
		token->type = TOKEN_TYPE_KEYWORD;
		return EXIT_SUCCESS;
	}
	else if (strcmp(token->value, "constrain") == 0) {
		token->type = TOKEN_TYPE_KEYWORD;
		return EXIT_SUCCESS;
	}
	else if (strcmp(token->value, "of") == 0) {
		token->type = TOKEN_TYPE_KEYWORD;
		return EXIT_SUCCESS;
	}
	else if (strcmp(token->value, "is") == 0) {
		token->type = TOKEN_TYPE_KEYWORD;
		return EXIT_SUCCESS;
	}
	else if (strcmp(token->value, "size") == 0) {
		token->type = TOKEN_TYPE_KEYWORD;
		return EXIT_SUCCESS;
	} 
	else if (strcmp(token->value, "on") == 0) {
		token->type = TOKEN_TYPE_KEYWORD;
		return EXIT_SUCCESS;
	}
	else
		return EXIT_FAILURE;
}

// Returns a pointer to the next token
struct Token* tokens_advance(struct Token **tokens, size_t *length, size_t *capacity) {
	if (tokens == NULL) {
		fprintf(stderr, "[%s] ERROR: Provided argument `struct Token **tokens` is a NULL pointer.\n", __func__);
		return NULL;
	}

	if (length == NULL) {
		fprintf(stderr, "[%s] ERROR: Provided argument `size_t *length` is a NULL pointer.\n", __func__);
		return NULL;
	}
	
	if (capacity == NULL) {
		fprintf(stderr, "[%s] ERROR: Provided argument `size_t *capacity` is a NULL pointer.\n", __func__);
		return NULL;
	}
	
	(*length)++;
	// Reallocation is required
	if ( (*length) >= (*capacity) ) {
		// Need to remember previous leave-off point, because now the 
		// new Token structs need to be initialized. 
		size_t old_capacity = (*capacity);

		void *realloc_pointer = NULL;
		(*capacity) *= 2;
		realloc_pointer = (void*) realloc((*tokens), sizeof(struct Token) * (*capacity));
		if (realloc_pointer == NULL) {
			fprintf(stderr, "[%s] ERROR: Failed to reallocate the tokens buffer after trying to reallocate for a new capacity of %zu.\n", __func__, (*capacity));
			return NULL;
		}

		(*tokens) = (struct Token*) realloc_pointer;
		struct Token *first_uninitialized_token = &( (*tokens)[(*length)] );
		tokens_init(first_uninitialized_token, (*capacity) - (*length));
	}
	
	struct Token *next_token = &( (*tokens)[(*length)] );
	return next_token;
}

int tokens_handle_special_character(struct Token **tokens, size_t *length, size_t *capacity, struct Token **current_token, struct TokenMetadata **current_metadata, struct TokenizerState *state, char c, size_t index) {
	if (tokens == NULL) {
		fprintf(stderr, "[%s] ERROR: Provided argument `struct Token **tokens` is a NULL pointer.\n", __func__);
		return EXIT_FAILURE;
	}

	if (length == NULL) {
		fprintf(stderr, "[%s] ERROR: Provided argument `size_t *length` is a NULL pointer.\n", __func__);
		return EXIT_FAILURE;
	}
	
	if (capacity == NULL) {
		fprintf(stderr, "[%s] ERROR: Provided argument `size_t *capacity` is a NULL pointer.\n", __func__);
		return EXIT_FAILURE;
	}

	if (current_token == NULL) {
		fprintf(stderr, "[%s] ERROR: Provided argument `struct Token **current_token` is a NULL pointer.\n", __func__);
		return EXIT_FAILURE;
	}

	if (current_metadata == NULL) {
		fprintf(stderr, "[%s] ERROR: Provided argument `struct TokenMetdata **current_metadata` is a NULL pointer.\n", __func__);
		return EXIT_FAILURE;
	}

	
	// Case 1: If we are reading a string literal, then this 
	//         special symbol is part of it.
	int status = 0;
	if (state->quote_opened) {
		status = token_add_character( (*current_token), c );
		if (status == EXIT_FAILURE) {
			fprintf(stderr, "[%s] ERROR: Failed to capture token #%zu. Error occurred on character '%c' at index %zu in the data.\n", __func__, (*length), c, index);
			tokens_destroy( 
				(*tokens),
				(*length)
			);
			return EXIT_FAILURE;
		}
	}
	// Case 2: If we are not reading a string literal, then this 
	//         special character is a token of its own. 
	//         We must separate from the current token and mark this
	//         as its own.
	else {
		// If whitespace precedes this token, then (*current_token) is already a pointer
		// to the appropriate token; however, it is possible that an expression was written
		// without spaces. For example 2+3 does not have spaces.
		// We should advance a token only if the current token has non-zero length.
		if ( (*current_token)->value_length > 0 ) {
			(*current_token) = tokens_advance(tokens, length, capacity);
			(*current_metadata) = &((*current_token)->metadata);
			if ( (*current_token) == NULL ) {
				fprintf(stderr, "[%s] Failed to advance to the next token.\n", __func__);
				tokens_destroy(
					(*tokens),
					(*length)
				);
				return EXIT_FAILURE;
			}
		}
		
		
		// Tag the token's type
		switch (c) {
			case '(':
				(*current_token)->type = TOKEN_TYPE_LEFT_PARENTHESIS;
				break;

			case ')':
				(*current_token)->type = TOKEN_TYPE_RIGHT_PARENTHESIS;
				break;
			
			case '=':
				(*current_token)->type = TOKEN_TYPE_ASSIGNMENT;
				break;
			
			case '[':
				(*current_token)->type = TOKEN_TYPE_LEFT_BRACKET;
				break;

			case ']':
				(*current_token)->type = TOKEN_TYPE_RIGHT_BRACKET;
				break;
			
			case '{':
				(*current_token)->type = TOKEN_TYPE_LEFT_BRACE;
				break;

			case '}':
				(*current_token)->type = TOKEN_TYPE_RIGHT_BRACE;
				break;

			case '+':
				(*current_token)->type = TOKEN_TYPE_PLUS;
				break;

			case '-':
				(*current_token)->type = TOKEN_TYPE_MINUS;
				break;

			case '*':
				(*current_token)->type = TOKEN_TYPE_ASTERISK;
				break;

			case '/':
				(*current_token)->type = TOKEN_TYPE_SLASH;
				break;
			
			default:
				fprintf(stderr, "[%s] ERROR: Special character '%c' is unrecognized.\n", __func__, c);
				return EXIT_FAILURE;
		}


		// Set the token with the special value
		status = token_add_character( (*current_token), c );
		if (status == EXIT_FAILURE) {
			fprintf(stderr, "[%s] ERROR: Failed to capture token #%zu. Error occurred on character '%c' at index %zu in the data.\n", __func__, (*length), c, index);
			tokens_destroy( 
				(*tokens), 
				(*length) 
			);
			return EXIT_FAILURE;
		}
		
		// Advance a token yet again
		(*current_token) = tokens_advance(tokens, length, capacity);
		(*current_metadata) = &((*current_token)->metadata);
		if ( (*current_token) == NULL ) {
			fprintf(stderr, "[%s] ERROR: Failed to advance to next token.\n", __func__);
			tokens_destroy( 
				(*tokens), 
				(*length)
			);
			return EXIT_FAILURE;
		}
	}	
}

struct Token* tokenize(char *data, size_t data_length, size_t *tokens_length, size_t *tokens_capacity) {
	if (data == NULL) {
		fprintf(stderr, "[%s] Provided argument `char *data` is a NULL pointer.\n", __func__);
		return NULL;
	}
	
	if (data_length == 0) {
		fprintf(stderr, "[%s] Provided argument `size_t data_length` is 0.\n", __func__);
		return NULL;
	};
	
	if (tokens_length == NULL) {
		fprintf(stderr, "[%s] Provided argument `size_t *tokens_length` is a NULL pointer.\n", __func__);
		return NULL;
	}

	if (tokens_capacity == NULL) {
		fprintf(stderr, "[%s] Provided argument `size_t *tokens_capacity` is a NULL pointer.\n", __func__);
		return NULL;
	}
	
	// Allocate tokens
	printf("[%s] INFO: Allocating tokens buffer... ", __func__);
	struct Token *tokens = malloc(sizeof(struct Token) * 128);
	(*tokens_length) = 0;
	(*tokens_capacity) = 128; // DEFAULT_TOKENS_AMOUNT;
	if (tokens == NULL) {
		fprintf(stderr, "[%s] Failed to allocate default amount of %u tokens on the heap.\n", __func__, DEFAULT_TOKENS_AMOUNT);
		return NULL;
	}
	printf("Done!\n");
	
	// Initialize tokens
	printf("[%s] INFO: Initializing tokens... ", __func__);
	if (tokens_init(tokens, (*tokens_capacity)) == EXIT_FAILURE) {
		fprintf(stderr, "[%s] ERROR: Failed to initialize tokens buffer.\n", __func__);
		tokens_destroy(tokens, (*tokens_capacity));
		return NULL;
	}
	printf("Done!\n");
	
	// Internal structure for keeping track of special behavior (e.g. escaping characters)
	printf("[%s] INFO: Creating TokenizerState struct... ", __func__);
	struct TokenizerState state = {
		.ingest_whitespace = 0, 
		.backslash_opened  = 0,
		.quote_opened      = 0,
		.reading_token     = 0
	}; 
	printf("Done!\n");

	// Fast lookup for special characters. Greatly reduces amount of
	// boilerplate code in the switch statement
	printf("[%s] INFO: Creating special character lookup table... ", __func__);
	unsigned char special_char_lookup_table[256];
	for (int i = 0; i < 255; i++)
		special_char_lookup_table[i] = 0;

	special_char_lookup_table['('] = 1;
	special_char_lookup_table[')'] = 1;
	special_char_lookup_table['['] = 1;
	special_char_lookup_table[']'] = 1;
	special_char_lookup_table['{'] = 1;
	special_char_lookup_table['}'] = 1;
	special_char_lookup_table['+'] = 1;
	special_char_lookup_table['-'] = 1;
	special_char_lookup_table['*'] = 1;
	special_char_lookup_table['/'] = 1;
	printf("Done!\n");
	
	// Create tokens
	size_t index  = 0;
	char c = 0;
	struct Token *current_token = &(tokens[0]);
	struct TokenMetadata *current_metadata = &(current_token->metadata);
	unsigned int status = 0; 
	printf("[%s] INFO: Initiating main loop. Index is %zu. Data length is %zu.\n", __func__, index, data_length);
	while(index < data_length) {
		printf("Index is %zu. Tokens processed is %zu.\n", index, (*tokens_length));
		c = data[index];
		// Case: Dealing with a special character
		printf("[%s] DEBUG: Looking up character \"%c\" in the special character table... ", __func__, c);
		if ( special_char_lookup_table[c] && state.quote_opened == 0 ) {
			printf("Found!\n");
			status = tokens_handle_special_character(
					&tokens, 
					tokens_length, 
					tokens_capacity,
					&current_token,
					&current_metadata,
					&state,
					c,
					index
			);
			
			if (status == EXIT_FAILURE) {
				fprintf(stderr, "[%s] ERROR: Failed to handle special character '%c' at index %zu.\n", __func__, c, index); 
				tokens_destroy(tokens, (*tokens_length));
				return NULL;
			}

			index++;		
			// Do not engage the switch statement
			continue;
		}
		printf("Not found!\n");

		// Case: Dealing with an ordinary character
		switch (c) {
			case EOF:
				printf("[%s] DEBUG: Entered EOF case.\n", __func__);
				return tokens;
				break;

			// Space is ignored and marks the end of a token until
			// something else is read. Just let the whitespace cases fall into
			// each other.
			case '\t':
			case '\n':
			case ' ':
				printf("[%s] DEBUG: Entered whitespace case.\n", __func__);
				// Case 0: Ignore attempts to escape whitespace. Why are you doing that.
				if (state.backslash_opened)
					state.backslash_opened = 0;

				// Case 1: If we are not reading a token, then we 
				//         completely ignore this whitespace. It does not
				//         get added to a token.
				if (! state.reading_token ) {
					index++;
					break;
				}
				
				// Case 2:   We are reading a token
				// Case 2.1: Special case in which we are not ignoring whitespace 
				// (e.g. when within a string literal). In this case, 
				// we are adding the whitespaces to the current token's value
				if (state.ingest_whitespace) {
					if (token_add_character(current_token, c) == EXIT_FAILURE) {
						fprintf(stderr, "[%s] ERROR: Failed to capture token #%zu. Error occurred on character '%c' at index %zu in the data.\n", __func__, (*tokens_length), c, index);
						tokens_destroy(tokens, (*tokens_length));
						return NULL;
					}
				}
				// Case 2.2: If we are reading a token, but we are ignoring spaces, 
				//           then this space means that the current token has ended.
				//           We are ignoring spaces, so this space has no meaning. 
				else {
					if (state.reading_token) {
						state.reading_token = 0;
						current_token = tokens_advance(&tokens, tokens_length, tokens_capacity);
						current_metadata = &(current_token->metadata);
						if (current_token == NULL) {
							fprintf(stderr, "[%s] Failed to advance to next token.\n", __func__);
							tokens_destroy(tokens, (*tokens_length));
							return NULL;
						}
					}
				}

				// If we are ignoring the whitespaces, then we are not 
				// advancing the length of the current token, we are just
				// trying to get through the spaces.
				index++;
				break;
			
			// Escape sequence character
			// (1) Escaping spaces is NOT permitted.
			// (2) Escaping quotes is permitted.
			case '\\':
				printf("[%s] DEBUG: Entered backslash case.\n", __func__);
				// Case 1: We have not read a backslash previously, so 
				//         the next character we read is escaped.
				if (! state.backslash_opened )
					state.backslash_opened = 1;

				// Case 2: We HAVE read a backslash previously, so
				//         the next character is not escaped.
				else 
					state.backslash_opened = 0; 
				
				// Augment current token
				if (token_add_character(current_token, c) == EXIT_FAILURE) {
					fprintf(stderr, "[%s] ERROR: Failed to capture token #%zu. Error occurred on character '%c' at index %zu in the data.\n", __func__, (*tokens_length), c, index);
					tokens_destroy(tokens, (*tokens_length));
					return NULL;
				}
				index++;
				break;

			// Escape spaces  
			case '"':	
				printf("[%s] DEBUG: Entered double quote case.\n", __func__);
				// Case 1: A quote has already been opened, and we are reading a string literal.
				//         This quote EITHER signifies an escaped quote (Case 1.1) OR it signifies
				//         the closing of the string literal (Case 1.2)
				if (state.quote_opened) {
					// Case 1.1: If a backslash was opened, then this quote is not syntactic, 
					//           and actually meant to be ingested as part of the data.
					if (state.backslash_opened) {
						if (token_add_character(current_token, c) == EXIT_FAILURE) {
							fprintf(stderr, "[%s] ERROR: Failed to capture token #%zu. Error occurred on character '%c' at index %zu in the data.\n", __func__, (*tokens_length), c, index);
							tokens_destroy(tokens, (*tokens_length));
							return NULL;
						}
						state.backslash_opened = 0;
						index++;
						break;
					}
					// Case 1.2: If a quote was opened before, AND a backslash was NOT opened,
					//           then this quote closes the string literal. This also denotes 
					//           the end of the token.
					//           SIDE EFFECTS:
					//           (1) Spaces are now IGNORED, since the string literal 
					//               has ended.
					//           (2) If were reading a token before, we are no longer
					//               reading that token
					else {
						// Closing quote for string literal marks the end of the current
						// token and the start of a new token.
						current_token = tokens_advance(&tokens, tokens_length, tokens_capacity);
						current_metadata = &(current_token->metadata);
						if (current_token == NULL) {
							fprintf(stderr, "[%s] ERROR: Failed to advance to next token after processing the final quote in a string literal. Tokens length was %zu and this breaking character was at index %zu.\n", __func__, (*tokens_length), index);
							tokens_destroy(tokens, (*tokens_length));
							return NULL;
						}
						state.quote_opened = 0;
						state.ingest_whitespace = 0;
						state.reading_token = 0;
						index++;
					}
				}
				// Case 2: If a quote was NOT opened before, then this opens it.
				//         SIDE EFFECTS:
				//         (1) Spaces are now NOT ignored, since a string literal
				//             may contain spaces.
				//         (2) If we were not reading a token before, we are now 
				//             reading a token.
				else {
					current_token->type = TOKEN_TYPE_STRING_LITERAL;
					state.quote_opened = 1;
					state.ingest_whitespace = 1;
					state.reading_token = 1;
					// current_token = tokens_advance(&tokens, tokens_length, tokens_capacity);
					index++;
				}	
				break;
					
			// Just reading regular characters, so we add to the token's value
			default:
				printf("[%s] DEBUG: Entered regular character case.\n", __func__);
				printf("[%s] DEBUG: Checking if backslash was opened...\n", __func__);
				if (state.backslash_opened) {
					printf("[%s] DEBUG: Backslash was opened!\n", __func__);
					state.backslash_opened = 0;
				}
				
				/*
				printf("[%s] DEBUG: Checking if reading_token is active...\n", __func__);
				if ( ! state.reading_token ) {
					printf("[%s] DEBUG: Reading token was inactive.\n", __func__);
					printf("Trying to set reading_token to 1.\n");
					printf("Success.\n");
				}
				*/
				
				// Update metdata (useful for lexing)
				switch (c) {
					case '.':
						current_metadata->dots++;
						break;

					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						current_metadata->numeric_digits++;
						break;

					default:
						break;	
				}

				printf("[%s] DEBUG: Calling `token_add_character`... ", __func__);
				if (token_add_character(current_token, c) == EXIT_FAILURE) {
					fprintf(stderr, "[%s] ERROR: Failed to capture token #%zu. Error occurred on character '%c' at index %zu in the data.\n", __func__, (*tokens_length), c, index);
					tokens_destroy(tokens, (*tokens_length));
				}
				else 
					state.reading_token = 1;

				printf("Succeeded.");
				index++;
		} // end switch(c)
	} // end tokenize while
} // end tokenize function
