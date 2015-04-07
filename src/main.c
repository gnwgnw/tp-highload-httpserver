#include <ev.h>

#include "utils.h"
#include "server.h"

#define DEFAULT_ROOT "/home"

char root_path[FILENAME_MAX] = DEFAULT_ROOT;
uint8_t chunk_size = 8;
uint8_t addon_worker = 3;
uint16_t port = 8080;
uint16_t backlog_size = 128;

int main(int argc, char** argv) {
	arg_parser(argc, argv, &addon_worker, &port, &backlog_size);

	int sock;
	create_socket(&sock, port, backlog_size);

	start_message(addon_worker, port, backlog_size, sock);

	pid_t pid = create_workers(addon_worker);

	struct ev_loop* loop = EV_DEFAULT;
	struct ev_io w_accept;
	struct ev_signal w_exit;

	if (pid) {
		ev_signal_init(&w_exit, exit_cb, SIGINT);   //only in main process
		ev_signal_start(EV_A_ &w_exit);
	}

	ev_io_init(&w_accept, accept_cb, sock, EV_READ);
	ev_io_start(loop, &w_accept);

	ev_run(loop, 0);

	close(sock);
	printf("Stop server\nBye!\n");

	return 0;
}