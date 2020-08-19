/*
    set49mode sets the mode of Groovy Game Gear's 49-way Joystick. 
    
    This script is based off of De Waegeneer Gijsbrecht's 2018 SetUltrastik360 script

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    contact: mahuti@gmail.com
 */

#include <chrono>
#include <cstring>
#include <iostream>
#include <libusb-1.0/libusb.h>
#include <sstream>
#include <thread>
#include <vector>
#include <algorithm>

#define UM_REQUEST_TYPE 0x21
#define UM_REQUEST 9
#define UM_TIMEOUT 2000
#define J_VENDOR_ID 0xFAFA
#define J_REPORT 0x0200
#define J_MESG_LENGTH 2
#define J_INTERFACE 0
#define KERNEL_DRIVER_ATTACHED 1
#define VERSION "1.0.0"

void cleanup(libusb_context * context, libusb_device_handle * devicehandle  ) {
   if (devicehandle) {
      libusb_close(devicehandle);
   }
   if (context) {
      libusb_exit(context);
   }
}

[[noreturn]] void
   errorhandler(libusb_context * context, libusb_device_handle * devicehandle, const std::string & errorMessage) {
   std::cerr << "ERROR: " << errorMessage << "\n";
   cleanup(context, devicehandle);
   exit(EXIT_FAILURE);
}

[[noreturn]] void errorhandler(libusb_context * context, libusb_device_handle * devicehandle, int rc) {
   std::cerr << "ERROR: " << libusb_error_name(rc) << " - " << libusb_strerror(static_cast<libusb_error>(rc)) << "\n";
   cleanup(context, devicehandle);
   exit(EXIT_FAILURE);
}

auto apply49mode(unsigned char modeId, unsigned char product_id) {
   libusb_context *       context(nullptr);
   libusb_device *        device(nullptr);
   libusb_device_handle * devicehandle(nullptr);
   libusb_device **       devices;
   auto                   rc = libusb_init(&context);

   if (rc != LIBUSB_SUCCESS) {
      errorhandler(context, devicehandle, rc);
   }
#if LIBUSB_API_VERSION >= 0x01000106
   libusb_set_option(context, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_WARNING);
#else
   libusb_set_debug(context, LIBUSB_LOG_LEVEL_WARNING);
#endif
   auto deviceCount = libusb_get_device_list(context, &devices);
   
     for (auto deviceIndex(0); deviceIndex < deviceCount; deviceIndex++) {
      device = devices[deviceIndex];
      libusb_device_descriptor descriptor = {};
      rc = libusb_get_device_descriptor(device, &descriptor);
      if (rc != LIBUSB_SUCCESS) {
         std::cout << "WARNING: " << libusb_error_name(rc) << " - " << libusb_strerror(static_cast<libusb_error>(rc))
                   << " - trying to proceed...\n";
      } else {
         if ((descriptor.idVendor == J_VENDOR_ID) && (descriptor.idProduct == product_id)) {
            break;
         }
      }
      device = nullptr;
   }
  
   if (device == nullptr) {
      
       std::cout << "GPWiz49Way Device id# " << static_cast<unsigned>(product_id) << " not found\n";
       cleanup(context, devicehandle);
       return 0; 
      // errorhandler(context, devicehandle, ss.str());
   } else {   // good to go
      rc = libusb_open(device, &devicehandle);
      if (rc != LIBUSB_SUCCESS) {
         errorhandler(context, devicehandle, rc);
      }
      if (devicehandle) {
         if (libusb_kernel_driver_active(devicehandle, J_INTERFACE) == KERNEL_DRIVER_ATTACHED) {
         
            rc = libusb_detach_kernel_driver(devicehandle, J_INTERFACE);
            libusb_claim_interface(devicehandle,J_INTERFACE);
            
            if (rc != LIBUSB_SUCCESS) {
               errorhandler(context, devicehandle, rc);
            }
         }
         
         
         unsigned char message[J_MESG_LENGTH] = { 204, modeId };  

        rc = libusb_control_transfer(devicehandle,
                                      UM_REQUEST_TYPE,
                                      UM_REQUEST,
                                      J_REPORT,
                                      J_INTERFACE,
                                      message,
                                      J_MESG_LENGTH,
                                      UM_TIMEOUT);
         std::cout << "GPWiz49way Device id# " << static_cast<unsigned>(product_id) << ((rc == sizeof(message)) ? " successfully converted " : " failed to convert ") << "to mode# " << static_cast<unsigned>(modeId) << "\n";
      }
   }
   libusb_release_interface(devicehandle, J_INTERFACE);
   rc = libusb_attach_kernel_driver(devicehandle, J_INTERFACE);
   libusb_free_device_list(devices, 1);
   cleanup(context, devicehandle);
   return 0; 
}

