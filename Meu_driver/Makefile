MY_KERNEL_ROOT=../

obj-m   +=  caesar_cipher.o

all:
		make -C $(MY_KERNEL_ROOT) M=$(PWD) modules

clean:
		make -C $(MY_KERNEL_ROOT) M=$(PWD) clean
