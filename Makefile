all:
	gcc -I include -O3 -std=gnu99 src/* -lev -o httpd

clean:
	rm httpd