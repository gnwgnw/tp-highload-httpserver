#ifndef __SERVER_H__
#define __SERVER_H__

#include <ev.h>
#include <netinet/in.h>
#include <malloc.h>
#include <strings.h>

#include "async_buffer.h"
#include "utils.h"
#include "mime_types.h"

#define MAX_RESPONSE_HEAD 256

#define HTTP_CRLF "\r\n"
#define HTTP_SP " "
#define HTTP1_1 "HTTP/1.1"
#define HTTP_200 "200 OK"
#define HTTP_400 "400 Bad request"
#define HTTP_403 "403 Forbidden"
#define HTTP_404 "404 Not Found"
#define HTTP_405 "405 Method Not Allowed"

#define S_200 1
#define S_400 2
#define S_403 3
#define S_404 4
#define S_405 5

extern char root_path[];
extern uint8_t chunk_size;

void response(EV_P_ ev_io* w, struct async_buffer* ab);
void close_connection(EV_P_ ev_io* w);
char* check_request_end(struct async_buffer* ab);
void set_header_line(char* const response, char* const val, char* const key);

void accept_cb(EV_P_ ev_io* w, int revents);
void read_cb(EV_P_ ev_io* w, int revents);
void write_cb(EV_P_ ev_io* w, int revents);
void exit_cb(EV_P_ ev_signal* w, int revents);

void check_request(char* request, char** method, char** url, short* only_head, short* status);
void check_url(char** url, size_t* file_size, short* status);
void check_access(char* url, short* status);
void set_header(char* response_header, short status, char* url, size_t file_size);
void write_header_to_ab(struct async_buffer* ab, char* response_header);
void write_file_to_ab(struct async_buffer* ab, char* url, short only_header, size_t file_size, short status);
char* const get_content_type(char const* ext);

#endif