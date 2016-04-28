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

#include "system.h"

#if HAS_GLES == 2
#include "FrameBufferObjectGLES.h"
#include "windowing/WindowingFactory.h"
#include "utils/GLUtils.h"
#include "utils/log.h"

//////////////////////////////////////////////////////////////////////
// CFrameBufferObjectGLES
//////////////////////////////////////////////////////////////////////

CFrameBufferObjectGLES::CFrameBufferObjectGLES()
{
  m_fbo = 0;
  m_valid = false;
  m_supported = false;
  m_bound = false;
  m_texid = 0;
}

bool CFrameBufferObjectGLES::IsSupported()
{
  if(g_Windowing.IsExtSupported("GL_EXT_framebuffer_object"))
    m_supported = true;
  else
    m_supported = false;
  return m_supported;
}

bool CFrameBufferObjectGLES::Initialize()
{
  if (!IsSupported())
    return false;

  Cleanup();

  glGenFramebuffers(1, &m_fbo);
  VerifyGLState();

  if (!m_fbo)
    return false;

  m_valid = true;
  return true;
}

void CFrameBufferObjectGLES::Cleanup()
{
  if (!IsValid())
    return;

  if (m_fbo)
    glDeleteFramebuffers(1, &m_fbo);

  if (m_texid)
    glDeleteTextures(1, &m_texid);

  m_texid = 0;
  m_fbo = 0;
  m_valid = false;
  m_bound = false;
}

bool CFrameBufferObjectGLES::CreateAndBindToTexture(GLenum target, int width, int height, GLenum format,
                                                GLenum filter, GLenum clampmode)
{
  if (!IsValid())
    return false;

  if (m_texid)
    glDeleteTextures(1, &m_texid);

  glGenTextures(1, &m_texid);
  glBindTexture(target, m_texid);
  glTexImage2D(target, 0, format,  width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(target, GL_TEXTURE_WRAP_S, clampmode);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, clampmode);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
  VerifyGLState();
  return BindToTexture(target, m_texid);
}

void CFrameBufferObjectGLES::SetFiltering(GLenum target, GLenum mode)
{
  glBindTexture(target, m_texid);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mode);
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, mode);
}

bool CFrameBufferObjectGLES::BindToTexture(GLenum target, GLuint texid)
{
  if (!IsValid())
    return false;

  m_bound = false;
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  glBindTexture(target, texid);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, texid, 0);
  VerifyGLState();
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  if (status != GL_FRAMEBUFFER_COMPLETE)
  {
    VerifyGLState();
    return false;
  }
  m_bound = true;
  return true;
}

// Begin rendering to FBO
bool CFrameBufferObjectGLES::BeginRender()
{
  if (IsValid() && IsBound())
  {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    return true;
  }
  return false;
}

// Finish rendering to FBO
void CFrameBufferObjectGLES::EndRender() const
{
  if (IsValid())
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#endif
