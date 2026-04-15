/* generation.c */

#include "generator.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <cglm/cglm.h>

const int CHUNK_SIZE = 16;
const int RENDER_DISTANCE = 4;
const int CHUNK_NAME_LEN = 64;
const char CHUNK_FILENAME_EXTENSION[] = ".chunk";
const char FILE_PATH[] = "saves/";

struct chunk *init_chunks(void)
{
	const int SIDE = 2 * RENDER_DISTANCE - 1;
	struct chunk *loaded_chunks = (struct chunk *)calloc((size_t)pow(SIDE, 3),
	                                                 sizeof(struct chunk));

	for (int i = 0; i < (int)pow(SIDE, 3); i++) {

		loaded_chunks[i].chunk_data = (char *)calloc(
		                    (size_t)pow(CHUNK_SIZE, 3), sizeof(char));

		loaded_chunks[i].pos = (struct position *)calloc(1,
		                                      sizeof(struct position));
	}

	return loaded_chunks;
}

void deinit_chunks(struct chunk *chunks)
{
	const int SIDE = 2 * RENDER_DISTANCE - 1;

	for (int i = 0; i < (int)pow(SIDE, 3); i++) {
		free(chunks[i].chunk_data);
		free(chunks[i].pos);
	}

	free(chunks);

	return;
}

void get_chunks(struct chunk *chunks, vec3 player_vector_pos)
{
	struct position player_pos;
	const int SIDE = 2 * RENDER_DISTANCE - 1;
	struct position local_chunk_pos;

	player_pos.x = (int)player_vector_pos[0];
	player_pos.y = (int)player_vector_pos[1];
	player_pos.z = (int)player_vector_pos[2];

	for (int i = 0; i < pow(SIDE, 3); i++) {
		local_chunk_pos.x = (i % SIDE);
		local_chunk_pos.y = (i / SIDE) % SIDE;
		local_chunk_pos.z = (i / SIDE) / SIDE;

		chunks[i].pos->x = player_pos.x / CHUNK_SIZE +
		                                  local_chunk_pos.x - SIDE / 2;

		chunks[i].pos->y = player_pos.y / CHUNK_SIZE +
		                                  local_chunk_pos.y - SIDE / 2;

		chunks[i].pos->z = player_pos.z / CHUNK_SIZE +
		                                  local_chunk_pos.z - SIDE / 2;

		chunks[i].pos->x -= player_pos.x < 0 ? 1 : 0;
		chunks[i].pos->y -= player_pos.y < 0 ? 1 : 0;
		chunks[i].pos->z -= player_pos.z < 0 ? 1 : 0;

		gen_chunk(&(chunks[i]));
	}

	return;
}

void gen_chunk(struct chunk *chunk)
{
	struct position local_pos;
	char *chunk_file_name = get_chunk_name(chunk->pos);
	FILE *chunk_file = fopen(chunk_file_name, "rb");
	char ch = 0;

	if (chunk_file) {
		load_chunk(chunk->chunk_data, chunk_file);
		fclose(chunk_file);
	} else {

		for (int i = 0; i < (int)pow(CHUNK_SIZE, 3); i++) {
			local_pos.x = (i % CHUNK_SIZE);
			local_pos.y = (i / CHUNK_SIZE) % CHUNK_SIZE;
			local_pos.z = (i / CHUNK_SIZE) / CHUNK_SIZE;
			ch = chunk_gen_logic(chunk->pos, &local_pos);
			chunk->chunk_data[i] = ch;
		}

		save_chunk(chunk->chunk_data, chunk_file_name);
	}

	return;
}

void save_chunk(char *chunk_data, char *file_name)
{
	size_t blocks_amount = (size_t)pow(CHUNK_SIZE, 3);
	FILE *file = fopen(file_name, "wb");

	fwrite(chunk_data, sizeof(char), blocks_amount, file);
	fclose(file);

	return;
}

void load_chunk(char *chunk_data, FILE *file)
{
	size_t blocks_amount = (size_t)pow(CHUNK_SIZE, 3);

	if (file)
		fread(chunk_data, sizeof(char), blocks_amount, file);
}

char chunk_gen_logic(struct position *chunk_pos, struct position *local_pos)
{
	int x = chunk_pos->x * CHUNK_SIZE + local_pos->x;
	int y = chunk_pos->y * CHUNK_SIZE + local_pos->y;
	int z = chunk_pos->z * CHUNK_SIZE + local_pos->z;
	int result = ' ';
	vec3 point, biome_point;
	float perlin_value, biome_value;

	point[0] = x / 20.0f;
	point[1] = y / 20.0f;
	point[2] = z / 10.0f;

	biome_point[0] = x / 200.0f;
	biome_point[1] = y / 200.0f;
	biome_point[2] = z / 100.0f;

	perlin_value = glm_perlin_vec3(point);
	biome_value = glm_perlin_vec3(biome_point);

	if ((perlin_value + 1.0f) * (biome_value + 1.0f) <= 1.0f) {
		result = '*';
	}

	if (result == '*') {
		point[0] = (float)x / 10.001f;
		point[1] = (float)y / 10.001f;
		point[2] = (float)z / 10.001f;
		perlin_value = glm_perlin_vec3(point);
		if (perlin_value <= -0.0f) {
			result = '#';
		}
	}

	return result;
}

char *get_chunk_name(struct position *chunk_pos)
{
	char *chunk_name = (char *)calloc(CHUNK_NAME_LEN, sizeof(char));
	char *str_pos = chunk_name + strlen(FILE_PATH);
	size_t extension_len = strlen(CHUNK_FILENAME_EXTENSION);

	strcpy(chunk_name, FILE_PATH);

	str_pos = num2str(str_pos, chunk_pos->x);
	str_pos[0] = '_';
	str_pos = num2str(++str_pos, chunk_pos->y);
	str_pos[0] = '_';
	str_pos = num2str(++str_pos, chunk_pos->z);

	for (size_t i = 0; i < extension_len; i++)
		str_pos[i] = CHUNK_FILENAME_EXTENSION[i];

	return chunk_name;
}

char *num2str(char *str, int num)
{
	int ch = num % 10;

	if (num > 0) {
		if (num / 10 != 0)
			str = num2str(str, num / 10);

		str[0] = ch + '0';
		str++;

	} else if(num < 0) {

		str[0] = '-';
		str = num2str(str + 1, -num);

	} else if (num == 0) {

		str[0] = '0';
		str++;
	}

	return str;
}
