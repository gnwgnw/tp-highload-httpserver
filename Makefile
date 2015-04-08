all:
	gcc -I include -O3 -std=gnu99 src/*.c -lev -o httpd

debug:
	gcc -I include -D DEBUG -g -std=gnu99 src/*.c -lev -o httpd
clean:
	rm httpd