/* 
**  Xbox/Xbox360 USB Gamepad Userspace Driver
**  Copyright (C) 2008 Ingo Ruhnke <grumbel@gmx.de>
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

#include <assert.h>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <usb.h>

#include "usb_read_thread.hpp"

USBReadThread::USBReadThread(struct usb_dev_handle* handle, int endpoint, int len) : 
  m_handle(handle),
  m_read_endpoint(endpoint),
  m_read_length(len),
  m_stop(false)
{
}

USBReadThread::~USBReadThread()
{
  if (!m_stop)
    stop_thread();

  for(std::list<Paket>::iterator i = m_read_buffer.begin(); i != m_read_buffer.end(); ++i)
    delete[] i->data;
}
  
void
USBReadThread::start_thread()
{
  m_stop = false;
  m_thread = std::auto_ptr<boost::thread>(new boost::thread(boost::bind(&USBReadThread::run, this)));
}

void
USBReadThread::stop_thread()
{
  m_stop = true;
  m_thread->join();
}

int
USBReadThread::read(uint8_t* data, int len, int timeout)
{
  boost::mutex::scoped_lock lock(m_read_buffer_mutex);

  if (!m_read_buffer_cond.timed_wait(lock, boost::posix_time::milliseconds(timeout), buffer_not_empty(m_read_buffer)))
    return 0;

  assert(len == m_read_length);

  Paket& paket = m_read_buffer.front();

  memcpy(data, paket.data, m_read_length);
  delete[] paket.data;
  int ret = paket.length;

  m_read_buffer.pop_front();

  return ret;
}

void
USBReadThread::run()
{
  while(!m_stop)
    {
      uint8_t* data = new uint8_t[m_read_length];

      int ret = usb_interrupt_read(m_handle, m_read_endpoint, (char*)data, m_read_length, 0 /*timeout*/);

      if (ret == 0)
        {
          delete[] data;
        }
      else
        {
          boost::mutex::scoped_lock lock(m_read_buffer_mutex);

          Paket paket;
          paket.data   = data;
          paket.length = ret;

          m_read_buffer.push_back(paket);
          
          m_read_buffer_cond.notify_one();
        }
    }
}

/* EOF */
