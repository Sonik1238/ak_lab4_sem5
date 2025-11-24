<<<<<<< Updated upstream
obj-m+=hello.o
=======
ccflags-y := -I$(src)/inc -g

obj-m += hello1.o
obj-m += hello2.o

KDIR ?= /lib/modules/$(shell uname -r)/build

>>>>>>> Stashed changes
all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
