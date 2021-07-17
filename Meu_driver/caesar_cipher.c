// According to all known laws of aviation, there is no way that a bee should be able to fly. Its wings are too small to get its fat little body off the ground. The bee, of course, flies anyways. Because bees don't care what humans think is impossible.

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include "cifra.c"

#include "caesar_cipher.h"

// For NULL
#include <linux/types.h>

#define FIRST_MINOR 0
#define MINOR_CNT 1

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;
static texto_t t;

/** 
* Faz parte da interface que um character device driver deve implmentar
*/
static ssize_t cipher_read(struct file *file, char __user *buf, size_t size,
			   loff_t *ppos)
{
	// Função que chama o wrapper do copy_from_user
	// Nesse caso está retornando a string cifrada
	return simple_read_from_buffer(buf, size, ppos, t.texto,
				       strlen(t.texto));
}

/** 
* Faz parte da interface que um character device driver deve implmentar
*
*/
static ssize_t cipher_write(struct file *file, const char __user *buf,
			    size_t size, loff_t *ppos)
{
	int ret;
	char value[256];
	// Recebe o input do userspace
	if ((ret = strncpy_from_user(value, buf, strlen(buf))) == -EFAULT) {
		return ret;
	}
	if (sscanf(value, "%d %255[^\n]", &t.offset, t.texto) != 2) {
		// Não consegui encontrar um número e uma string
		return -EINVAL;
	}
	cifrar(t.texto, t.offset);

	return ret;
}

/**
* Implementação da interface ioctl para o driver
*/
static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case SET_TEXTO:
		// Copia os valores do userspace que usuario envia
		if (copy_from_user(&t, (texto_t *)arg, sizeof(texto_t))) {
			return -EACCES;
		}
		//cifra a string e a armazena
		cifrar(t.texto, t.offset);
		break;

	//desencriptando
	case READ_TEXTO:
		texto_t temp;

		// Copia os valores do userspace que usuario envia
		// if (copy_from_user(&temp, (texto_t *)arg, sizeof(texto_t))) {
		// 	return -EACCES;
		// }
		temp.offset = ((texto_t *)arg)->offset;

		strcpy(temp.texto, t.texto);

		//decifra a string armazenada com o número enviado pelo usuário
		decifrar(temp.texto, temp.offset);

		//envia
		if (copy_to_user((texto_t *)arg, &temp, sizeof(texto_t))) {
			return -EACCES;
		}
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

/**
* Operacoes que o device driver ira realizar
*/
static struct file_operations cipher_fops = { .owner = THIS_MODULE,
					      .read = cipher_read,
					      .write = cipher_write,
					      .unlocked_ioctl = my_ioctl };

/**
* Inicializa o driver
*/
static int __init caesar_cipher_init(void)
{
	int ret;
	struct device *dev_ret;

	// Aloca o device number dinâmicamente
	// pede apenas 1 minor number começando em FIRST_MINOR = 0
	if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT,
				       "caesar_cipher")) < 0) {
		return ret;
	}

	// Inicializa a estrutura do char device
	cdev_init(&c_dev, &cipher_fops);

	// Registra o device para o seu device number
	if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0) {
		return ret;
	}

	// Faz o "mesmo" que mknod, cria um device node em /dev, mas automáticamente
	if (IS_ERR(cl = class_create(THIS_MODULE, "char"))) {
		cdev_del(&c_dev);
		unregister_chrdev_region(dev, MINOR_CNT);
		return PTR_ERR(cl);
	}
	if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL,
					   "caesar_cipher"))) {
		class_destroy(cl);
		cdev_del(&c_dev);
		unregister_chrdev_region(dev, MINOR_CNT);
		return PTR_ERR(dev_ret);
	}

	return 0;
}

/**
* Destroi o device driver
*/
static void __exit caesar_cipher_exit(void)
{
	device_destroy(cl, dev);
	class_destroy(cl);
	cdev_del(&c_dev);
	unregister_chrdev_region(dev, MINOR_CNT);
}

module_init(caesar_cipher_init);
module_exit(caesar_cipher_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Caesar cipher() Char Driver");