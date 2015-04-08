#include "utils.h"

void check_status(int s, const char* message) {
	if (s == -1) {
		logging("%s %s", message, "error. Exit!");
		exit(1);
	}
}

pid_t create_workers(int count) {
	pid_t pid = 1;
	logging("Start worker 1");
	for (int i = 0; i < count; ++i) {
		pid = fork();
		check_status(pid, "Fork");
		if (!pid) {
			logging("Start worker %d", i + 2);
			break;
		}
	}
	return pid;
}

void start_message(uint8_t addon_worker, uint16_t port, uint16_t backlog_size, int sock) {
	logging("Start server");
	logging("Port: %d", port);
	logging("Socket: %d", sock);
	logging("Dir root: %s", root_path);
	logging("Chunck size: %d", chunk_size);
	logging("Addon workers: %d", addon_worker);
	logging("Backlog size: %d", backlog_size);
}

void arg_parser(int argc, char** argv, uint8_t* addon_worker, uint16_t* port, uint16_t* backlog_size) {
	debug("arg parser");
	int c = 0;
	while ((c = getopt(argc, argv, "C:r:c:p:b:")) != -1)
		switch (c) {
			case 'C':
				chunk_size = (uint8_t) atoi(optarg);
		        break;
			case 'r':
				strcpy(root_path, optarg);
		        break;
			case 'c':
				(*addon_worker) = (uint8_t) (atoi(optarg) - 1);
		        break;
			case 'p':
				(*port) = (uint16_t) atoi(optarg);
		        break;
			case 'b':
				(*backlog_size) = (uint8_t) atoi(optarg);
		        break;
			default:
				exit(1);
		}
}

int create_socket(uint16_t port, uint16_t backlog_size) {
	int status;
	int sock = 0;
	struct sockaddr_in sockaddr;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	check_status(sock, "Socket");

	bzero(&sockaddr, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);
	sockaddr.sin_addr.s_addr = htons(INADDR_ANY);

	status = bind(sock, (struct sockaddr const*) &sockaddr, (socklen_t) sizeof(sockaddr));
	check_status(status, "Bind");

	status = listen(sock, backlog_size);
	check_status(status, "Listen");

	return sock;
}

char* get_abs_file_path(const char* root_path, const char* file_path) {
	size_t root_len = strlen(root_path);

	char* abs_file_path = calloc(root_len + strlen(file_path) + 1, sizeof(char));
	strcpy(abs_file_path, root_path);
	strcpy(abs_file_path + root_len, file_path);
	return abs_file_path;
}

const char* get_filename_ext(const char* filename) {
	const char* dot = strrchr(filename, '.');
	if (!dot || dot == filename)
		return "";
	return dot + 1;
}

void change_url_to_urlpath(char* url) {
	while (*url) {
		if (*url == '?') {
			*url = '\0';
			break;
		}
		++url;
	}
}

void urldecode(char* dst, const char* src) {
	char a, b;
	while (*src) {
		if ((*src == '%') && ((a = src[1]) && (b = src[2])) && (isxdigit(a) && isxdigit(b))) {
			if (a >= 'a')
				a -= 'a' - 'A';
			if (a >= 'A')
				a -= ('A' - 10);
			else
				a -= '0';
			if (b >= 'a')
				b -= 'a' - 'A';
			if (b >= 'A')
				b -= ('A' - 10);
			else
				b -= '0';
			*dst++ = (char) (16 * a + b);
			src += 3;
		}
		else {
			*dst++ = *src++;
		}
	}
	*dst = '\0';
}

char* get_date_str() {
	time_t now = time(0);
	struct tm* now_tm = gmtime(&now);
	char* temp = asctime(now_tm);
	temp[24] = 0;   // '\n' -> '\0'
	return temp;
}
