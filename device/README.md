# Device

Set of files intended to be placed on or executed per cluster node.

NOTE: OS type is Ubuntu Server 25.04, from raspberry pi imager.

NOTE: Process to mount a USB drive on linux is as follows:

```sh
lsblk # find the name of the USB device
sudo mkdir /mnt/usb
sudo mount $USB_NAME /mnt/usb
# sudo cp /mnt/usb/* ~
sudo umount /mnt/usb
```
