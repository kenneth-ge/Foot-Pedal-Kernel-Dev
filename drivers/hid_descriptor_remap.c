/*
 * Modified from https://www.marcusfolkesson.se/blog/hid-report-descriptors/
 * */

#include <linux/module.h>
#include <linux/init.h>

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/input.h>
#include <linux/hid.h>

#include <asm/irq.h>
#include <asm/io.h>

//parsed using https://eleccelerator.com/usbdescreqparser/ 
static __u8 pedal_rdesc_fixed[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x06,        // Usage (Keyboard)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x01,        //   Report ID (1)
    0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
    0x19, 0xE0,        //   Usage Minimum (0xE0)
    0x29, 0xE7,        //   Usage Maximum (0xE7)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x08,        //   Report Count (8)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x08,        //   Report Size (8)
    0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x03,        //   Report Count (3)
    0x75, 0x01,        //   Report Size (1)
    0x05, 0x08,        //   Usage Page (LEDs)
    0x19, 0x01,        //   Usage Minimum (Num Lock)
    0x29, 0x03,        //   Usage Maximum (Scroll Lock)
    0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x05,        //   Report Count (5)
    0x75, 0x01,        //   Report Size (1)
    0x91, 0x01,        //   Output (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x06,        //   Report Count (6)
    0x75, 0x08,        //   Report Size (8)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0xFF,        //   Logical Maximum (-1)
    0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
    // this is where the standard keys are mapped
    // if we shift the usage minimum by some amount X, 
    // the kernel will interpret scancodes as being 
    // shifted up by X
    // so, we shift the keycode up by 0x35 to remap the 'b'
    // to F1
    0x19, 0x35,        //   Usage Minimum (0x00)
    0x29, 0xFF,        //   Usage Maximum (0xFF)
    0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
    0x05, 0x0C,        // Usage Page (Consumer)
    0x09, 0x01,        // Usage (Consumer Control)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x03,        //   Report ID (3)
    0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
    0x09, 0x81,        //   Usage (Sys Power Down)
    0x09, 0x82,        //   Usage (Sys Sleep)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x02,        //   Report Count (2)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x06,        //   Report Count (6)
    0x75, 0x01,        //   Report Size (1)
    0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x0C,        //   Usage Page (Consumer)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x10,        //   Report Size (16)
    0x19, 0x00,        //   Usage Minimum (Unassigned)
    0x2A, 0x2E, 0x02,  //   Usage Maximum (AC Zoom Out)
    0x26, 0x2E, 0x02,  //   Logical Maximum (558)
    0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x02,        // Usage (Mouse)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x02,        //   Report ID (2)
    0x09, 0x01,        //   Usage (Pointer)
    0xA1, 0x00,        //   Collection (Physical)
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (0x01)
    0x29, 0x05,        //     Usage Maximum (0x05)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x95, 0x05,        //     Report Count (5)
    0x75, 0x01,        //     Report Size (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x03,        //     Report Size (3)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,        //     Usage (X)
    0x09, 0x31,        //     Usage (Y)
    0x09, 0x38,        //     Usage (Wheel)
    0x15, 0x81,        //     Logical Minimum (-127)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x03,        //     Report Count (3)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xC0,              // End Collection
    // 162 bytes
};

static __u8 *pedal_report_fixup(struct hid_device *hdev, __u8 *rdesc,
                unsigned int *rsize)
{
    printk(KERN_INFO "Changing driver\n");
    hid_info(hdev, "Editing FootSwitch report descriptor\n");
    *rsize = sizeof(pedal_rdesc_fixed);
    return pedal_rdesc_fixed;
}

static int pedal_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
    int ret;

    printk(KERN_INFO "Report descriptor size %d\n", hdev->dev_rsize);
    /*
     * The device gives us 2 separate USB endpoints.
     * One of those (the one with report descriptor size of 23) is just bogus so ignore it
     * 
     * we can tell because we get the following output from /var/log/messages:
     * Aug  7 11:54:10 kge kernel: Report descriptor size 23
     * Aug  7 11:54:10 kge mtp-probe[201656]: checking bus 3, device 35: "/sys/devices/pci0000:00/0000:00:14.0/usb3/3-7"
     * Aug  7 11:54:10 kge mtp-probe[201656]: bus: 3, device: 35 was not an MTP device
     * Aug  7 11:54:10 kge /usr/libexec/gdm-x-session[3310]: (**) PCsensor FootSwitch: Applying InputClass "system-keyboard"
     * Aug  7 11:54:10 kge /usr/libexec/gdm-x-session[3310]: (II) No input driver specified, ignoring this device.
     * Aug  7 11:54:10 kge /usr/libexec/gdm-x-session[3310]: (II) This device may have been added with another device file.
     * 
     * This "other device file" is the other driver with a much longer report descriptor
     * The reason it's so long is because this pedal can simulate all sorts of different things, including
     * mouse movements, meta keys, etc. 
     */
    if (hdev->dev_rsize == 23)
        return -ENODEV;

    ret = hid_parse(hdev);
    if (ret) {
        hid_err(hdev, "parse failed\n");
        return ret;
    }

    ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
    if (ret) {
        hid_err(hdev, "hw start failed\n");
        return ret;
    }

    return 0;
}

static const struct hid_device_id pedal_devices[] = {
    { HID_USB_DEVICE(0x3553, 0xb001) }, // vendor and product ID for PCSensor FootSwitch
    { HID_USB_DEVICE(0x1a86, 0xe026) }, // vendor and product ID for QinHeng FootSwitch
    {} // terminate with a null entry
};
MODULE_DEVICE_TABLE(hid, pedal_devices);

static struct hid_driver pedal_driver = {
    .name = "FootSwitch2",
    .id_table = pedal_devices,
    .report_fixup = pedal_report_fixup,
    .probe = pedal_probe,
};
module_hid_driver(pedal_driver);

MODULE_AUTHOR("Kenneth Ge <kge@redhat.com>");
MODULE_DESCRIPTION("HID driver for PCSensor/QinHeng FootSwitch");
MODULE_LICENSE("GPL");