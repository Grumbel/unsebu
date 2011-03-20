/*
**  Xbox360 USB Gamepad Userspace Driver
**  Copyright (C) 2011 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_XBOXDRV_USB_CONTROLLER_HPP
#define HEADER_XBOXDRV_USB_CONTROLLER_HPP

#include <libusb.h>
#include <string>
#include <memory>

#include "controller.hpp"

class USBController : public Controller
{
protected:
  libusb_device* m_dev;
  libusb_device_handle* m_handle;

  libusb_transfer* m_read_transfer;

  std::string m_usbpath;
  std::string m_usbid;
  std::string m_name;

public:
  USBController(libusb_device* dev);
  virtual ~USBController();
  
  virtual std::string get_usbpath() const;
  virtual std::string get_usbid() const;
  virtual std::string get_name() const;

  virtual bool parse(uint8_t* data, int len, XboxGenericMsg* msg_out) =0;

  void usb_claim_interface(int ifnum, bool try_detach);
  void usb_release_interface(int ifnum);
  void usb_write(int endpoint, uint8_t* data, int len);
  int  usb_find_ep(int direction, uint8_t if_class, uint8_t if_subclass, uint8_t if_protocol);

  void usb_submit_read(int endpoint, int len);
  void usb_cancel_read();

  void on_read_data(libusb_transfer *transfer);
  static void on_read_data_wrap(libusb_transfer *transfer)
  {
    static_cast<USBController*>(transfer->user_data)->on_read_data(transfer);
  }

private:
  USBController(const USBController&);
  USBController& operator=(const USBController&);
};

#endif

/* EOF */
