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
#include <sstream>
#include <iostream>
#include <stdexcept>

#include <wayland-client.h>

#include "DllWaylandClient.h"
#include "WaylandProtocol.h"
#include "Touch.h"
#include "input/touch/generic/GenericTouchActionHandler.h"
#include "input/touch/generic/GenericTouchInputHandler.h"

#include "settings/DisplaySettings.h"

namespace xw = xbmc::wayland;

const struct wl_touch_listener xw::Touch::m_listener =
{
    Touch::HandleDownCallback,
    Touch::HandleUpCallback,
    Touch::HandleMotionCallback,
    Touch::HandleFrameCallback,
    Touch::HandleCancleCallback,
};

std::map< int32_t, std::pair<wl_fixed_t, wl_fixed_t> > xw::Touch::m_button_positions;
std::map<int32_t, int32_t> xw::Touch::uniqueIdtoFingerId;

extern int g_current_width, g_current_height;

int xw::Touch::m_height = 0;
int xw::Touch::m_width = 0;
float xw::Touch::m_aspect = 0;

int32_t get_first_free_id(int32_t free_id, std::map<int32_t, int32_t> uniqueIdtoFingerId)
{
    for(std::map<int32_t, int32_t>::iterator it=uniqueIdtoFingerId.begin();it != uniqueIdtoFingerId.end();it++)
    {
        if(it->second == free_id)
        {
            return get_first_free_id(++free_id, uniqueIdtoFingerId);
        }
    }

    return free_id;
}

xw::Touch::Touch(IDllWaylandClient &clientLibrary,
                     struct wl_touch *touch) :
  m_clientLibrary(clientLibrary),
  m_touch(touch)
{
  protocol::AddListenerOnWaylandObject(m_clientLibrary,
                                       touch,
                                       &m_listener,
                                       this);
  CGenericTouchInputHandler::GetInstance().RegisterHandler(&CGenericTouchActionHandler::GetInstance());

  m_height = g_current_height;
  m_width = g_current_width;

  m_aspect = (float)m_width/(float)m_height;
}

xw::Touch::~Touch()
{
  protocol::DestroyWaylandObject(m_clientLibrary,
                                 m_touch);
  CGenericTouchInputHandler::GetInstance().UnregisterHandler();
}

void xw::Touch::HandleDownCallback(void *data,
                                      struct wl_touch *touch,
                                      uint32_t serial,
                                      uint32_t time,
                                      struct wl_surface *surface,
                                      int32_t id,
                                      wl_fixed_t x,
                                      wl_fixed_t y)
{
    m_height = g_current_height;
    m_width = g_current_width;

    m_aspect = (float)m_width/(float)m_height;

    m_button_positions[id].first = x;
    m_button_positions[id].second = y;

    int32_t first_free = get_first_free_id(0, uniqueIdtoFingerId);
    uniqueIdtoFingerId[id] = first_free;

    int rx, ry;
    std::pair<int,int> xy = get_rotated_xy(wl_fixed_to_double(x), wl_fixed_to_double(y));
    rx = xy.first;
    ry = xy.second;

    for(unsigned int pointer = 0; pointer < TOUCH_MAX_POINTERS; pointer++)
    {
        CGenericTouchInputHandler::GetInstance().UpdateTouchPointer(pointer, rx, ry, time, 10.f);
    }

    CGenericTouchInputHandler::GetInstance().HandleTouchInput(TouchInputDown, rx, ry, time, uniqueIdtoFingerId[id], 10.f);
}

void xw::Touch::HandleUpCallback(void *data,
                                      struct wl_touch *touch,
                                      uint32_t serial,
                                      uint32_t time,
                                      int32_t id)
{
    m_height = g_current_height;
    m_width = g_current_width;

    m_aspect = (float)m_width/(float)m_height;

    std::map< int32_t, std::pair<wl_fixed_t, wl_fixed_t> >::iterator it = m_button_positions.find(id);

    if(it != m_button_positions.end())
    {
        int rx, ry;
        std::pair<int,int> xy = get_rotated_xy(wl_fixed_to_double(it->second.first), wl_fixed_to_double(it->second.second));
        rx = xy.first;
        ry = xy.second;

        for(unsigned int pointer = 0; pointer < TOUCH_MAX_POINTERS; pointer++)
        {
            CGenericTouchInputHandler::GetInstance().UpdateTouchPointer(pointer, rx, ry, time, 10.f);
        }

        CGenericTouchInputHandler::GetInstance().HandleTouchInput(TouchInputUp, rx, ry, time, uniqueIdtoFingerId[id], 10.f);
        uniqueIdtoFingerId.erase(uniqueIdtoFingerId.find(id));
        m_button_positions.erase(it);
    }
}

void xw::Touch::HandleMotionCallback(void *data,
                                       struct wl_touch *touch,
                                       uint32_t time,
                                       int32_t id,
                                       wl_fixed_t x,
                                       wl_fixed_t y)
{
    m_width = g_current_width;
    m_height = g_current_height;

    m_aspect = (float)m_width / (float)m_height;

    m_button_positions[id].first = x;
    m_button_positions[id].second = y;

    int rx, ry;
    std::pair<int,int> xy = get_rotated_xy(wl_fixed_to_double(x), wl_fixed_to_double(y));
    rx = xy.first;
    ry = xy.second;

    for(unsigned int pointer = 0; pointer < TOUCH_MAX_POINTERS; pointer++)
    {
        CGenericTouchInputHandler::GetInstance().UpdateTouchPointer(pointer, rx, ry, time, 10.f);
    }

    CGenericTouchInputHandler::GetInstance().HandleTouchInput(TouchInputMove, rx, ry, time, uniqueIdtoFingerId[id], 10.f);
}

void xw::Touch::HandleFrameCallback(void *data,
                                       struct wl_touch *touch)
{
}

void xw::Touch::HandleCancleCallback(void *data,
                                       struct wl_touch *touch)
{
}

std::pair<int, int> xw::Touch::get_rotated_xy(double x, double y)
{
    auto r = std::make_pair<int, int>(m_width - y, x);
    return r;
}

