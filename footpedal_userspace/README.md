## userspace foot pedal detector

Will detect when this foot pedal is pressed/released, and output to `/dev/shm/footpedal`!

Note that you need root permissions for this program to work. 

Monitors the raw hex output from this pedal in sysfs/devfs (`/dev/hidraw0` for me, but this program should autodetect the right one), and then parses that data to detect press/release events. Then, it writes to the kernel's shared memory RAM disk (`/dev/shm`) for other programs to be able to read the pedal's current state without needing root permissions. 0 is not pressed, 1 is pressed. 

This file also contains a shell script that we use in order to get the hid device number for the pedal. 

Limitations: you can only have one foot pedal plugged in. Also, no other devices should have the name `FootSwitch` in them. 