#include <ev.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>
#include <stdio.h>

#include "utils.h"
#include "server.h"

#define DEFAULT_ROOT "/home/gexogen"

char root_path[FILENAME_MAX] = DEFAULT_ROOT;
uint8_t chunk_size = 8;

void arg_parser(int argc, char** argv, uint8_t* addon_worker, uint16_t* port, uint8_t* backlog_size);
void start_message(uint8_t addon_worker, uint16_t port, uint8_t backlog_size, int sock);

int main(int argc, char** argv) {
	uint8_t addon_worker = 3;
	uint16_t port = 8080;
	uint8_t backlog_size = 128;

	arg_parser(argc, argv, &addon_worker, &port, &backlog_size);

	int status;
	int sock;
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

	start_message(addon_worker, port, backlog_size, sock);

	pid_t pid = multi_fork(addon_worker);

	struct ev_loop* loop = EV_DEFAULT;
	struct ev_io w_accept;
	struct ev_signal w_exit;

	if (pid) {
		ev_signal_init(&w_exit, exit_cb, SIGINT);   //at future - only in main process
		ev_signal_start(EV_A_ &w_exit);
	}

	ev_io_init(&w_accept, accept_cb, sock, EV_READ);
	ev_io_start(loop, &w_accept);

	ev_run(loop, 0);

	close(sock);
	printf("Stop server\nBye!\n");

	return 0;
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

