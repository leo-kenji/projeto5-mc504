#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>

int char_to_int(char c)
{
    if (c >= '0' && c <= '9')
        return (int)(c - '0');
    return -1;
}

int get_int()
{
    int i;
    do
    {
        char c;
        scanf("%c", &c);
        i = char_to_int(c);
    } while (i == -1);
    return i;
}

int main()
{
    char c = 'a';
    int i;
    do
    {
        printf("What you want to do?\ns - set\ng - get\nq - quit\n");
        scanf("%c", &c);
        if(c == '\n') scanf("%c", &c);

        // get
        if (c == 'g')
        {
            printf("get weight from which user? (int)uid: ");
            scanf("%d", &i);
            printf("\n");
            int ret = syscall(548, i);
            printf("retorno da syscall: %d\n", ret);
            if (errno)
            {
                perror("Saida do perror");
                errno = 0;
            }
            printf("\n");
        }
        // set
        if (c == 's')
        {
            printf("set weight of which user? (int)uid (int)weight: ");
            int uid, weight;
            scanf("%d %d", &uid, &weight);
            printf("\n");

            int ret = syscall(549, uid, weight);
            printf("retorno da syscall: %d\n", ret);
            if (errno)
            {
                perror("Saida do perror");
                errno = 0;
            }
            printf("\n");
        }
    } while (c != 'q');

    return 0;
}