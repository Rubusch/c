// test.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIN_LEN 7

int get_read_file_pointer(FILE **fp, char filename[FILENAME_MAX]);
int read_char(FILE *fp, char **content, unsigned long int *content_size);
int get_more_space(char **str, unsigned long int *str_size,
		   const unsigned long int how_much_more);
int close_stream(FILE **fp);

int get_read_file_pointer(FILE **fp, char filename[FILENAME_MAX])
{
	*fp = NULL;
	if (filename == NULL)
		return -1;
	if ((*fp = fopen(filename, "r")) == NULL) {
		perror("fo::get_read_file_pointer(FILE**, char[] - Failed!\n");
		return -1;
	}
	return 0;
};

int read_char(FILE *fp, char **content, unsigned long int *content_size)
{
	if (fp == NULL)
		return -1;
	if (*content_size == 0)
		return -1;
	int c = 0;
	unsigned long int idx = 0;
	const unsigned long int INITIAL_SIZE = *content_size;

	// if pTemp full, append and allocate new space
	while ((c = fgetc(fp)) != EOF) {
		putchar(c); // print out on screen
		(*content)[idx] = c;
		if (idx >= (*content_size) - 2) {
			if (get_more_space(content, content_size,
					   INITIAL_SIZE) == -1) {
				fprintf(stderr,
					"fo::read_char(*fp, char**, unsigned long int*) - Failed!\n");
				exit(EXIT_FAILURE);
			}
		}
		++idx;
	}
	(*content)[idx] = '\0';
	return 0;
};

int get_more_space(char **str, unsigned long int *str_size,
		   const unsigned long int how_much_more)
{
	char *tmp = NULL;
	if ((tmp = realloc(*str, (size_t)(*str_size + how_much_more))) ==
	    NULL) {
		if (tmp != NULL)
			free(tmp);
		tmp = NULL;
		return -1;
	}
	*str = tmp;
	*str_size += how_much_more;
	return 0;
};

int close_stream(FILE **fp)
{
	if (fp == NULL)
		return -1;
	int iRes = fclose(*fp);
	*fp = NULL;
	return iRes;
};

int main()
{
	// init
	FILE *fp = NULL;
	char file2[] = "file2.txt";
	unsigned long int content_size = LIN_LEN;
	char *content = malloc(content_size * sizeof(char));
	strcpy(content, "");

	printf("%i - Get read file pointer to %s\n",
	       get_read_file_pointer(&fp, file2), file2);
	printf("%i - Reading file characterwise\n",
	       read_char(fp, &content, &content_size));
	printf("content:\n\'%s\', %ld\n", content, content_size);
	strcpy(content, "");
	printf("%i - Close stream\n", close_stream(&fp));
	printf("Done.\n\n");
	free(content);

	return EXIT_SUCCESS;
}
