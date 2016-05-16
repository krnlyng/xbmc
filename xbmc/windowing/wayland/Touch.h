#pragma once

/*
 *      Copyright (C) 2011-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */
#include <wayland-client.h>

#include <map>

class IDllWaylandClient;

namespace xbmc
{
namespace wayland
{

/* Wrapper class for a touch object. Generally there is one touch
 * per seat.
 * 
 * Events are forwarded from the internal proxy to an GenericTouchInputReceiver
 * for further processing. The responsibility of this class is only
 * to receive the events and represent them in a sensible way */
class Touch
{
public:

  Touch(IDllWaylandClient &,
          struct wl_touch *);
  ~Touch();

  struct wl_touch * GetWlTouch();

  static void HandleDownCallback(void *data,
		         struct wl_touch *touch,
		         uint32_t serial,
		         uint32_t time,
		         struct wl_surface *surface,
		         int32_t id,
		         wl_fixed_t x,
		         wl_fixed_t y);

  static void HandleUpCallback(void *data,
                                      struct wl_touch *touch,
                                      uint32_t serial,
                                      uint32_t time,
                                      int32_t id);

  static void HandleMotionCallback(void *data,
                                       struct wl_touch *touch,
                                       uint32_t time,
                                       int32_t id,
                                       wl_fixed_t x,
                                       wl_fixed_t y);

  static void HandleFrameCallback(void *data,
                                       struct wl_touch *touch);

  static void HandleCancleCallback(void *data,
                                       struct wl_touch *touch);

private:

  static std::pair<int, int> get_rotated_xy(double x, double y);

/*
  static int get_rotated_x(int x);
  static int get_rotated_y(int y);
*/

  static const struct wl_touch_listener m_listener;

  IDllWaylandClient &m_clientLibrary;
  struct wl_touch *m_touch;
  static std::map< int32_t, std::pair<wl_fixed_t, wl_fixed_t> > m_button_positions;
  static std::map<int32_t, int32_t> uniqueIdtoFingerId;

  static float m_aspect;
  static int m_width;
  static int m_height;
};
}
}
