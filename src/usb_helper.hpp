/*
**  Xbox360 USB Gamepad Userspace Driver
**  Copyright (C) 2010 Ingo Ruhnke <grumbel@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef HEADER_XBOXDRV_USB_HELPER_HPP
#define HEADER_XBOXDRV_USB_HELPER_HPP

#include <libusb.h>

int usb_claim_n_detach_interface(libusb_device_handle* handle, int interface, bool try_detach);
const char* usb_strerror(int err);
const char* usb_transfer_strerror(libusb_transfer_status err);
libusb_device* usb_find_device_by_path(uint8_t busnum, uint8_t devnum);

#endif

/* EOF */