int main(int argc, char *argv[]) {
  unsigned char modeId(1);
  long int joystick(0);
  switch (argc) {
      case 3:
        try {
            joystick = std::stol(argv[2], nullptr, 10);
            if (joystick < 0 || joystick > 1) {
                joystick = 0;
            }
        } catch (std::exception &) {
            joystick = 0;
        }
      case 2:
        try {
            modeId = std::stol(argv[1], nullptr, 10);
            if (modeId < 1 || modeId > 8) {
                modeId = 1;
            }
        } catch (std::exception &) {
            modeId = 1;
        }

    break;
  default:
  
  
   std::cout << "GPWiz49 switcher Version " << VERSION << "\n\n"
         << "This software is to set the joystick mode for the GPWiz49 Joystick from GroovyGameGear. \n"
        << "https://groovygamegear.com/webstore/index.php?main_page=product_info&cPath=76_81&products_id=233\n"
        << "This script will not work on the other GGG 49way encoders, just the GPWiz49,\n"
        << "though it should be easy to change the product ID to support other encoders\n"
        << "\n"
        << "=====================================\n"
        << "\n"
        << "Note: To use the GGG GPWiz49 on a Raspberry Pi, you will need to set some HID quirks.\n" 
        << "1. Create file /etc/modprobe.d/usbhid.conf with the following:\n"
        << "    options usbhid quirks=0xFAFA:0x0007:0x00000020,0xFAFA:0x0008:0x00000020\n"
        << "2. Add a hid quirk to the end of boot script: sudo pico /boot/cmdline.txt\n"
        << "    usbhid.quirks=0xFAFA:0x0007:0x00000020,0xFAFA:0x0008:0x00000020\n"
        << "If you want to use one of the other GGG products, you will need to change the existing product id: 0x0007\n"
        << "3. You will also need to add a UDEV rule so that this app can be run without ROOT privileges\n"
        << "    sudo pico :/etc/udev/rules.d/50-set49mode.rules\n" 
        << "Add this:\n" 
        << "    ACTION=='add', SUBSYSTEMS=='usb', ATTRS{idVendor}=='fafa', ATTRS{idProduct}=='00ff', MODE:='666' \n"
        << "After saving and closing the UDEV rule, Reload UDEV\n"
        << "    sudo udevadm control --reload\n"
        << "\n"
        << "====================================\n"
        << "\n"
        << "Usage\n"
        << "set49mode <joystick-mode> <joystick-type>\n"
        << "modes:\n"
        << "1: 49-way (default)\n"
        << "2: Progressive 49\n"
        << "3: 8-Way\n"
        << "4: 4-Way\n"
        << "5: 4-Way Diagonal\n"
        << "6: 2-Way Horizontal\n"
        << "7: 2-Way Vertical\n"
        << "8: 16-Way / Large dead zone in center\n"
        << "\n"
        << "joystick types:\n"
        << "0: Happs (default doesn't need to be passed in)\n"
        << "1: Williams\n"
        << "\n"
        << "examples\n"
        << "    set49mode 5 1\n"
        << "GPWiz49 will switch to 4-Way Diagonal on a Williams stick\n"
        << "     set49mode 8\n"
        << "Will switch to 16-Way on a Happs stick\n"
        << "\n"
        << "====================================\n"
        << "\n"
        << "This program comes with ABSOLUTELY NO WARRANTY. This is free software,\nand you are welcome to redistribute it under certain conditions.\n"
        << "license: GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007\nCopyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>\n";
    return EXIT_SUCCESS;
  }
  // williams stick, just add 10. 
  if (joystick)
  {
      modeId+=10; 
  }
  
   apply49mode(modeId, 0x0007);
   apply49mode(modeId, 0x0008);
   apply49mode(modeId, 0x0009);
   apply49mode(modeId, 0x000a);

  return EXIT_SUCCESS;
}
