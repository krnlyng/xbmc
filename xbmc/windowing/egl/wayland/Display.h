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
#include <functional>
#include <memory>

class IDllWaylandClient;

struct wl_display;
struct wl_callback;

typedef struct wl_display * EGLNativeDisplayType;

namespace xbmc
{
namespace wayland
{
class Display
{
  public:

    Display(IDllWaylandClient &clientLibrary);
    ~Display();

    Display(const Display &) = delete;
    Display &operator=(const Display &) = delete;

    struct wl_display * GetWlDisplay();
    EGLNativeDisplayType* GetEGLNativeDisplay();
    struct wl_callback * Sync();

  private:

    IDllWaylandClient &m_clientLibrary;
    struct wl_display *m_display;
};

/* This is effectively just a seam for testing purposes so that
 * we can listen for extra objects that the core implementation might
 * not necessarily be interested in */
class WaylandDisplayListener
{
public:

  typedef std::function<void(Display &)> Handler;
  
  void SetHandler(const Handler &);
  void DisplayAvailable(Display &);

  static WaylandDisplayListener & GetInstance();
private:

  Handler m_handler;
  
  static std::unique_ptr<WaylandDisplayListener> m_instance;
};
}
}
