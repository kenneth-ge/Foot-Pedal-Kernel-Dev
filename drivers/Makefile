obj-m += demo_driver.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

run: all
	sudo insmod ./hello-1.ko

logs:
	sudo tail -f /var/log/messages

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean