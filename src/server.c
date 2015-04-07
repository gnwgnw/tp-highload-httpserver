#include "server.h"

void response(EV_P_ ev_io* w, struct async_buffer* ab) {
	char response_header[MAX_RESPONSE_HEAD] = "";
	short status = S_200;
	char* method = NULL;
	char* url = NULL;
	short only_header = 0;
	size_t file_size = 0;

	char* request = calloc(ab->length, sizeof(char));
	strcpy(request, ab->buff);

	check_request(request, &method, &url, &only_header, &status);
	check_url(&url, &file_size, &status);
	check_access(url, &status);

	set_header(response_header, status, url, file_size);

	write_header_to_ab(ab, response_header);
	write_file_to_ab(ab, url, only_header, file_size, status);

	free(request);
	if (url)
		free(url);

	ev_io_stop(loop, w);
	ev_io_init(w, write_cb, w->fd, EV_WRITE);
	ev_io_start(EV_A_ w);

	return;
}

void write_file_to_ab(struct async_buffer* ab, char* url, short only_header, size_t file_size, short status) {
	if (status == S_200 && !only_header) {
		FILE* file = fopen(url, "r");

		ab_increase(ab, file_size);

		size_t read_bytes = fread(ab->buff + ab->cur_write_pos, sizeof(char), file_size, file);
		ab->cur_write_pos += read_bytes;

		fclose(file);
	}
}

void write_header_to_ab(struct async_buffer* ab, char* response_header) {
	ab_free(ab);
	ab_alloc(ab, ASYNC_BUFFER_SIZE);

	ab_write(ab, response_header, strlen(response_header));
}

void set_header(char* response_header, short status, char* url, size_t file_size) {
	switch (status) {
		case S_200:
			set_header_line(response_header, HTTP1_1, HTTP_200);
	        break;
		case S_400:
			set_header_line(response_header, HTTP1_1, HTTP_400);
	        break;
		case S_403:
			set_header_line(response_header, HTTP1_1, HTTP_403);
	        break;
		case S_404:
			set_header_line(response_header, HTTP1_1, HTTP_404);
	        break;
		case S_405:
			set_header_line(response_header, HTTP1_1, HTTP_405);
	        break;
		default:
			break;
	}

	set_header_line(response_header, "Server:", "httpd");
	set_header_line(response_header, "Date:", get_date_str());
	set_header_line(response_header, "Connection:", "close");

	if (status == S_200) {
		char temp[20] = "";
		sprintf(temp, "%li", file_size);
		set_header_line(response_header, "Content-Length:", temp);

		set_header_line(response_header, "Content-Type:", get_content_type(get_filename_ext(url)));
	}

	strcat(response_header, HTTP_CRLF);
}

char* const get_content_type(char const* ext) {
	if (!strcasecmp(ext, "html") || !strcasecmp(ext, "htm")) {
		return MT_HTML;
	}
	else if (!strcasecmp(ext, "css")) {
		return MT_CSS;
	}
	else if (!strcasecmp(ext, "gif")) {
		return MT_GIF;
	}
	else if (!strcasecmp(ext, "png")) {
		return MT_PNG;
	}
	else if (!strcasecmp(ext, "jpg") || !strcasecmp(ext, "jpe") || !strcasecmp(ext, "jpeg")) {
		return MT_JPG;
	}
	else if (!strcasecmp(ext, "tiff") || !strcasecmp(ext, "tif")) {
		return MT_TIFF;
	}
	else if (!strcasecmp(ext, "bmp")) {
		return MT_BMP;
	}
	else if (!strcasecmp(ext, "js")) {
		return MT_JS;
	}
	else if (!strcasecmp(ext, "swf")) {
		return MT_SWF;
	}
	return MT_PLAIN;
}

void check_access(char* url, short* status) {
	if ((*status) == S_200 && access(url, R_OK) == -1) {
		(*status) = S_403;
	}
}

void check_url(char** url, size_t* file_size, short* status) {
	if ((*status) == S_200) {
		struct stat file_stat;

		urldecode(*url, *url);
		get_url_path(url);
		(*url) = get_abs_file_path(root_path, *url);

		if (!stat((*url), &file_stat)) {
			if (S_ISDIR(file_stat.st_mode)) {
				char* temp = get_abs_file_path((*url), "/index.html");
				free((*url));
				(*url) = temp;

				if (stat((*url), &file_stat)) {
					(*status) = S_403;
				}
			}
			(*file_size) = (size_t) file_stat.st_size;
		}
		else {
			(*status) = S_404;
		}
	}
}

void check_request(char* request, char** method, char** url, short* only_head, short* status) {
	char* token = strtok(request, "\r\n");
	if (token) {
		(*method) = strtok(token, " ");
		(*url) = strtok(NULL, " ");

		if ((*method) && (*url) && !strstr(*url, "..")) {
			if (strcmp((*method), "GET")) {
				if (strcmp((*method), "HEAD")) {
					(*status) = S_405;
					(*url) = NULL;
				}
				else {
					(*only_head) = 1;
				}
			}
		}
		else {
			(*status) = S_400;
			(*url) = NULL;
		}
	}
	else {
		(*status) = S_400;
	}
}

void close_connection(EV_P_ ev_io* w) {
	ev_io_stop(loop, w);

	close(w->fd);

	ab_free(w->data);
	free(w->data);
	free(w);

	return;
}

char* check_request_end(struct async_buffer* ab) {
	return strstr(ab->buff, "\r\n\r\n");
}

void set_header_line(char* const response, char* const val, char* const key) {
	strcat(response, val);
	strcat(response, HTTP_SP);
	strcat(response, key);
	strcat(response, HTTP_CRLF);
}

void accept_cb(EV_P_ ev_io* w, int revents) {
	if (revents & EV_ERROR)
		return;

	int client_sock;

	client_sock = accept(w->fd, NULL, NULL);

	struct async_buffer* data = malloc(sizeof(struct async_buffer));
	bzero(data, sizeof(data));
	ab_alloc(data, ASYNC_BUFFER_SIZE);

	struct ev_io* w_client_read = malloc(sizeof(struct ev_io));
	bzero(w_client_read, sizeof(w_client_read));

	w_client_read->data = data;

	ev_io_init(w_client_read, read_cb, client_sock, EV_READ);
	ev_io_start(EV_A_ w_client_read);
}

void read_cb(EV_P_ ev_io* w, int revents) {
	if (revents & EV_ERROR)
		return;

	struct async_buffer* ab = (struct async_buffer*) w->data;
	char chunk[chunk_size];

	ssize_t read_bytes = recv(w->fd, chunk, chunk_size, 0);
	if (read_bytes > 0) {
		ab_write(ab, chunk, (size_t) read_bytes);

		if (check_request_end(ab)) {
			response(EV_A_ w, ab);
		}
	}
	else {
		close_connection(EV_A_ w);
	}
}

void write_cb(EV_P_ ev_io* w, int revents) {
	if (revents & EV_ERROR)
		return;

	struct async_buffer* ab = (struct async_buffer*) w->data;
	size_t byte_to_send = ab->cur_write_pos - ab->cur_read_pos;

	ssize_t sended_bytes = send(w->fd, ab->buff + ab->cur_read_pos, byte_to_send, 0);
	if (sended_bytes > 0) {
		ab->cur_read_pos += sended_bytes;

		if (byte_to_send == sended_bytes) {
			close_connection(EV_A_ w);
		}
	}
}

void exit_cb(EV_P_ ev_signal* w, int revents) {
	ev_break(EV_A_ EVBREAK_ALL);
	printf("Stop signal recived\n");
}