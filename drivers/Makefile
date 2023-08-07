obj-m += demo_driver.o
obj-m += hid_descriptor_remap.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

logs:
	sudo tail -f /var/log/messages

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean