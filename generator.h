#ifndef GENERATOR_H
#define GENERATOR_H

extern const int CHUNK_SIZE;
extern const int RENDER_DISTANCE;
extern const int CHUNK_NAME_LEN;

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

void get_chunks(struct chunk *chunks, struct position *player_pos);
void gen_chunk(struct chunk *chunk);
char chunk_gen_logic(struct position *chunk_pos, struct position *local_pos);

void save_chunk(char *chunk_data, char * file_name);
void load_chunk(char *chunk_data, char *file_name);

char *get_chunk_name(struct position *chunk_pos);
char *num2str(char *str, int num);

#endif
