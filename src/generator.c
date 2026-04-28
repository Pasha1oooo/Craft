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

	loaded_chunks = (struct chunk *)calloc(CHUNKS_AMOUNT,
	                                       sizeof(struct chunk));

	for (int i = 0; i < CHUNKS_AMOUNT; i++) {
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
}

void get_chunks(struct chunk *chunks, vec3 player_vec_pos)
{
	struct position player_pos = vec2pos(player_vec_pos);
	struct position player_chunk = gpos2chunkpos(&player_pos);
	struct position local_chunk_pos;

	for (int i = 0; i < CHUNKS_AMOUNT; i++) {
		local_chunk_pos = index2lpos(i, CHUNKS_SIDE);

		chunks[i].pos->x = player_chunk.x + local_chunk_pos.x -
		                   (RENDER_DISTANCE - 1);
		chunks[i].pos->y = player_chunk.y + local_chunk_pos.y -
		                   (RENDER_DISTANCE - 1);
		chunks[i].pos->z = player_chunk.z + local_chunk_pos.z -
		                   (RENDER_DISTANCE - 1);

		gen_chunk(&(chunks[i]));
	}
}

void gen_chunk(struct chunk *chunk)
{
	struct position lpos, gpos;
	char *chunk_file_name = get_chunk_name(chunk->pos);
	FILE *chunk_file = fopen(chunk_file_name, "rb");

	if (chunk_file) {
		load_chunk(chunk->chunk_data, chunk_file);
		fclose(chunk_file);
	} else {
		for (int i = 0; i < BLOCKS_IN_CHUNK; i++) {
			lpos = index2lpos(i, CHUNK_SIZE);
			gpos = lpos2gpos(&lpos, chunk->pos);

			chunk->chunk_data[i] = chunk_gen_logic(&gpos);
		}

		save_chunk(chunk->chunk_data, chunk_file_name);
	}
}

void save_chunk(char *chunk_data, char *file_name)
{
	FILE *file = fopen(file_name, "wb");

	fwrite(chunk_data, sizeof(char), BLOCKS_IN_CHUNK, file);
	fclose(file);
}

void load_chunk(char *chunk_data, FILE *file)
{
	fread(chunk_data, sizeof(char), BLOCKS_IN_CHUNK, file);
}

char chunk_gen_logic(struct position *gpos)
{
	float perlin_value, biome_value;
	char result = AIR;
	vec3 point, biome_point, vec_pos;
/*
	pos2vec(gpos, vec_pos);

	point[0] = vec_pos[0] / 20.0f;
	point[1] = vec_pos[1] / 20.0f;
	point[2] = vec_pos[2] / 10.0f;

	biome_point[0] = vec_pos[0] / 200.0f;
	biome_point[1] = vec_pos[1] / 200.0f;
	biome_point[2] = vec_pos[2] / 100.0f;

	perlin_value = glm_perlin_vec3(point);
	biome_value = glm_perlin_vec3(biome_point);

	if ((perlin_value + 1.0f) * (biome_value + 1.0f) <= 1.0f) {
		result = STONE;
	}

	if (result == STONE) {
		point[0] = vec_pos[0] / 10.0f;
		point[1] = vec_pos[1] / 10.0f;
		point[2] = vec_pos[2] / 10.0f;

		perlin_value = glm_perlin_vec3(point);

		if (perlin_value <= -0.5f)
			result = ORE;
	}

	return result;
*/

	struct position pos;
	int R = 10;

	pos.x = gpos->x;
	pos.y = gpos->y;
	pos.z = gpos->z;

	int distance = pow(pos.x, 2) + pow(pos.y, 2) + pow(pos.z, 2);

	if(distance < pow(R, 2))
	{
		if (pos.z >= 0) {
			return AIR;
		} else {
			return STONE;
		}
	}

	int is_block[3] = {1, 1, 1};

	while (pos.x || pos.y || pos.z) {
		is_block[0] = abs(pos.x) % 3 == 1;
		is_block[1] = abs(pos.y) % 3 == 1;
		is_block[2] = abs(pos.z) % 3 == 1;

		pos.x /= 3;
		pos.y /= 3;
		pos.z /= 3;

		if (is_block[0] + is_block[1] + is_block[2] >= 2)
			return AIR;
	}

	return STONE;
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

struct position gpos2chunkpos(struct position *pos)
{
	struct position chunk_pos;

	if (pos->x >= 0) {
		chunk_pos.x = pos->x / CHUNK_SIZE;
	} else {
		chunk_pos.x = -((abs(pos->x) + 1) / CHUNK_SIZE);
	}

	if (pos->y >= 0) {
		chunk_pos.y = pos->y / CHUNK_SIZE;
	} else {
		chunk_pos.y = -((abs(pos->y) + 1) / CHUNK_SIZE);
	}

	if (pos->z >= 0) {
		chunk_pos.z = pos->z / CHUNK_SIZE;
	} else {
		chunk_pos.z = -((abs(pos->z) + 1) / CHUNK_SIZE);
	}

	return chunk_pos;
}

struct position gpos2lpos(struct position *global)
{
	struct position local;

	if (global->x >= 0) {
		local.x = global->x % CHUNK_SIZE;
	} else {
		local.x = CHUNK_SIZE - abs(global->x) % CHUNK_SIZE;
	}

	if (global->y >= 0) {
		local.y = global->y % CHUNK_SIZE;
	} else {
		local.y = CHUNK_SIZE - abs(global->y) % CHUNK_SIZE;
	}

	if (global->z >= 0) {
		local.z = global->z % CHUNK_SIZE;
	} else {
		local.z = CHUNK_SIZE - abs(global->z) % CHUNK_SIZE;
	}

	return local;
}

struct position lpos2gpos(struct position *lpos, struct position *chunk_pos)
{
	struct position gpos;

	gpos.x = chunk_pos->x * CHUNK_SIZE + lpos->x;
	gpos.y = chunk_pos->y * CHUNK_SIZE + lpos->y;
	gpos.z = chunk_pos->z * CHUNK_SIZE + lpos->z;

	return gpos;
}

struct position vec2pos(vec3 vec_pos)
{
	struct position pos;

	pos.x = (int)floorf(vec_pos[0]);
	pos.y = (int)floorf(vec_pos[1]);
	pos.z = (int)floorf(vec_pos[2]);

	return pos;
}

void pos2vec(struct position *pos, vec3 vec_pos)
{
	vec_pos[0] = (float)pos->x;
	vec_pos[1] = (float)pos->y;
	vec_pos[2] = (float)pos->z;
}

struct position index2lpos(int index, const int SIDE)
{
	struct position pos;

	pos.x =  index % SIDE;
	pos.y = (index / SIDE) % SIDE;
	pos.z = (index / SIDE) / SIDE;

	return pos;
}

int lpos2index(struct position *pos, const int SIDE)
{
	int index = pos->z * SIDE * SIDE +
	            pos->y * SIDE +
	            pos->x;

	return index;
}

