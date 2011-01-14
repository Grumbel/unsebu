/*
**  Xbox360 USB Gamepad Userspace Driver
**  Copyright (C) 2010 Ingo Ruhnke <grumbel@gmx.de>
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

#include "usb_helper.hpp"

#include <libusb.h>
#include <errno.h>

int usb_claim_n_detach_interface(struct libusb_device_handle* handle, int interface, bool try_detach)
{
  int ret = libusb_claim_interface(handle, interface);

  if (ret == LIBUSB_ERROR_BUSY)
  {
    if (try_detach)
    {
      ret = libusb_detach_kernel_driver(handle, interface);
      if (ret == LIBUSB_SUCCESS)
      {
        ret = libusb_claim_interface(handle, interface);
        return ret;
      }
      else
      {
        return ret;
      }
    }
    else
    {
      return ret;
    }
  }
  else
  {
    // success or unknown failure
    return ret;
  }
}

const char* usb_strerror(int err)
{
  switch(err)
  {
    case LIBUSB_SUCCESS: return "LIBUSB_SUCCESS";
    case LIBUSB_ERROR_IO: return "LIBUSB_ERROR_IO";
    case LIBUSB_ERROR_INVALID_PARAM: return "LIBUSB_ERROR_INVALID_PARAM";
    case LIBUSB_ERROR_ACCESS: return "LIBUSB_ERROR_ACCESS";
    case LIBUSB_ERROR_NO_DEVICE: return "LIBUSB_ERROR_NO_DEVICE";
    case LIBUSB_ERROR_NOT_FOUND: return "LIBUSB_ERROR_NOT_FOUND";
    case LIBUSB_ERROR_BUSY: return "LIBUSB_ERROR_BUSY";
    case LIBUSB_ERROR_TIMEOUT: return "LIBUSB_ERROR_TIMEOUT";
    case LIBUSB_ERROR_OVERFLOW: return "LIBUSB_ERROR_OVERFLOW";
    case LIBUSB_ERROR_PIPE: return "LIBUSB_ERROR_PIPE";
    case LIBUSB_ERROR_INTERRUPTED: return "LIBUSB_ERROR_INTERRUPTED";
    case LIBUSB_ERROR_NO_MEM: return "LIBUSB_ERROR_NO_MEM";
    case LIBUSB_ERROR_NOT_SUPPORTED: return "LIBUSB_ERROR_NOT_SUPPORTED";
    case LIBUSB_ERROR_OTHER: return "LIBUSB_ERROR_OTHER";
    default: return "<unknown libusb error code>";
  }
}

/* EOF */