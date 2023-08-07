obj-m += demo_driver.o
obj-m += hid_descriptor_remap.o

ccflags-y := -std=gnu99 -Wno-declaration-after-statement

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

logs:
	dmesg -w

test-demo: all
	sudo rmmod demo_driver.ko
	sudo insmod demo_driver.ko
	sudo cat /dev/demo-0
	echo "some data, data dee dee dee data" | sudo tee /dev/demo-0
	sudo cat /dev/demo-0

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean