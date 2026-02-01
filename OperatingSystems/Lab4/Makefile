KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

.PHONY: clean load mount uload umount test

obj-m += osfs.o

osfs-objs := super.o inode.o file.o dir.o osfs_init.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean


load:
	@sudo insmod osfs.ko
	@sudo dmesg

mount:
	@sudo mount -t osfs none mnt/
	@sudo dmesg

uload:
	@sudo rmmod osfs.ko
	@sudo dmesg

umount:
	@sudo umount mnt/


test:
	sudo touch ./mnt/test1.txt
	sudo bash -c "echo 'I LOVE OSLAB' > ./mnt/test1.txt"
	cat ./mnt/test1.txt
