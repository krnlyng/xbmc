/*
 *      Copyright (C) 2011-2015 Team XBMC
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

#include "EGLNativeTypeSunxiMali.h"
#include "system.h"
#include "guilib/gui3d.h"
#include "utils/StringUtils.h"

#include <EGL/egl.h>

#include <math.h>

CEGLNativeTypeSunxiMali::CEGLNativeTypeSunxiMali()
{
}

CEGLNativeTypeSunxiMali::~CEGLNativeTypeSunxiMali()
{
}

bool CEGLNativeTypeSunxiMali::CheckCompatibility()
{
  return true;
}

void CEGLNativeTypeSunxiMali::Initialize()
{
}

void CEGLNativeTypeSunxiMali::Destroy()
{
}

bool CEGLNativeTypeSunxiMali::CreateNativeDisplay()
{
  m_nativeDisplay = EGL_DEFAULT_DISPLAY;
  return true;
}

bool CEGLNativeTypeSunxiMali::CreateNativeWindow()
{
  // TODO: detect full-screen resolution
  mali_native_window.width = 1280;
  mali_native_window.height = 1024;
  m_nativeWindow = &mali_native_window;
  return true;
}

bool CEGLNativeTypeSunxiMali::GetNativeDisplay(XBNativeDisplayType **nativeDisplay) const
{
  if (!nativeDisplay)
    return false;
  *nativeDisplay = (XBNativeDisplayType*) &m_nativeDisplay;
  return true;
}

bool CEGLNativeTypeSunxiMali::GetNativeWindow(XBNativeWindowType **nativeWindow) const
{
  if (!nativeWindow || !m_nativeWindow)
    return false;
  *nativeWindow = (XBNativeWindowType*) &m_nativeWindow;
  return true;
}

bool CEGLNativeTypeSunxiMali::DestroyNativeDisplay()
{
  return true;
}

bool CEGLNativeTypeSunxiMali::DestroyNativeWindow()
{
  return true;
}

bool CEGLNativeTypeSunxiMali::GetNativeResolution(RESOLUTION_INFO *res) const
{
  // TODO: detect full-screen resolution
  res->iWidth = 1280;
  res->iHeight= 1024;

  res->fRefreshRate = 60;
  res->dwFlags= D3DPRESENTFLAG_PROGRESSIVE;
  res->iScreen       = 0;
  res->bFullScreen   = true;
  res->iSubtitles    = (int)(0.965 * res->iHeight);
  res->fPixelRatio   = 1.0f;
  res->iScreenWidth  = res->iWidth;
  res->iScreenHeight = res->iHeight;
  res->strMode       = StringUtils::Format("%dx%d @ %.2f%s - Full Screen", res->iScreenWidth, res->iScreenHeight, res->fRefreshRate,
  res->dwFlags & D3DPRESENTFLAG_INTERLACED ? "i" : "");

  return true;
}

bool CEGLNativeTypeSunxiMali::SetNativeResolution(const RESOLUTION_INFO &res)
{
  return false;
}

bool CEGLNativeTypeSunxiMali::ProbeResolutions(std::vector<RESOLUTION_INFO> &resolutions)
{
  RESOLUTION_INFO res;
  bool ret = GetNativeResolution(&res);
  if (ret && res.iWidth > 1 && res.iHeight > 1)
  {
    resolutions.push_back(res);
    return true;
  }
  return false;
}

bool CEGLNativeTypeSunxiMali::GetPreferredResolution(RESOLUTION_INFO *res) const
{
  return false;
}

bool CEGLNativeTypeSunxiMali::ShowWindow(bool show)
{
  return false;
}
