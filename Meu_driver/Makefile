MY_KERNEL_ROOT=../

obj-m   +=  hello.o query_ioctl.o

all:
		make -C $(MY_KERNEL_ROOT) M=$(PWD) modules

clean:
		make -C $(MY_KERNEL_ROOT) M=$(PWD) clean
