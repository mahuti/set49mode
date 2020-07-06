set49mode is a command line utility to set the status of the [GP-Wiz49 Joystick Interface](https://groovygamegear.com/webstore/index.php?main_page=product_info&cPath=76_81&products_id=233)

# Dependencies:
- libusb-1.0
- cmake 3.5.0 (3.8.0 recommended)

# Compilation:

    cd 'folder you extracted the archive to'
    mkdir -p builds/unix
    cd builds/unix
    cmake ../..
    make
    sudo make install
    
# About: 

This software is to set the joystick mode for the GPWiz49 Joystick from GroovyGameGear. 
https://groovygamegear.com/webstore/index.php?main_page=product_info&cPath=76_81&products_id=233
This script will not work on the other GGG 49way encoders, just the GPWiz49,
though it should be easy to change the product ID to support other encoders

=====================================

Note: To use the GGG GPWiz49 on a Raspberry Pi, you will need to set some HID quirks. 
1. Create file /etc/modprobe.d/usbhid.conf with the following:
 
        options usbhid quirks=0xFAFA:0x0007:0x00000020,0xFAFA:0x0008:0x00000020

2. Add a hid quirk to the end of boot script: sudo pico /boot/cmdline.txt

        usbhid.quirks=0xFAFA:0x0007:0x00000020,0xFAFA:0x0008:0x00000020

If you want to use one of the other GGG products, you will need to change the existing product id: 0x0007

3. You will also need to add a UDEV rule so that this app can be run without ROOT privileges
        
        sudo pico :/etc/udev/rules.d/50-set49mode.rules 

Add this:

        ACTION=='add', SUBSYSTEMS=='usb', ATTRS{idVendor}=='fafa', ATTRS{idProduct}=='00ff', MODE:='666' 

After saving and closing the UDEV rule, Reload UDEV

        sudo udevadm control --reload

====================================

# Usage

        set49mode <joystick-mode> <joystick-type>

modes:
1: 49-way (default)
2: Progressive 49
3: 8-Way
4: 4-Way
5: 4-Way Diagonal
6: 2-Way Horizontal
7: 2-Way Vertical
8: 16-Way / Large dead zone in center

joystick types:
0: Happs (default doesn't need to be passed in)
1: Williams

examples
        
        set49mode 5 1
        
GPWiz49 will switch to 4-Way Diagonal on a Williams stick

        set49mode 8
        
Will switch to 16-Way on a Happs stick