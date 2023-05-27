/*
**  Xbox360 USB Gamepad Userspace Driver
**  Copyright (C) 2011 Ingo Ruhnke <grumbel@gmail.com>
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

#include "usb_gsource.hpp"

#include <algorithm>
#include <assert.h>
#include <poll.h>
#include <libusb.h>
#include <stdlib.h>

#include <logmich/log.hpp>

#include "usb_helper.hpp"

namespace unsebu {

USBGSource::USBGSource() :
  m_source_funcs(),
  m_source(),
  m_source_id(),
  m_pollfds()
{
  // create the source functions
  m_source_funcs.prepare  = &USBGSource::on_source_prepare;
  m_source_funcs.check    = &USBGSource::on_source_check;
  m_source_funcs.dispatch = &USBGSource::on_source_dispatch;
  m_source_funcs.finalize = NULL;

  m_source_funcs.closure_callback = NULL;
  m_source_funcs.closure_marshal  = NULL;

  // create the source itself
  m_source = reinterpret_cast<GUSBSource*>(g_source_new(&m_source_funcs, sizeof(GUSBSource)));
  m_source->usb_source = this;
  g_source_set_callback(&m_source->source,
                        [](void* userdata) -> gboolean {
                          return static_cast<USBGSource*>(userdata)->on_source();
                        },
                        this,
                        NULL);

  // add pollfds to source
  libusb_pollfd const** fds = libusb_get_pollfds(NULL);
  for(libusb_pollfd const** i = fds; *i != NULL; ++i)
  {
    on_usb_pollfd_added((*i)->fd, (*i)->events);
  }
  free(fds);

  // register pollfd callbacks
  libusb_set_pollfd_notifiers(NULL,
                              [](int fd, short events, void* userdata) {
                                static_cast<USBGSource*>(userdata)->on_usb_pollfd_added(fd, events);
                              },
                              [](int fd,  void* userdata) {
                                static_cast<USBGSource*>(userdata)->on_usb_pollfd_removed(fd);
                              },
                              this);
}

USBGSource::~USBGSource()
{
  // get rid of the callbacks as they will be triggered by libusb_exit()
  libusb_set_pollfd_notifiers(NULL, NULL, NULL, NULL);

  // get rid of the GSource created in the constructor
  g_source_unref(reinterpret_cast<GSource*>(m_source));
}

void
USBGSource::attach(GMainContext* context)
{
  // attach source
  m_source_id = g_source_attach(&m_source->source, context);
}

void
USBGSource::on_usb_pollfd_added(int fd, short events)
{
  auto gfd = std::make_unique<GPollFD>();

  gfd->fd = fd;
  gfd->events  = events;
  gfd->revents = 0;

  g_source_add_poll(&m_source->source, gfd.get());

  m_pollfds.push_back(std::move(gfd));
}

void
USBGSource::on_usb_pollfd_removed(int fd)
{
  auto it = std::find_if(m_pollfds.begin(), m_pollfds.end(), [fd](std::unique_ptr<GPollFD> const& el){
    return el->fd == fd;
  });

  assert(it != m_pollfds.end());

  // FIXME: here is a bug
  g_source_remove_poll(&m_source->source, it->get());

  m_pollfds.erase(it);
}

gboolean
USBGSource::on_source_prepare(GSource* source, gint* timeout)
{
  struct timeval tv;
  int err = libusb_get_next_timeout(NULL, &tv);

  if (err == 0) // no timeouts
  {
    *timeout = -1;
  }
  else if (err == 1) // timeout was returned
  {
    *timeout = static_cast<gint>((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
  }
  else
  {
    log_error("libusb_get_next_timeout() failed: {}", libusb_strerror(err));
    *timeout = -1;
  }

  // FALSE means the source isn't yet ready
  return FALSE;
}

gboolean
USBGSource::on_source_check(GSource* source)
{
  USBGSource* usb_source = reinterpret_cast<GUSBSource*>(source)->usb_source;
  //log_debug("Number of PollFD: " << usb_source->m_pollfds.size());
  for(auto i = usb_source->m_pollfds.begin(); i != usb_source->m_pollfds.end(); ++i)
  {
    if (false)
    {
      log_debug("GSource GPollFD: {}", (*i)->fd);
      log_debug("REvents: G_IO_OUT: {}", ((*i)->revents & G_IO_OUT));
      log_debug("         G_IO_IN:  {}", ((*i)->revents & G_IO_IN));
      log_debug("         G_IO_PRI: {}", ((*i)->revents & G_IO_PRI));
      log_debug("         G_IO_HUP: {}", ((*i)->revents & G_IO_HUP));
      log_debug("         G_IO_ERR: {}", ((*i)->revents & G_IO_ERR));
    }

    if ((*i)->revents)
    {
      return TRUE;
    }
  }

  return FALSE;
}

gboolean
USBGSource::on_source_dispatch(GSource* source, GSourceFunc callback, gpointer userdata)
{
  return callback(userdata);
}

gboolean
USBGSource::on_source()
{
  libusb_handle_events(NULL);
  return TRUE;
}

} // namespace unsebu

/* EOF */
