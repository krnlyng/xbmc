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

#include <memory>

#include "input/linux/Keymap.h"

class IDllWaylandClient;
class IDllXKBCommon;

struct xkb_context;

namespace xbmc
{
namespace wayland
{
class IKeyboardReceiver
{
public:

  virtual ~IKeyboardReceiver() {}

  virtual void UpdateKeymap(ILinuxKeymap *) = 0;
  virtual void Enter(uint32_t serial,
                     struct wl_surface *surface,
                     struct wl_array *keys) = 0;
  virtual void Leave(uint32_t serial,
                     struct wl_surface *surface) = 0;
  virtual void Key(uint32_t serial,
                   uint32_t time,
                   uint32_t key,
                   enum wl_keyboard_key_state state) = 0;
  virtual void Modifier(uint32_t serial,
                        uint32_t depressed,
                        uint32_t latched,
                        uint32_t locked,
                        uint32_t group) = 0;
};

/* Wrapper class for a keyboard object. Generally there is one keyboard
 * per seat.
 * 
 * Keyboard events are translated into a more readable form and
 * forwarded on to the injected IKeyboardReceiver for further 
 * processing.
 * 
 * Many of these events require some shared agreement between the
 * compositor and the client as to the keymap in use. A file descriptor
 * for a shared memory region to a serialized keymap parsable
 * with libxkbcommon is provided in HandleKeymap and to the
 * registered IKeyboardReceiever through UpdateKeymap. The delegate for
 * that interface should ascertain the intended keymap before processing
 * any other events.
 */
class Keyboard
{
public:

  Keyboard(IDllWaylandClient &,
           IDllXKBCommon &,
           struct wl_keyboard *,
           IKeyboardReceiver &);
  ~Keyboard();

  Keyboard(const Keyboard &) = delete;
  Keyboard &operator=(const Keyboard &) = delete;

  struct wl_keyboard * GetWlKeyboard();

  static void HandleKeymapCallback(void *,
                                   struct wl_keyboard *,
                                   uint32_t,
                                   int,
                                   uint32_t);
  static void HandleEnterCallback(void *,
                                  struct wl_keyboard *,
                                  uint32_t,
                                  struct wl_surface *,
                                  struct wl_array *);
  static void HandleLeaveCallback(void *,
                                  struct wl_keyboard *,
                                  uint32_t,
                                  struct wl_surface *);
  static void HandleKeyCallback(void *,
                                struct wl_keyboard *,
                                uint32_t,
                                uint32_t,
                                uint32_t,
                                uint32_t);
  static void HandleModifiersCallback(void *,
                                      struct wl_keyboard *,
                                      uint32_t,
                                      uint32_t,
                                      uint32_t,
                                      uint32_t,
                                      uint32_t);

private:

  void HandleKeymap(uint32_t format,
                    int fd,
                    uint32_t size);
  void HandleEnter(uint32_t serial,
                   struct wl_surface *surface,
                   struct wl_array *keys);
  void HandleLeave(uint32_t serial,
                   struct wl_surface *surface);
  void HandleKey(uint32_t serial,
                 uint32_t time,
                 uint32_t key,
                 uint32_t state);
  void HandleModifiers(uint32_t serial,
                       uint32_t mods_depressed,
                       uint32_t mods_latched,
                       uint32_t mods_locked,
                       uint32_t group);

  static const struct wl_keyboard_listener m_listener;

  IDllWaylandClient &m_clientLibrary;
  IDllXKBCommon &m_xkbCommonLibrary;

  struct XkbContextDeleter {
    IDllXKBCommon &m_xkbCommonLibrary;

    constexpr explicit XkbContextDeleter(IDllXKBCommon &xkbCommonLibrary)
      :m_xkbCommonLibrary(xkbCommonLibrary) {}

    void operator()(struct xkb_context *c);
  };
  
  std::unique_ptr<struct xkb_context, XkbContextDeleter> m_xkbCommonContext;
  struct wl_keyboard *m_keyboard;
  IKeyboardReceiver &m_reciever;

  /* Keyboard owns the keymap object, but it might inject observing
   * references elsewhere in order to assist those objects in their
   * processing */
  std::unique_ptr<ILinuxKeymap> m_keymap;
};
}
}
