#ifndef GENERATOR_H
#define GENERATOR_H

#include <cglm/cglm.h>
#include <math.h>

enum {
	FLAT,
	CAVES,
	MENGERS_SPONGE,
	PILLARS,
	MOUNTAIN,
	SKY_GRID
};

enum {
	CHUNK_SIZE = 16,
	RENDER_DISTANCE = 3,
	CHUNK_NAME_LEN = 64,

	GENERATION_TYPE = SKY_GRID,
	PLATFORM_RADIUS = 5,

	CHUNKS_SIDE = 2 * RENDER_DISTANCE - 1,
	CHUNKS_AMOUNT = CHUNKS_SIDE * CHUNKS_SIDE * CHUNKS_SIDE,
	BLOCKS_IN_CHUNK = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE,
	BLOCKS_AMOUNT = BLOCKS_IN_CHUNK * CHUNKS_AMOUNT
};

enum BLOCKS {
	UNDEFINED_BLOCK,
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

struct position index2lpos(int index, const int SIDE);

struct chunk *init_chunks(void);
void deinit_chunks(struct chunk *chunks);

void get_chunks(struct chunk *chunks, vec3 a);
void gen_chunk(struct chunk *chunk);
char chunk_gen_logic(struct position *gpos);

int generate_flat(struct position *gpos);
int generate_caves(struct position *gpos);
int generate_mengers_sponge(struct position *gpos);
int generate_pillars(struct position *gpos);
int generate_mountain(struct position *gpos);
int generate_sky_grid(struct position *gpos);
int generate_platform(struct position *gpos);

void save_chunk(char *chunk_data, char *file_name);
void load_chunk(char *chunk_data, FILE *file);

char *get_chunk_name(struct position *chunk_pos);

struct position gpos2chunkpos(struct position *pos);
struct position gpos2lpos(struct position *global);
struct position lpos2gpos(struct position *lpos, struct position *chunk_pos);

struct position vec2pos(vec3 vec_pos);
void pos2vec(struct position *pos, vec3 vec_pos);

struct position index2lpos(int index, const int SIDE);
int lpos2index(struct position *pos, const int SIDE);

#endif
