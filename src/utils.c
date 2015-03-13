#include "utils.h"

pid_t multi_fork(const int count) {
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
	*dst++ = '\0';
}