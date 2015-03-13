all:
	gcc -I include -O3 -std=gnu99 src/main.c src/async_buffer.c src/server.c src/utils.c -lev -o httpd

clean:
	rm httpd