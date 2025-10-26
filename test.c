#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"

int main(void) {
	FILE *fh = fopen("test.txt", "r");
	if (fh == NULL) {
		fprintf(stderr, "Failed to open test.txt\n");
		return 1;
	}
	char *data = malloc(512);
	if (data == NULL) {
		fprintf(stderr, "Failed to malloc text buffer.\n");
		fclose(fh);
		return 1;
	}
	
	printf("Reading file...\n");
	size_t data_length = 0;
	char c = 0;
	while (c != EOF) {
		c = fgetc(fh);
		data[data_length] = c;
		data_length++;
	} 
	data[data_length] = '\0';	
	fclose(fh);
	printf("Done reading file.\n");

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
		token_print(&(tokens[i]));

	tokens_destroy(tokens, tokens_length);
	return 0;
}
