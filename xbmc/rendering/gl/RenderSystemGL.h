/*
 *      Copyright (C) 2005-2013 Team XBMC
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

#ifndef RENDER_SYSTEM_GL_H
#define RENDER_SYSTEM_GL_H

#pragma once

#include "system.h"
#include "system_gl.h"
#include "rendering/RenderSystem.h"

#ifdef HAS_GLES
#include "xbmc/guilib/GUIShader.h"

enum ESHADERMETHOD
{
  SM_DEFAULT,
  SM_TEXTURE,
  SM_MULTI,
  SM_FONTS,
  SM_TEXTURE_NOBLEND,
  SM_MULTI_BLENDCOLOR,
  SM_TEXTURE_RGBA,
  SM_TEXTURE_RGBA_OES,
  SM_TEXTURE_RGBA_BLENDCOLOR,
  SM_TEXTURE_RGBA_BOB,
  SM_TEXTURE_RGBA_BOB_OES,
  SM_ESHADERCOUNT
};

#endif

class CRenderSystemGL : public CRenderSystemBase
{
public:
  CRenderSystemGL();
  virtual ~CRenderSystemGL();

#ifdef HAS_GL
  void CheckOpenGLQuirks();
#endif
  bool InitRenderSystem() override;
  bool DestroyRenderSystem() override;
  bool ResetRenderSystem(int width, int height, bool fullScreen, float refreshRate) override;

  bool BeginRender() override;
  bool EndRender() override;
  void PresentRender(bool rendered) override;
  bool ClearBuffers(color_t color) override;
  bool IsExtSupported(const char* extension) override;

  void SetVSync(bool vsync);
  void ResetVSync() { m_bVsyncInit = false; }
#ifdef HAS_GL
  void FinishPipeline() override;
#endif

  void SetViewPort(CRect& viewPort) override;
  void GetViewPort(CRect& viewPort) override;

#ifdef HAS_GLES
  bool ScissorsCanEffectClipping() override;
  CRect ClipRectToScissorRect(const CRect &rect) override;
#endif
  void SetScissors(const CRect &rect) override;
  void ResetScissors() override;

  void CaptureStateBlock() override;
  void ApplyStateBlock() override;

  void SetCameraPosition(const CPoint &camera, int screenWidth, int screenHeight, float stereoFactor = 0.0f) override;

  void ApplyHardwareTransform(const TransformMatrix &matrix) override;
  void RestoreHardwareTransform() override;
#ifdef HAS_GL
  void SetStereoMode(RENDER_STEREO_MODE mode, RENDER_STEREO_VIEW view) override;
#endif
  bool SupportsStereo(RENDER_STEREO_MODE mode) const override;

  bool TestRender() override;

  void Project(float &x, float &y, float &z) override;

#ifdef HAS_GL
  void GetGLSLVersion(int& major, int& minor);

  void ResetGLErrors();
#else
  void InitialiseGUIShader();
  void EnableGUIShader(ESHADERMETHOD method);
  void DisableGUIShader();

  GLint GUIShaderGetPos();
  GLint GUIShaderGetCol();
  GLint GUIShaderGetCoord0();
  GLint GUIShaderGetCoord1();
  GLint GUIShaderGetUniCol();
  GLint GUIShaderGetCoord0Matrix();
  GLint GUIShaderGetField();
  GLint GUIShaderGetStep();
  GLint GUIShaderGetContrast();
  GLint GUIShaderGetBrightness();
  GLint GUIShaderGetModel();
#endif

protected:
  virtual void SetVSyncImpl(bool enable) = 0;
  virtual void PresentRenderImpl(bool rendered) = 0;
  void CalculateMaxTexturesize();

  int        m_iVSyncMode;
  int        m_iVSyncErrors;
  bool       m_bVsyncInit;
  int        m_width;
  int        m_height;

  std::string m_RenderExtensions;

#ifdef HAS_GL
  int        m_glslMajor = 0;
  int        m_glslMinor = 0;
#else
  CGUIShader  **m_pGUIshader = nullptr; // One GUI shader for each method
  ESHADERMETHOD m_method = SM_DEFAULT; // Current GUI Shader method
#endif

  GLint      m_viewPort[4];

#ifdef HAS_GL
  uint8_t m_latencyCounter = 0;
#endif
};

#endif // RENDER_SYSTEM_H
