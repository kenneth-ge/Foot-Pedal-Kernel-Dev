# Foot-Pedal-Kernel-Dev
## footpedal_userspace
A userspace program that reads data from the kernel (devfs), and outputs the current state of the pedal

## drivers
Drivers I'm writing, some of which control/manipulate the foot pedal

### demo_driver.c
Basic character driver program template that allows reading and writing from its character file in sysfs. Stores 64 bytes of data (think of it like a USB flash drive with a storage space of 64 bytes). 

### hid_descriptor_remap.c
Hacks the HID descriptor for the PCSensor FootSwitch by shifting its key mapping up to 0xC2, in order to remap the 'b' key to 'KEYPAD XOR', an unused key. This allows us to read the raw HID info to detect pedal presses using demo_driver.c
