/*
 *      Copyright (C) 2005-2014 Team XBMC
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

#pragma once

#if defined(HAVE_X11)

#include "WinSystemX11.h"
#include "EGL/egl.h"
#include "utils/GlobalsHandling.h"
#include "rendering/gl/RenderSystemGL.h"

#ifdef HAS_GL
#include "GL/glx.h"
#endif

class CGLContext;

class CWinSystemX11GLContext : public CWinSystemX11, public CRenderSystemGL
{
public:
  CWinSystemX11GLContext();
  virtual ~CWinSystemX11GLContext();

  bool CreateNewWindow(const std::string& name, bool fullScreen, RESOLUTION_INFO& res, PHANDLE_EVENT_FUNC userFunction) override;
  bool ResizeWindow(int newWidth, int newHeight, int newLeft, int newTop) override;
  bool SetFullScreen(bool fullScreen, RESOLUTION_INFO& res, bool blankOtherDisplays) override;
  bool DestroyWindowSystem() override;
  bool DestroyWindow() override;

  bool IsExtSupported(const char* extension) override;

#ifdef HAS_GL
  GLXWindow GetWindow() const;
  GLXContext GetGlxContext() const;
#endif

  EGLDisplay GetEGLDisplay() const;
  EGLSurface GetEGLSurface() const;
  EGLContext GetEGLContext() const;
  EGLConfig GetEGLConfig() const;

protected:
  bool SetWindow(int width, int height, bool fullscreen, const std::string &output, int *winstate = NULL) override;
  void PresentRenderImpl(bool rendered) override;
  void SetVSyncImpl(bool enable) override;
  bool RefreshGLContext(bool force);
  XVisualInfo* GetVisual() override;

  CGLContext *m_pGLContext = nullptr;
  bool m_newGlContext;
};

#ifdef HAS_GL
// TODO: why is this not enabled for GLES?
XBMC_GLOBAL_REF(CWinSystemX11GLContext,g_Windowing);
#endif

#define g_Windowing XBMC_GLOBAL_USE(CWinSystemX11GLContext)

#endif //HAVE_X11
