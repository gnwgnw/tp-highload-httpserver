#include "async_buffer.h"

void ab_alloc(struct async_buffer* const buff, const size_t size) {
	buff->buff = calloc(size, sizeof(char));
	buff->cur_write_pos = 0;
	buff->cur_read_pos = 0;
	buff->length = size;
}

void ab_free(struct async_buffer* const buff) {
	free(buff->buff);
	buff->buff = NULL;
	buff->cur_write_pos = 0;
	buff->length = 0;
}

void ab_write(struct async_buffer* const buffer, char* const src_buf, const size_t size) {
	if (size > (buffer->length - buffer->cur_write_pos - 1)) {
		ab_increase(buffer, size);
	}
	memcpy(buffer->buff + buffer->cur_write_pos, src_buf, size);
	buffer->cur_write_pos += size;
}

void ab_increase(struct async_buffer* const buffer, const size_t size) {
	int delta = 2;
	char* temp = buffer->buff;

	buffer->buff = calloc(buffer->length * delta + size, sizeof(char));
	memcpy(buffer->buff, temp, buffer->length);

	buffer->length *= delta;
	buffer->length += size;

	free(temp);
}