#ifndef __CAESAR_CIPHER_H__
#define __CAESAR_CIPHER_H__
#include <linux/ioctl.h>

#define DRIVER_BUFFER 256

typedef struct {
	int offset;
	char texto[DRIVER_BUFFER];
} texto_t;

// Funções do ioctl com seus indentificadores
#define READ_TEXTO _IOR('a', 1, texto_t *)
#define SET_TEXTO _IOW('a', 2, texto_t *)

#endif //__CAESAR_CIPHER_H__