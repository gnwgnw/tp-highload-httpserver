#ifndef __ASYNC_BUFFER_H__
#define __ASYNC_BUFFER_H__

#include <stddef.h>
#include <malloc.h>
#include <string.h>

#define ASYNC_BUFFER_SIZE 1024

struct async_buffer {
	char* buff;
	size_t length;
	size_t cur_write_pos;
	size_t cur_read_pos;
};

void ab_alloc(struct async_buffer* const buff, const size_t size);
void ab_free(struct async_buffer* const buff);
void ab_write(struct async_buffer* const buffer, char* const src_buf, const size_t size);
void ab_increase(struct async_buffer* const buffer, const size_t size);

#endif