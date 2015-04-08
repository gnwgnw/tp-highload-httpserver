#ifndef __UTILS_H__
#define __UTILS_H__

#ifdef DEBUG
#define debug(M, ...) fprintf(stderr, "DEBUG " M "\n", ##__VA_ARGS__)
#else
#define debug(M, ...)
#endif

#define logging(M, ...) printf(M "\n", ##__VA_ARGS__)

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <netinet/in.h>

extern char root_path[];
extern uint8_t chunk_size;

void check_status(int s, const char* message);
int create_workers(int count);
void start_message(uint8_t addon_worker, uint16_t port, uint16_t backlog_size, int sock);
void arg_parser(int argc, char** argv, uint8_t* addon_worker, uint16_t* port, uint16_t* backlog_size);
int create_socket(uint16_t port, uint16_t backlog_size);

char* get_abs_file_path(const char* root_path, const char* file_path);
const char* get_filename_ext(const char* filename);
void change_url_to_urlpath(char* url);
void urldecode(char* dst, const char* src);
char* get_date_str();

#endif