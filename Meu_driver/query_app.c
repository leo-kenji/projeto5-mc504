#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#include "query_ioctl.h"
 
void get_vars(int fd)
{
    query_arg_t q;
 
    if (ioctl(fd, QUERY_GET_VARIABLES, &q) == -1)
    {
        perror("query_apps ioctl get");
    }
    else
    {
        printf("Status : %d\n", q.status);
        printf("Dignity: %d\n", q.dignity);
        printf("Ego    : %d\n", q.ego);
    }
}
void clr_vars(int fd)
{
    if (ioctl(fd, QUERY_CLR_VARIABLES) == -1)
    {
        perror("query_apps ioctl clr");
    }
}
void set_vars(int fd)
{
    int v;
    query_arg_t q;
 
    printf("Enter Status: ");
    scanf("%d", &v);
    getchar();
    q.status = v;
    printf("Enter Dignity: ");
    scanf("%d", &v);
    getchar();
    q.dignity = v;
    printf("Enter Ego: ");
    scanf("%d", &v);
    getchar();
    q.ego = v;
 
    if (ioctl(fd, QUERY_SET_VARIABLES, &q) == -1)
    {
        perror("query_apps ioctl set");
    }
}
 
void set_texto(int fd)
{

    texto_t q;
    char *t = malloc(100);
 
    // printf("Enter text: ");
    // scanf("%s", t);
    // // getchar();
    // q.texto = t;
    char texto[] = "Bom dia!";
    q.texto = texto;
    printf("Dentro do app: %s\n",q.texto);
    
    if (ioctl(fd, SET_TEXTO, &q) == -1)
    {
        perror("query_apps ioctl set texto");
    }
}


void get_texto(int fd)
{
    texto_t q;
    q.texto = malloc(256);

    
    if (ioctl(fd, READ_TEXTO, &q) == -1)
    {
        perror("query_apps ioctl set texto");
    }
    printf("%s",q.texto);
}

int main(int argc, char *argv[])
{
    char *file_name = "/dev/query";
    int fd;
    enum
    {
        e_get,
        e_clr,
        e_set,
        e_get_t,
        e_set_t
    } option;
 
    if (argc == 1)
    {
        option = e_get;
    }
    else if (argc == 2)
    {
        if (strcmp(argv[1], "-g") == 0)
        {
            option = e_get;
        }
        else if (strcmp(argv[1], "-c") == 0)
        {
            option = e_clr;
        }
        else if (strcmp(argv[1], "-s") == 0)
        {
            option = e_set;
        }

        else if (strcmp(argv[1], "-gt") == 0)
        {
            option = e_get_t;
        }
        else if (strcmp(argv[1], "-st") == 0)
        {
            option = e_set_t;
        }
        else
        {
            fprintf(stderr, "Usage: %s [-g | -c | -s | -st]\n", argv[0]);
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Usage: %s [-g | -c | -s | -st]\n", argv[0]);
        return 1;
    }
    fd = open(file_name, O_RDWR);
    if (fd == -1)
    {
        perror("query_apps open");
        return 2;
    }
 
    switch (option)
    {
        case e_get:
            get_vars(fd);
            break;
        case e_clr:
            clr_vars(fd);
            break;
        case e_set:
            set_vars(fd);
            break;
            case e_get_t:
            get_texto(fd);
            break;
        case e_set_t:
            set_texto(fd);
            break;
        default:
            break;
    }
 
    close (fd);
 
    return 0;
}