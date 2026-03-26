#include <stdio.h>
#include <stdlib.h>

char *get_shader(char *shader_path)
{
	FILE *file = fopen(shader_path, "r");
	char *shader;
	size_t fsize;

	if (!file) {
		printf("File openning error [%s]\n", shader_path);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	fsize = (size_t)ftell(file);
	fseek(file, 0, SEEK_SET);

	shader = (char *)calloc(fsize + 1, sizeof(char));

	if (!shader) {
		printf("Memory allocation error\n");
		fclose(file);
		return NULL;
	}

	fread(shader, sizeof(char), fsize, file);

	return shader;
}
