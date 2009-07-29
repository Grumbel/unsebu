/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_USB_READ_THREAD_HPP
#define HEADER_USB_READ_THREAD_HPP

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <list>

class USBReadThread
{
private:
  struct usb_dev_handle* m_handle;
  const int m_read_endpoint;
  const int m_read_length;

  struct Paket {
    uint8_t* data;
    int      length;
  };

  std::list<Paket> m_read_buffer;
  boost::mutex m_read_buffer_mutex;
  boost::thread m_thread;

  bool m_stop;

public:
  USBReadThread(struct usb_dev_handle* handle, int endpoint, int len);
  ~USBReadThread();

  int read(uint8_t* data, int len);

  void start_thread();
  void stop_thread();

private:
  void run();

private:
  USBReadThread(const USBReadThread&);
  USBReadThread& operator=(const USBReadThread&);
};

#endif

/* EOF */
