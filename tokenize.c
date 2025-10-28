#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"

void print_usage() {
	printf("Usage: tokenizer [SOURCE FILE]\n");
}

int main(int argc, char **argv) {
	if (argc < 2) {
		print_usage();
		return 1;
	}

	FILE *fh = fopen(argv[1], "r");
	if (fh == NULL) {
		fprintf(stderr, "Failed to open \"%s\"\n", argv[1]);
		return 1;
	}


	// Create a buffer to hold file contents
	size_t data_length = 0;
	size_t data_capacity = 512;
	char *data = malloc(data_capacity);
	if (data == NULL) {
		fprintf(stderr, "Failed to allocate initial %zu bytes for the text buffer that contains the file's contents.\n", data_capacity);
		fclose(fh);
		return 1;
	}


	// Read file
	printf("Reading file...\n");
	char c = 0;
	void *realloc_ptr = NULL;
	while (c != EOF) {
		c = fgetc(fh);
		data[data_length] = c;
		data_length++;
		if (data_length >= data_capacity) {
			data_capacity *= 2;
			realloc_ptr = realloc(data, data_capacity);
			if (realloc_ptr == NULL) {
				fprintf(stderr, "Failed to reallocate memory for the text buffer containing the file's contents. Reallocation target was %zu bytes.\n", data_capacity);
				free(data);
				fclose(fh);
				return 1;
			}
			data = (char*) realloc_ptr;	
		}
	} 
	data[data_length] = '\0';	
	fclose(fh);
	printf("Done reading file.\n");


	// Tokenize
	size_t tokens_length = 0;
	size_t tokens_capacity = 128;
	printf("Attempting to tokenize...\n");
	struct Token *tokens = tokenize(data, data_length, &tokens_length, &tokens_capacity);
	if (tokens == NULL) {
		fprintf(stderr, "Failed to tokenize.\n");
		free(data);
		return 1;
	}
	
	for (size_t i = 0; i < tokens_length; i++) 
		lex( &(tokens[i]) );

	for (size_t i = 0; i < tokens_length; i++)
		token_print(&(tokens[i]));

	tokens_destroy(tokens, tokens_length);
	free(data);
	return 0;
}
