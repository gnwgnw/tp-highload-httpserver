#include "utils.h"

pid_t create_workers(const int count) {
	pid_t pid = 1;
	for (int i = 0; i < count; ++i) {
		pid = fork();
		if (!pid) {
			break;
		}
	}
	return pid;
}

void check_status(const int s, char* message) {
	if (s == -1) {
		printf("%s error. Exit!\n", message);
		exit(1);
	}
}

char* get_abs_file_path(char* const root_path, char* const file_path) {
	size_t root_len = strlen(root_path);

	char* abs_file_path = calloc(root_len + strlen(file_path) + 1, sizeof(char));
	strcpy(abs_file_path, root_path);
	strcpy(abs_file_path + root_len, file_path);
	return abs_file_path;
}

char* get_date_str() {
	time_t now = time(0);
	struct tm* now_tm = gmtime(&now);
	char* temp = asctime(now_tm);
	temp[24] = 0;   // '\n' -> '\0'
	return temp;
}

const char* get_filename_ext(const char* filename) {
	const char* dot = strrchr(filename, '.');
	if (!dot || dot == filename)
		return "";
	return dot + 1;
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

void start_message(uint8_t addon_worker, uint16_t port, uint8_t backlog_size, int sock) {
	printf("Start server\n"
		"Port: %d\n"
		"Socket: %d\n"
		"Dir root: %s\n"
		"Chunck size: %d\n"
		"Addon workers: %d\n"
		"Backlog size: %d\n",
		port, sock, root_path, chunk_size, addon_worker, backlog_size);
}

void arg_parser(int argc, char** argv, uint8_t* addon_worker, uint16_t* port, uint8_t* backlog_size) {
	int c;
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

void get_url_path(char** url) {
	char* temp = *url;
	while (*temp) {
			if (*temp == '?'){
				*temp = '\0';
				break;
			}
			++temp;
		}
}

void create_socket(int* sock, uint16_t port, uint16_t backlog_size) {
	int status;
	struct sockaddr_in sockaddr;

	*sock = socket(AF_INET, SOCK_STREAM, 0);
	check_status(*sock, "Socket");

	bzero(&sockaddr, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);
	sockaddr.sin_addr.s_addr = htons(INADDR_ANY);

	status = bind(*sock, (struct sockaddr const*) &sockaddr, (socklen_t) sizeof(sockaddr));
	check_status(status, "Bind");

	status = listen(*sock, backlog_size);
	check_status(status, "Listen");

	return;
}