#ifndef __UTILS_H__
#define __UTILS_H__

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

int multi_fork(const int count);
void check_status(const int s, char* message);
char* get_abs_file_path(char* const root_path, char* const file_path);
char* get_date_str();
void set_header_line(char* const response, char* const val, char* const key);
const char* get_filename_ext(const char* filename);
void urldecode(char* dst, const char* src);

#endif