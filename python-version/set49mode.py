#!/usr/bin/python

# this is a self-contained script that does not need compiling
# 
# To use this, you must have PYUSB installed
# sudo apt-get install python libusb-1.0
# sudo apt-get install python-pip
# sudo pip install --upgrade pyusb
# call like so: 
# sudo python set49mode.py --debug --mode 1 --joystick 0
# sudo python set49mode.py -h

import usb.core
import usb.util
import sys, getopt
#import time
import os

argv = sys.argv
msg = [204,1]
mode = '1'
joystick = 'happs'
debug = 0

#find device
dev = usb.core.find(idVendor=0xfafa, idProduct=0x0007)
if dev is None:
    raise ValueError('No GPWiz49 could be found')
    exit()
    
#claim device
if dev.is_kernel_driver_active(0) is True:
    dev.detach_kernel_driver(0)
    usb.util.claim_interface(dev,0)

#activate device
if not dev.get_active_configuration():
    dev.set_configuration(1)

# happs
# 1 2041 = 49
# 2 2042 = progressive 49
# 3 2043 = 8-way
# 4 2044 = 4-way
# 5 2045 = 4-way diag
# 6 2046 = 2-way horiz
# 7 2047 = 2-way vert
# 8 2048 = 16-way
#
# williams
# 1 20411 = 49
# 2 20412 = progressive 49
# 3 20413 = 8-way
# 4 20414 = 4-way
# 5 20415 = 4-way diag
# 6 20416 = 2-way horiz
# 7 20417 = 2-way vert
# 8 20418 = 16-way

# Remove 1st argument from the list of command line arguments 
argumentList = sys.argv[1:] 
  
# Options 
options = "hdm:j:"
  
# Long options 
long_options = ["Help", "Debug", "Mode=", "Joystick="]           
    
try:
    arguments, values = getopt.getopt(argumentList, options, long_options)
except getopt.GetoptError:
    print 'set49mode.py -h -m <mode> -j <happs/williams>'
    sys.exit(2)
for currentArgument, currentValue in arguments:
    if currentArgument in ("-h", "--help"):
        print 'sudo python set49mode.py -m <mode> -j <happs/williams>'
        print ''
        print 'This software is to set the joystick mode for the GPWiz49 Joystick from GroovyGameGear. '
        print 'This script will not work on the other GGG 49way encoders, just the GPWiz 49,'
        print 'though it should be easy to change the product ID to support other encoders'
        print 'https://groovygamegear.com/webstore/index.php?main_page=product_info&cPath=76_81&products_id=233'
        print 'If no arguments are passed in, this software will set the joystick to 49-way mode for a Happs joystick'
        print ''
        print '====================================='
        print ''
        print 'Note: To use the GGG GPWiz49 on a Raspberry Pi, you will need to set some HID quirks.' 
        print '1. Create file /etc/modprobe.d/usbhid.conf with the following:'
        print 'options usbhid quirks=0xFAFA:0x0007:0x00000020,0xFAFA:0x0008:0x00000020'
        print '2. Add a hid quirk to the end of boot script: sudo pico /boot/cmdline.txt'
        print 'usbhid.quirks=0xFAFA:0x0007:0x00000020,0xFAFA:0x0008:0x00000020'
        print 'If you want to use one of the other GGG products, you will need to change the existing product id: 0x0007'
        print ''
        print '===================================='
        print ''
        print '-h --help Help'
        print '-d --debug Debug mode, will output information about selection & joystick type'
        print ''
        print '===================================='
        print ''
        print '-m --mode Restriction Mode'
        print '1: 49-way (default)'
        print '2: Progressive 49'
        print '3: 8-Way'
        print '4: 4-Way'
        print '5: 4-Way Diagonal'
        print '6: 2-Way Horizontal'
        print '7: 2-Way Vertical'
        print '8: 16-Way / Large dead zone in center'
        print ''
        print '===================================='
        print '-j --joystick Joystick'
        print 'happs (default)'
        print 'williams'
        sys.exit()
    elif currentArgument in ("-m", "--mode"):
        if debug:
            print 'mode set', currentValue
        mode = currentValue
    elif currentArgument in ("-d", "--debug"):
        debug = 1
    elif currentArgument in ("-j", "--joystick"):
        if debug:
            print 'joystick set', currentValue
        joystick = currentValue

if joystick == "happs":
    if mode == "2":
        msg = [204,2] 
    elif mode == "3": 
        msg = [204,3]
    elif mode == "4": 
        msg = [204,4]
    elif mode == "5": 
        msg = [204,5]
    elif mode == "6": 
        msg = [204, 6] 
    elif mode == "7": 
        msg = [204,7] 
    elif mode == "8": 
        msg = [204,8]
    else: 
        msg = [204,1]    
else: 
    if mode == "2":
        msg = [204,12]
    elif mode == "3": 
        msg = [204,13] 
    elif mode == "4": 
        msg = [204,14] 
    elif mode == "5": 
        msg = [204,15]
    elif mode == "6": 
        msg = [204,16] 
    elif mode == "7": 
        msg = [204,17] 
    elif mode == "8": 
        msg = [204,18] 
    else: 
        msg = [204,11]   

if debug:
    print 'Mode ', mode
    print 'Joystick ', joystick
    print 'Message ', msg
    print 'argv', argv


#HID - 0x21 - 
#SET_REPORT - 0x09
#Report Type and Report ID - 0x02 0x00
#Interface - 0
#list of values - msg

response = dev.ctrl_transfer(0x21, 0x09, 0x0200, 0, msg)
if debug:
    print "response", response

#release device
usb.util.release_interface(dev,0)
dev.attach_kernel_driver(0)


exit()
