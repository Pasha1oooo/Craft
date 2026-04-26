/* generation.c */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <cglm/cglm.h>
#include "generator.h"

const char FILE_EXTENSION[] = ".chunk";
const char FILE_PATH[] = "saves/";

struct chunk *init_chunks(void)
{
	struct chunk *loaded_chunks;

	loaded_chunks = (struct chunk *)calloc(BLOCKS_AMOUNT,
	                                       sizeof(struct chunk));

	for (int i = 0; i < BLOCKS_AMOUNT; i++) {
		loaded_chunks[i].chunk_data = (char *)calloc(BLOCKS_IN_CHUNK,
		                                             sizeof(char));

		loaded_chunks[i].pos = (struct position *)calloc(1,
		                                      sizeof(struct position));
	}

	return loaded_chunks;
}

void deinit_chunks(struct chunk *chunks)
{
	for (int i = 0; i < CHUNKS_AMOUNT; i++) {
		free(chunks[i].chunk_data);
		free(chunks[i].pos);
	}

	free(chunks);

	return;
}

void get_chunks(struct chunk *chunks, vec3 player_vector_pos)
{
	struct position player_pos;
	struct position local_chunk_pos;

	player_pos.x = floorf(player_vector_pos[0]);
	player_pos.y = floorf(player_vector_pos[1]);
	player_pos.z = floorf(player_vector_pos[2]);

	for (int i = 0; i < CHUNKS_AMOUNT; i++) {
		local_chunk_pos.x = (i % CHUNKS_SIDE);
		local_chunk_pos.y = (i / CHUNKS_SIDE) % CHUNKS_SIDE;
		local_chunk_pos.z = (i / CHUNKS_SIDE) / CHUNKS_SIDE;

		chunks[i].pos->x = player_pos.x / CHUNK_SIZE +
		                   local_chunk_pos.x - CHUNKS_SIDE / 2;

		chunks[i].pos->y = player_pos.y / CHUNK_SIZE +
		                   local_chunk_pos.y - CHUNKS_SIDE / 2;

		chunks[i].pos->z = player_pos.z / CHUNK_SIZE +
		                   local_chunk_pos.z - CHUNKS_SIDE / 2;

		if (player_pos.x < 0)
			chunks[i].pos->x -= 1;
		if (player_pos.y < 0)
			chunks[i].pos->y -= 1;
		if (player_pos.z < 0)
			chunks[i].pos->z -= 1;

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
		for (int i = 0; i < BLOCKS_IN_CHUNK; i++) {
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
	FILE *file = fopen(file_name, "wb");

	fwrite(chunk_data, sizeof(char), BLOCKS_IN_CHUNK, file);
	fclose(file);

	return;
}

void load_chunk(char *chunk_data, FILE *file)
{
	fread(chunk_data, sizeof(char), BLOCKS_IN_CHUNK, file);

	return;
}

char chunk_gen_logic(struct position *chunk_pos, struct position *local_pos)
{
	char result = AIR;
	vec3 point, biome_point;
	float perlin_value, biome_value;
	int x = chunk_pos->x * CHUNK_SIZE + local_pos->x;
	int y = chunk_pos->y * CHUNK_SIZE + local_pos->y;
	int z = chunk_pos->z * CHUNK_SIZE + local_pos->z;

	point[0] = (float)x / 20.0f;
	point[1] = (float)y / 20.0f;
	point[2] = (float)z / 10.0f;

	biome_point[0] = (float)x / 200.0f;
	biome_point[1] = (float)y / 200.0f;
	biome_point[2] = (float)z / 100.0f;

	perlin_value = glm_perlin_vec3(point);
	biome_value = glm_perlin_vec3(biome_point);

	if ((perlin_value + 1.0f) * (biome_value + 1.0f) <= 1.0f) {
		result = STONE;
	}

	if (result == STONE) {
		point[0] = (float)x / 10.0f;
		point[1] = (float)y / 10.0f;
		point[2] = (float)z / 10.0f;

		perlin_value = glm_perlin_vec3(point);

		if (perlin_value <= -0.5f)
			result = ORE;
	}

	return result;
}

char *get_chunk_name(struct position *chunk_pos)
{
	char *chunk_name = (char *)calloc(CHUNK_NAME_LEN, sizeof(char));

	snprintf(chunk_name, CHUNK_NAME_LEN, "%s%d_%d_%d%s", FILE_PATH,
	                                                     chunk_pos->x,
	                                                     chunk_pos->y,
	                                                     chunk_pos->z,
	                                                     FILE_EXTENSION);

	return chunk_name;
}
