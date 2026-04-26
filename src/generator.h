#ifndef GENERATOR_H
#define GENERATOR_H

#include <cglm/cglm.h>
#include <math.h>

enum GENERATOR_PARAMETERS {
	CHUNK_SIZE = 16,
	RENDER_DISTANCE = 3,
	CHUNK_NAME_LEN = 64,
	CHUNKS_SIDE = 2 * RENDER_DISTANCE - 1,
	CHUNKS_AMOUNT = CHUNKS_SIDE * CHUNKS_SIDE * CHUNKS_SIDE,
	BLOCKS_IN_CHUNK = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE,
	BLOCKS_AMOUNT = BLOCKS_IN_CHUNK * CHUNKS_AMOUNT
};

enum BLOCKS {
	AIR = ' ',
	STONE = '*',
	ORE = '#'
};

struct position {
	int x;
	int y;
	int z;
};

struct chunk {
	char *chunk_data;
	struct position *pos;
};

struct chunk *init_chunks(void);
void deinit_chunks(struct chunk *chunks);

void get_chunks(struct chunk *chunks, vec3 a);
void gen_chunk(struct chunk *chunk);
char chunk_gen_logic(struct position *chunk_pos, struct position *local_pos);

void save_chunk(char *chunk_data, char *file_name);
void load_chunk(char *chunk_data, FILE *file);

char *get_chunk_name(struct position *chunk_pos);

#endif
