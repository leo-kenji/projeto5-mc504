#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
 
#include "query_ioctl.h"
 
// For NULL
#include <linux/types.h>

#include <linux/slab.h> // For krealloc
#include <linux/gfp.h> // For krealloc flag

#define FIRST_MINOR 0
#define MINOR_CNT 1
 
static dev_t dev;
static struct cdev c_dev;
static struct class *cl;
static int status = 1, dignity = 3, ego = 5;
static char* texto = NULL; 

static int my_open(struct inode *i, struct file *f)
{
    return 0;
}


static int my_close(struct inode *i, struct file *f)
{
    return 0;
}


static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    query_arg_t q;
    texto_t t;

    switch (cmd)
    {
        case QUERY_GET_VARIABLES:
            q.status = status;
            q.dignity = dignity;
            q.ego = ego;
            if (copy_to_user((query_arg_t *)arg, &q, sizeof(query_arg_t)))
            {
                return -EACCES;
            }
            break;
        case QUERY_CLR_VARIABLES:
            status = 0;
            dignity = 0;
            ego = 0;
            break;
        case QUERY_SET_VARIABLES:
            if (copy_from_user(&q, (query_arg_t *)arg, sizeof(query_arg_t)))
            {
                return -EACCES;
            }
            status = q.status;
            dignity = q.dignity;
            ego = q.ego;
            break;

        case SET_TEXTO:
            // Limite de 4MB para x86
            // TODO Fazer a verificação do tamanho
            int size_string = strlen(((texto_t *)arg)->texto)+1;
            texto = krealloc(texto,size_string*sizeof(char), GFP_KERNEL);
            // TODO verificar krealloc
            strncpy_from_user(texto, ((texto_t *)arg)->texto, size_string);
            // TODO verificar strncpy
            // TODO verificar adicionar terminador
            printk("Dentro do SET_TEXTO recebido: %s\n",((texto_t *)arg)->texto);


            // texto = krealloc(texto,256*sizeof(char), GFP_KERNEL);
            //  if (copy_from_user(&t, (texto_t *)arg, sizeof(texto_t)))
            // {
            //     return -EACCES;
            // }

            // if (copy_from_user(texto, t.texto, 256))
            // {
            //     return -EACCES;
            // }
            printk("Dentro do SET_TEXTO gravado: %s\n",texto);
            break;
            
        case READ_TEXTO:
            // t.texto = texto;
            printk("%s",texto);

            char* antigo =((texto_t *)arg)->texto;
            // if (copy_to_user((texto_t *)arg, &t, sizeof(texto_t)))
            // {
            //     return -EACCES;
            // }
            // if (copy_to_user(((texto_t *)arg)->texto, t.texto, strlen(t.texto)*sizeof(char)))
            if (copy_to_user(((texto_t *)arg)->texto, t.texto, 256))
            {
                return -EACCES;
            }


            break;

        default:
            return -EINVAL;
    }
 
    return 0;
}
 
static struct file_operations query_fops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .unlocked_ioctl = my_ioctl
};
 
static int __init query_ioctl_init(void)
{
    int ret;
    struct device *dev_ret;
 
 
    if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "query_ioctl")) < 0)
    {
        return ret;
    }
 
    cdev_init(&c_dev, &query_fops);
 
    if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)
    {
        return ret;
    }
     
    if (IS_ERR(cl = class_create(THIS_MODULE, "char")))
    {
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(cl);
    }
    if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "query")))
    {
        class_destroy(cl);
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(dev_ret);
    }
 
    return 0;
}
 
static void __exit query_ioctl_exit(void)
{
    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
}
 
module_init(query_ioctl_init);
module_exit(query_ioctl_exit);
 
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Query ioctl() Char Driver");