#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#include "caesar_cipher.h"

void set_texto(int fd)
{
	texto_t q;
	printf("Digite o número para o shift:\n");
	scanf("%d",&q.offset);
	getchar();// Ignora o \n
	printf("Digite a string a ser cifrada:\n");
	scanf("%255[^\n]",q.texto);

	if (ioctl(fd, SET_TEXTO, &q) == -1) {
		perror("app ioctl set texto");
	}
}

void get_texto(int fd)
{
	texto_t q;
	printf("Digite o número para o shift:\n");
	scanf("%d",&q.offset);
	getchar();// Ignora o \n

	if (ioctl(fd, READ_TEXTO, &q) == -1) {
		perror("app ioctl set texto");
	}
	printf("%s\n", q.texto);
}

int main(int argc, char *argv[])
{
	char *file_name = "/dev/caesar_cipher";
	int fd;
	enum { e_get_t, e_set_t } option;

	if (argc == 1) {
		option = e_get_t;
	} else if (argc == 2) {
		if (strcmp(argv[1], "-gt") == 0) {
			option = e_get_t;
		} else if (strcmp(argv[1], "-st") == 0) {
			option = e_set_t;
		} else {
			fprintf(stderr, "Usage: %s [-gt | -st]\n", argv[0]);
			return 1;
		}
	} else {
		fprintf(stderr, "Usage: %s [-gt | -st]\n", argv[0]);
		return 1;
	}
	fd = open(file_name, O_RDWR);
	if (fd == -1) {
		perror("app open");
		return 2;
	}

	switch (option) {
	case e_get_t:
		get_texto(fd);
		break;
	case e_set_t:
		set_texto(fd);
		break;
	default:
		break;
	}

	close(fd);

	return 0;
}