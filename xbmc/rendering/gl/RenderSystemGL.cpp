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

#if defined(HAS_GL) || defined(HAS_GLES)

#include "RenderSystemGL.h"
#include "guilib/GraphicContext.h"
#include "settings/AdvancedSettings.h"
#include "guilib/MatrixGLES.h"
#include "utils/log.h"
#include "utils/GLUtils.h"
#include "utils/TimeUtils.h"
#include "utils/SystemInfo.h"
#include "utils/MathUtils.h"

#ifdef HAS_GL
#include "utils/StringUtils.h"
#else
#include "windowing/WindowingFactory.h"
#endif

#ifdef TARGET_POSIX
#include "linux/XTimeUtils.h"
#endif

#ifdef HAS_GLES

static const char* ShaderNames[SM_ESHADERCOUNT] =
    {"guishader_frag_default.glsl",
     "guishader_frag_texture.glsl",
     "guishader_frag_multi.glsl",
     "guishader_frag_fonts.glsl",
     "guishader_frag_texture_noblend.glsl",
     "guishader_frag_multi_blendcolor.glsl",
     "guishader_frag_rgba.glsl",
     "guishader_frag_rgba_oes.glsl",
     "guishader_frag_rgba_blendcolor.glsl",
     "guishader_frag_rgba_bob.glsl",
     "guishader_frag_rgba_bob_oes.glsl"
    };

#endif

CRenderSystemGL::CRenderSystemGL() : CRenderSystemBase()
{
#ifdef HAS_GL
  m_enumRenderingSystem = RENDERING_SYSTEM_OPENGL;
#else
  m_enumRenderingSystem = RENDERING_SYSTEM_OPENGLES;
#endif
}

CRenderSystemGL::~CRenderSystemGL()
{
}

#ifdef HAS_GL

void CRenderSystemGL::CheckOpenGLQuirks()

{
#ifdef TARGET_DARWIN_OSX
  if (m_RenderVendor.find("NVIDIA") != std::string::npos)
  {             
    // Nvidia 7300 (AppleTV) and 7600 cannot do DXT with NPOT under OSX
    // Nvidia 9400M is slow as a dog
    if (m_renderCaps & RENDER_CAPS_DXT_NPOT)
    {
      const char *arr[3]= { "7300","7600","9400M" };
      for(int j = 0; j < 3; j++)
      {
        if((int(m_RenderRenderer.find(arr[j])) > -1))
        {
          m_renderCaps &= ~ RENDER_CAPS_DXT_NPOT;
          break;
        }
      }
    }
  }
#ifdef __ppc__
  // ATI Radeon 9600 on osx PPC cannot do NPOT
  if (m_RenderRenderer.find("ATI Radeon 9600") != std::string::npos)
  {
    m_renderCaps &= ~ RENDER_CAPS_NPOT;
    m_renderCaps &= ~ RENDER_CAPS_DXT_NPOT;
  }
#endif
#endif
  if (StringUtils::EqualsNoCase(m_RenderVendor, "nouveau"))
    m_renderQuirks |= RENDER_QUIRKS_YV12_PREFERED;

  if (StringUtils::EqualsNoCase(m_RenderVendor, "Tungsten Graphics, Inc.")
  ||  StringUtils::EqualsNoCase(m_RenderVendor, "Tungsten Graphics, Inc"))
  {
    unsigned major, minor, micro;
    if (sscanf(m_RenderVersion.c_str(), "%*s Mesa %u.%u.%u", &major, &minor, &micro) == 3)
    {

      if((major  < 7)
      || (major == 7 && minor  < 7)
      || (major == 7 && minor == 7 && micro < 1))
        m_renderQuirks |= RENDER_QUIRKS_MAJORMEMLEAK_OVERLAYRENDERER;
    }
    else
      CLog::Log(LOGNOTICE, "CRenderSystemGL::CheckOpenGLQuirks - unable to parse mesa version string");

    if(m_RenderRenderer.find("Poulsbo") != std::string::npos)
      m_renderCaps &= ~RENDER_CAPS_DXT_NPOT;

    m_renderQuirks |= RENDER_QUIRKS_BROKEN_OCCLUSION_QUERY;
  }
}	

#endif

bool CRenderSystemGL::InitRenderSystem()
{
#ifdef HAS_GL
  m_maxTextureSize = 2048;
#else
  GLint maxTextureSize;

  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

  m_maxTextureSize = maxTextureSize;
#endif

  m_bVSync = false;
  m_iVSyncMode = 0;
  m_bVsyncInit = false;
  m_renderCaps = 0;

#ifdef HAS_GL
  m_RenderExtensions  = " ";
  m_RenderExtensions += (const char*) glGetString(GL_EXTENSIONS);
  m_RenderExtensions += " ";

  LogGraphicsInfo();
#endif

  // Get the GL version number
  m_RenderVersionMajor = 0;
  m_RenderVersionMinor = 0;

  const char* ver = (const char*)glGetString(GL_VERSION);
  if (ver != 0)
  {
    sscanf(ver, "%d.%d", &m_RenderVersionMajor, &m_RenderVersionMinor);
#ifdef HAS_GLES
    if (!m_RenderVersionMajor)
      sscanf(ver, "%*s %*s %d.%d", &m_RenderVersionMajor, &m_RenderVersionMinor);
#endif
    m_RenderVersion = ver;
  }

#ifdef HAS_GL
  if (IsExtSupported("GL_ARB_shading_language_100"))
  {
    ver = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    if (ver)
    {
      sscanf(ver, "%d.%d", &m_glslMajor, &m_glslMinor);
    }
    else
    {
      m_glslMajor = 1;
      m_glslMinor = 0;
    }
  }
#endif

  // Get our driver vendor and renderer
  const char* tmpVendor = (const char*) glGetString(GL_VENDOR);
  m_RenderVendor.clear();
  if (tmpVendor != NULL)
    m_RenderVendor = tmpVendor;

  const char* tmpRenderer = (const char*) glGetString(GL_RENDERER);
  m_RenderRenderer.clear();
  if (tmpRenderer != NULL)
    m_RenderRenderer = tmpRenderer;

#ifdef HAS_GL
  // grab our capabilities
  if (IsExtSupported("GL_EXT_texture_compression_s3tc"))
    m_renderCaps |= RENDER_CAPS_DXT;

  if (IsExtSupported("GL_ARB_texture_non_power_of_two"))
  {
    m_renderCaps |= RENDER_CAPS_NPOT;
    if (m_renderCaps & RENDER_CAPS_DXT) 
      m_renderCaps |= RENDER_CAPS_DXT_NPOT;
  }
  //Check OpenGL quirks and revert m_renderCaps as needed
  CheckOpenGLQuirks();
#else
  m_RenderExtensions  = " ";

  const char *tmpExtensions = (const char*) glGetString(GL_EXTENSIONS);
  if (tmpExtensions != NULL)
  {
    m_RenderExtensions += tmpExtensions;
  }

  m_RenderExtensions += " ";

  LogGraphicsInfo();

  if (IsExtSupported("GL_TEXTURE_NPOT"))
  {
    m_renderCaps |= RENDER_CAPS_NPOT;
  }

  if (IsExtSupported("GL_EXT_texture_format_BGRA8888"))
  {
    m_renderCaps |= RENDER_CAPS_BGRA;
  }

  if (IsExtSupported("GL_IMG_texture_format_BGRA8888"))
  {
    m_renderCaps |= RENDER_CAPS_BGRA;
  }

  if (IsExtSupported("GL_APPLE_texture_format_BGRA8888"))
  {
    m_renderCaps |= RENDER_CAPS_BGRA_APPLE;
  }
#endif

  m_bRenderCreated = true;

#ifdef HAS_GLES
  InitialiseGUIShader();
#endif

  return true;
}

bool CRenderSystemGL::ResetRenderSystem(int width, int height, bool fullScreen, float refreshRate)
{
  m_width = width;
  m_height = height;

  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

  CalculateMaxTexturesize();

//  CRect rect( 0, 0, width, height );
  CRect rect( 0, 0, width, height );
  SetViewPort( rect );

#ifdef HAS_GL
  glEnable(GL_TEXTURE_2D);
#endif
  glEnable(GL_SCISSOR_TEST);

  glMatrixProject.Clear();
  glMatrixModview->LoadIdentity();
  glMatrixProject->Ortho(0.0f, width-1, height-1, 0.0f, -1.0f, 1.0f);
  glMatrixProject.Load();

  glMatrixModview.Clear();
  glMatrixModview->LoadIdentity();
  glMatrixModview.Load();

  glMatrixTexture.Clear();
  glMatrixTexture->LoadIdentity();
  glMatrixTexture.Load();

#ifdef HAS_GL
  if (IsExtSupported("GL_ARB_multitexture"))
  {
    //clear error flags
    ResetGLErrors();

    GLint maxtex;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB, &maxtex);

    //some sanity checks
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
      CLog::Log(LOGERROR, "ResetRenderSystem() GL_MAX_TEXTURE_IMAGE_UNITS_ARB returned error %i", (int)error);
      maxtex = 3;
    }
    else if (maxtex < 1 || maxtex > 32)
    {
      CLog::Log(LOGERROR, "ResetRenderSystem() GL_MAX_TEXTURE_IMAGE_UNITS_ARB returned invalid value %i", (int)maxtex);
      maxtex = 3;
    }

    //reset texture matrix for all textures
    for (GLint i = 0; i < maxtex; i++)
    {
      glActiveTextureARB(GL_TEXTURE0 + i);
      glMatrixTexture.Load();
    }
    glActiveTextureARB(GL_TEXTURE0);
  }
#endif

  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glEnable(GL_BLEND);          // Turn Blending On
  glDisable(GL_DEPTH_TEST);

  return true;
}

bool CRenderSystemGL::DestroyRenderSystem()
{
#ifdef HAS_GLES
  CLog::Log(LOGDEBUG, "GUI Shader - Destroying Shader : %p", m_pGUIshader);

  if (m_pGUIshader)
  {
    for (int i = 0; i < SM_ESHADERCOUNT; i++)
    {
      if (m_pGUIshader[i])
      {
        m_pGUIshader[i]->Free();
        delete m_pGUIshader[i];
        m_pGUIshader[i] = NULL;
      }
    }
    delete[] m_pGUIshader;
    m_pGUIshader = NULL;
  }

  ResetScissors();
  CDirtyRegionList dirtyRegions;
  CDirtyRegion dirtyWindow(g_graphicsContext.GetViewWindow());
  dirtyRegions.push_back(dirtyWindow);

  ClearBuffers(0);
  glFinish();
  PresentRenderImpl(true);
#endif

  m_bRenderCreated = false;

  return true;
}

bool CRenderSystemGL::BeginRender()
{
  if (!m_bRenderCreated)
    return false;

  return true;
}

bool CRenderSystemGL::EndRender()
{
  if (!m_bRenderCreated)
    return false;

  return true;
}

bool CRenderSystemGL::ClearBuffers(color_t color)
{
  if (!m_bRenderCreated)
    return false;

#ifdef HAS_GL
  /* clear is not affected by stipple pattern, so we can only clear on first frame */
  if(m_stereoMode == RENDER_STEREO_MODE_INTERLACED && m_stereoView == RENDER_STEREO_VIEW_RIGHT)
    return true;
#endif

  float r = GET_R(color) / 255.0f;
  float g = GET_G(color) / 255.0f;
  float b = GET_B(color) / 255.0f;
  float a = GET_A(color) / 255.0f;

  glClearColor(r, g, b, a);

  GLbitfield flags = GL_COLOR_BUFFER_BIT;
  glClear(flags);

  return true;
}

bool CRenderSystemGL::IsExtSupported(const char* extension)
{
#ifdef HAS_GLES
  if (strcmp( extension, "GL_EXT_framebuffer_object" ) == 0)
  {
    // GLES has FBO as a core element, not an extension!
    return true;
  }
  else if (strcmp( extension, "GL_TEXTURE_NPOT" ) == 0)
  {
    // GLES supports non-power-of-two textures as standard.
	return true;
	/* Note: The wrap mode can only be GL_CLAMP_TO_EDGE and the minification filter can only be
	 * GL_NEAREST or GL_LINEAR (in other words, not mipmapped). The extension GL_OES_texture_npot
	 * relaxes these restrictions and allows wrap modes of GL_REPEAT and GL_MIRRORED_REPEAT and
	 * also	allows npot textures to be mipmapped with the full set of minification filters
	 */
  }
  else
  {
#endif
    std::string name;
    name  = " ";
    name += extension;
    name += " ";

    bool supported = m_RenderExtensions.find(name) != std::string::npos;
#ifdef HAS_GLES
    CLog::Log(LOGDEBUG, "GLES: Extension Support Test - %s %s", extension, supported ? "YES" : "NO");
#endif
    return supported;
#ifdef HAS_GLES
  }
#endif
}

void CRenderSystemGL::PresentRender(bool rendered)
{
  SetVSync(true);

  if (!m_bRenderCreated)
    return;

  PresentRenderImpl(rendered);
#ifdef HAS_GL
  m_latencyCounter++;
#endif

  if (!rendered)
    Sleep(40);
}

void CRenderSystemGL::SetVSync(bool enable)
{
  if (m_bVSync==enable && m_bVsyncInit == true)
    return;

  if (!m_bRenderCreated)
    return;

  if (enable)
    CLog::Log(LOGINFO, "GL: Enabling VSYNC");
  else
    CLog::Log(LOGINFO, "GL: Disabling VSYNC");

  m_iVSyncMode   = 0;
  m_iVSyncErrors = 0;
  m_bVSync       = enable;
  m_bVsyncInit   = true;

  SetVSyncImpl(enable);

  if (!enable)
    return;

  if (!m_iVSyncMode)
    CLog::Log(LOGERROR, "GL: Vertical Blank Syncing unsupported");
  else
    CLog::Log(LOGINFO, "GL: Selected vsync mode %d", m_iVSyncMode);
}

#ifdef HAS_GL

void CRenderSystemGL::FinishPipeline()
{
  // GL implementations are free to queue an undefined number of frames internally
  // as a result video latency can be very high which is bad for a/v sync
  // calling glFinish reduces latency to the number of back buffers
  // in order to keep some elasticity, we call glFinish only every other cycle
  if (m_latencyCounter & 0x01)
    glFinish();
}

#endif

void CRenderSystemGL::CaptureStateBlock()
{
  if (!m_bRenderCreated)
    return;

  glMatrixProject.Push();
  glMatrixModview.Push();
  glMatrixTexture.Push();

  glDisable(GL_SCISSOR_TEST); // fixes FBO corruption on Macs
#ifdef HAS_GL
  if (glActiveTextureARB)
    glActiveTextureARB(GL_TEXTURE0_ARB);
  glDisable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glColor3f(1.0, 1.0, 1.0);
#else
  glActiveTexture(GL_TEXTURE0);
//TODO - NOTE: Only for Screensavers & Visualisations
//  glColor3f(1.0, 1.0, 1.0);
#endif
}

void CRenderSystemGL::ApplyStateBlock()
{
  if (!m_bRenderCreated)
    return;

#ifdef HAS_GL
  glViewport(m_viewPort[0], m_viewPort[1], m_viewPort[2], m_viewPort[3]);
#endif

  glMatrixProject.PopLoad();
  glMatrixModview.PopLoad();
  glMatrixTexture.PopLoad();

#ifdef HAS_GL
  if (glActiveTextureARB)
    glActiveTextureARB(GL_TEXTURE0_ARB);
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#else
  glActiveTexture(GL_TEXTURE0);
#endif
  glEnable(GL_BLEND);
  glEnable(GL_SCISSOR_TEST);
#ifdef HAS_GLES
  glClear(GL_DEPTH_BUFFER_BIT);
#endif
}

void CRenderSystemGL::SetCameraPosition(const CPoint &camera, int screenWidth, int screenHeight, float stereoFactor)
{
  if (!m_bRenderCreated)
    return;

  CPoint offset = camera - CPoint(screenWidth*0.5f, screenHeight*0.5f);

  float w = (float)m_viewPort[2]*0.5f;
  float h = (float)m_viewPort[3]*0.5f;

  glMatrixModview->LoadIdentity();
  glMatrixModview->Translatef(-(w + offset.x - stereoFactor), +(h + offset.y), 0);
  glMatrixModview->LookAt(0.0, 0.0, -2.0*h, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0);
  glMatrixModview.Load();

  glMatrixProject->LoadIdentity();
  glMatrixProject->Translatef(-0.5, -0.5, 0.0);
  glMatrixProject->Rotatef(270.f * M_PI / 180.f, 0.f, 0.f, 1.f);
  glMatrixProject->Translatef(0.5, 0.5, 0.0);
  glMatrixProject->Frustum( (-w - offset.x)*0.5f, (w - offset.x)*0.5f, (-h + offset.y)*0.5f, (h + offset.y)*0.5f, h, 100*h);
  glMatrixProject->Translatef(-w, /*-h*/0, 0.f);
  glMatrixProject.Load();
}

void CRenderSystemGL::Project(float &x, float &y, float &z)
{
  GLfloat coordX, coordY, coordZ;
  if (CMatrixGL::Project(x, y, z, glMatrixModview.Get(), glMatrixProject.Get(), m_viewPort, &coordX, &coordY, &coordZ))
  {
    x = coordX;
    y = (float)(m_viewPort[1] + m_viewPort[3] - coordY);
    z = 0;
  }
}

bool CRenderSystemGL::TestRender()
{
  static float theta = 0.0;

#ifdef HAS_GL
  glPushMatrix();
  glRotatef( theta, 0.0f, 0.0f, 1.0f );
  glBegin( GL_TRIANGLES );
  glColor3f( 1.0f, 0.0f, 0.0f ); glVertex2f( 0.0f, 1.0f );
  glColor3f( 0.0f, 1.0f, 0.0f ); glVertex2f( 0.87f, -0.5f );
  glColor3f( 0.0f, 0.0f, 1.0f ); glVertex2f( -0.87f, -0.5f );
  glEnd();
  glPopMatrix();
#else
  //RESOLUTION_INFO resInfo = CDisplaySettings::GetInstance().GetCurrentResolutionInfo();
  //glViewport(0, 0, resInfo.iWidth, resInfo.iHeight);

  glMatrixModview.Push();
  glMatrixModview->Rotatef( theta, 0.0f, 0.0f, 1.0f );

  EnableGUIShader(SM_DEFAULT);

  GLfloat col[4] = {1.0f, 0.0f, 0.0f, 1.0f};
  GLfloat ver[3][2];
  GLint   posLoc = GUIShaderGetPos();
  GLint   colLoc = GUIShaderGetCol();

  glVertexAttribPointer(posLoc,  2, GL_FLOAT, 0, 0, ver);
  glVertexAttribPointer(colLoc,  4, GL_FLOAT, 0, 0, col);

  glEnableVertexAttribArray(posLoc);
  glEnableVertexAttribArray(colLoc);

  // Setup vertex position values
  ver[0][0] =  0.0f;
  ver[0][1] =  1.0f;
  ver[1][0] =  0.87f;
  ver[1][1] = -0.5f;
  ver[2][0] = -0.87f;
  ver[2][1] = -0.5f;

  glDrawArrays(GL_TRIANGLES, 0, 3);

  glDisableVertexAttribArray(posLoc);
  glDisableVertexAttribArray(colLoc);

  DisableGUIShader();

  glMatrixModview.Pop();
#endif

  theta += 1.0f;

  return true;
}

void CRenderSystemGL::ApplyHardwareTransform(const TransformMatrix &finalMatrix)
{
  if (!m_bRenderCreated)
    return;

  glMatrixModview.Push();
  GLfloat matrix[4][4];

  for(int i = 0; i < 3; i++)
    for(int j = 0; j < 4; j++)
      matrix[j][i] = finalMatrix.m[i][j];

  matrix[0][3] = 0.0f;
  matrix[1][3] = 0.0f;
  matrix[2][3] = 0.0f;
  matrix[3][3] = 1.0f;

  glMatrixModview->MultMatrixf(&matrix[0][0]);
  glMatrixModview.Load();
}

void CRenderSystemGL::RestoreHardwareTransform()
{
  if (!m_bRenderCreated)
    return;

  glMatrixModview.PopLoad();
}

void CRenderSystemGL::CalculateMaxTexturesize()
{
#ifdef HAS_GL
  GLint width = 256;

  // reset any previous GL errors
  ResetGLErrors();

  // max out at 2^(8+8)
  for (int i = 0 ; i<8 ; i++)
  {
    glTexImage2D(GL_PROXY_TEXTURE_2D, 0, 4, width, width, 0, GL_BGRA,
                 GL_UNSIGNED_BYTE, NULL);
    glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,
                             &width);

    // GMA950 on OS X sets error instead
    if (width == 0 || (glGetError() != GL_NO_ERROR) )
      break;

    m_maxTextureSize = width;
    width *= 2;
    if (width > 65536) // have an upper limit in case driver acts stupid
    {
      CLog::Log(LOGERROR, "GL: Could not determine maximum texture width, falling back to 2048");
      m_maxTextureSize = 2048;
      break;
    }
  }

#ifdef TARGET_DARWIN_OSX
  // Max Texture size reported on some apple machines seems incorrect
  // Displaying a picture with that resolution results in a corrupted output
  // So force it to a lower value
  // Problem noticed on:
  // iMac with ATI Radeon X1600, both on 10.5.8 (GL_VERSION: 2.0 ATI-1.5.48)
  // and 10.6.2 (GL_VERSION: 2.0 ATI-1.6.6)
  if (m_RenderRenderer == "ATI Radeon X1600 OpenGL Engine")
    m_maxTextureSize = 2048;
  // Mac mini G4 with ATI Radeon 9200 (GL_VERSION: 1.3 ATI-1.5.48)
  else if (m_RenderRenderer == "ATI Radeon 9200 OpenGL Engine")
    m_maxTextureSize = 1024;
#endif

#else
  // GLES cannot do PROXY textures to determine maximum size,
#endif

  CLog::Log(LOGINFO, "GL: Maximum texture width: %u", m_maxTextureSize);
}

void CRenderSystemGL::GetViewPort(CRect& viewPort)
{
  if (!m_bRenderCreated)
    return;

  viewPort.x1 = m_viewPort[0];
  viewPort.y1 = m_height - m_viewPort[1] - m_viewPort[3];
  viewPort.x2 = m_viewPort[0] + m_viewPort[2];
  viewPort.y2 = viewPort.y1 + m_viewPort[3];
}

void CRenderSystemGL::SetViewPort(CRect& viewPortorig)
{
  if (!m_bRenderCreated)
    return;

  CRect viewPort(viewPortorig.y1, viewPortorig.x1, viewPortorig.Height(), viewPortorig.Width());

  glScissor((GLint) viewPort.x1, (GLint) (m_width - viewPort.y1 - viewPort.Height()), (GLsizei) viewPort.Width(), (GLsizei) viewPort.Height());
  glViewport((GLint) viewPort.x1, (GLint) (m_width - viewPort.y1 - viewPort.Height()), (GLsizei) viewPort.Width(), (GLsizei) viewPort.Height());

  m_viewPort[0] = viewPortorig.x1;
  m_viewPort[1] = m_height - viewPortorig.y1 - viewPortorig.Height();
  m_viewPort[2] = viewPortorig.Width();
  m_viewPort[3] = viewPortorig.Height();
}

#ifdef HAS_GLES
bool CRenderSystemGL::ScissorsCanEffectClipping()
{
  if (m_pGUIshader[m_method])
    return m_pGUIshader[m_method]->HardwareClipIsPossible();

  return false;
}

CRect CRenderSystemGL::ClipRectToScissorRect(const CRect &rect)
{
  if (!m_pGUIshader[m_method])
    return CRect();
  float xFactor = m_pGUIshader[m_method]->GetClipXFactor();
  float xOffset = m_pGUIshader[m_method]->GetClipXOffset();
  float yFactor = m_pGUIshader[m_method]->GetClipYFactor();
  float yOffset = m_pGUIshader[m_method]->GetClipYOffset();
  return CRect(rect.x1 * xFactor + xOffset,
               rect.y1 * yFactor + yOffset,
               rect.x2 * xFactor + xOffset,
               rect.y2 * yFactor + yOffset);
}
#endif

void CRenderSystemGL::SetScissors(const CRect &rectorig)

{
  if (!m_bRenderCreated)
    return;

  CRect rect(rectorig.y1, rectorig.x1, rectorig.Height(), rectorig.Width());

  GLint x1 = MathUtils::round_int(rect.x1);
  GLint y1 = MathUtils::round_int(rect.y1);
  GLint x2 = MathUtils::round_int(rect.x2);
  GLint y2 = MathUtils::round_int(rect.y2);
  glScissor(x1, m_width - y2, x2-x1, y2-y1);
}

void CRenderSystemGL::ResetScissors()
{
  SetScissors(CRect(0, 0, (float)m_width, (float)m_height));
}

#ifdef HAS_GL

void CRenderSystemGL::GetGLSLVersion(int& major, int& minor)
{
  major = m_glslMajor;
  minor = m_glslMinor;
}


void CRenderSystemGL::ResetGLErrors()
{
  int count = 0;
  while (glGetError() != GL_NO_ERROR)
  {
    count++;
    if (count >= 100)
    {
      CLog::Log(LOGWARNING, "CRenderSystemGL::ResetGLErrors glGetError didn't return GL_NO_ERROR after %i iterations", count);
      break;
    }
  }
}
static const GLubyte stipple_3d[] = {
  0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00,
};

void CRenderSystemGL::SetStereoMode(RENDER_STEREO_MODE mode, RENDER_STEREO_VIEW view)
{
  CRenderSystemBase::SetStereoMode(mode, view);

  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDisable(GL_POLYGON_STIPPLE);
  glDrawBuffer(GL_BACK);

  if(m_stereoMode == RENDER_STEREO_MODE_ANAGLYPH_RED_CYAN)
  {
    if(m_stereoView == RENDER_STEREO_VIEW_LEFT)
      glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
    else if(m_stereoView == RENDER_STEREO_VIEW_RIGHT)
      glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
  }
  if(m_stereoMode == RENDER_STEREO_MODE_ANAGLYPH_GREEN_MAGENTA)
  {
    if(m_stereoView == RENDER_STEREO_VIEW_LEFT)
      glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE);
    else if(m_stereoView == RENDER_STEREO_VIEW_RIGHT)
      glColorMask(GL_TRUE, GL_FALSE, GL_TRUE, GL_TRUE);
  }
  if(m_stereoMode == RENDER_STEREO_MODE_ANAGLYPH_YELLOW_BLUE)
  {
    if(m_stereoView == RENDER_STEREO_VIEW_LEFT)
      glColorMask(GL_TRUE, GL_TRUE, GL_FALSE, GL_TRUE);
    else if(m_stereoView == RENDER_STEREO_VIEW_RIGHT)
      glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE);
  }

  if(m_stereoMode == RENDER_STEREO_MODE_INTERLACED)
  {
    glEnable(GL_POLYGON_STIPPLE);
    if(m_stereoView == RENDER_STEREO_VIEW_LEFT)
      glPolygonStipple(stipple_3d);
    else if(m_stereoView == RENDER_STEREO_VIEW_RIGHT)
      glPolygonStipple(stipple_3d+4);
  }

  if(m_stereoMode == RENDER_STEREO_MODE_HARDWAREBASED)
  {
    if(m_stereoView == RENDER_STEREO_VIEW_LEFT)
      glDrawBuffer(GL_BACK_LEFT);
    else if(m_stereoView == RENDER_STEREO_VIEW_RIGHT)
      glDrawBuffer(GL_BACK_RIGHT);
  }

}

#else

void CRenderSystemGL::InitialiseGUIShader()
{
  if (!m_pGUIshader)
  {
    m_pGUIshader = new CGUIShader*[SM_ESHADERCOUNT];
    for (int i = 0; i < SM_ESHADERCOUNT; i++)
    {
      if (i == SM_TEXTURE_RGBA_OES || i == SM_TEXTURE_RGBA_BOB_OES)
      {
        if (!g_Windowing.IsExtSupported("GL_OES_EGL_image_external"))
        {
          m_pGUIshader[i] = NULL;
          continue;
        }
      }

      m_pGUIshader[i] = new CGUIShader( ShaderNames[i] );

      if (!m_pGUIshader[i]->CompileAndLink())
      {
        m_pGUIshader[i]->Free();
        delete m_pGUIshader[i];
        m_pGUIshader[i] = NULL;
        CLog::Log(LOGERROR, "GUI Shader [%s] - Initialise failed", ShaderNames[i]);
      }
      else
      {
        CLog::Log(LOGDEBUG, "GUI Shader [%s]- Initialise successful : %p", ShaderNames[i], m_pGUIshader[i]);
      }
    }
  }
  else
  {
    CLog::Log(LOGDEBUG, "GUI Shader - Tried to Initialise again. Was this intentional?");
  }
}

void CRenderSystemGL::EnableGUIShader(ESHADERMETHOD method)
{
  m_method = method;
  if (m_pGUIshader[m_method])
  {
    m_pGUIshader[m_method]->Enable();
  }
  else
  {
    CLog::Log(LOGERROR, "Invalid GUI Shader selected - [%s]", ShaderNames[(int)method]);
  }
}

void CRenderSystemGL::DisableGUIShader()
{
  if (m_pGUIshader[m_method])
  {
    m_pGUIshader[m_method]->Disable();
  }
  m_method = SM_DEFAULT;
}

GLint CRenderSystemGL::GUIShaderGetPos()
{
  if (m_pGUIshader[m_method])
    return m_pGUIshader[m_method]->GetPosLoc();

  return -1;
}

GLint CRenderSystemGL::GUIShaderGetCol()
{
  if (m_pGUIshader[m_method])
    return m_pGUIshader[m_method]->GetColLoc();

  return -1;
}

GLint CRenderSystemGL::GUIShaderGetCoord0()
{
  if (m_pGUIshader[m_method])
    return m_pGUIshader[m_method]->GetCord0Loc();

  return -1;
}

GLint CRenderSystemGL::GUIShaderGetCoord1()
{
  if (m_pGUIshader[m_method])
    return m_pGUIshader[m_method]->GetCord1Loc();

  return -1;
}

GLint CRenderSystemGL::GUIShaderGetUniCol()
{
  if (m_pGUIshader[m_method])
    return m_pGUIshader[m_method]->GetUniColLoc();

  return -1;
}

GLint CRenderSystemGL::GUIShaderGetCoord0Matrix()
{
  if (m_pGUIshader[m_method])
    return m_pGUIshader[m_method]->GetCoord0MatrixLoc();

  return -1;
}

GLint CRenderSystemGL::GUIShaderGetField()
{
  if (m_pGUIshader[m_method])
    return m_pGUIshader[m_method]->GetFieldLoc();

  return -1;
}

GLint CRenderSystemGL::GUIShaderGetStep()
{
  if (m_pGUIshader[m_method])
    return m_pGUIshader[m_method]->GetStepLoc();

  return -1;
}

GLint CRenderSystemGL::GUIShaderGetContrast()
{
  if (m_pGUIshader[m_method])
    return m_pGUIshader[m_method]->GetContrastLoc();

  return -1;
}

GLint CRenderSystemGL::GUIShaderGetBrightness()
{
  if (m_pGUIshader[m_method])
    return m_pGUIshader[m_method]->GetBrightnessLoc();

  return -1;
}

#endif

bool CRenderSystemGL::SupportsStereo(RENDER_STEREO_MODE mode) const
{
  switch(mode)
  {
#ifdef HAS_GL
    case RENDER_STEREO_MODE_ANAGLYPH_RED_CYAN:
    case RENDER_STEREO_MODE_ANAGLYPH_GREEN_MAGENTA:
    case RENDER_STEREO_MODE_ANAGLYPH_YELLOW_BLUE:
    case RENDER_STEREO_MODE_INTERLACED:
      return true;
    case RENDER_STEREO_MODE_HARDWAREBASED: {
      //This is called by setting init, at which point GL is not inited
      //luckily if GL doesn't support this, it will just behave as if
      //it was not in effect.
      //GLboolean stereo = GL_FALSE;
      //glGetBooleanv(GL_STEREO, &stereo);
      //return stereo == GL_TRUE ? true : false;
      return true;
    }
#else
    case RENDER_STEREO_MODE_INTERLACED:
      if (g_sysinfo.HasHW3DInterlaced())
        return true;
#endif

    default:
      return CRenderSystemBase::SupportsStereo(mode);
  }
}

#ifdef HAS_GLES
GLint CRenderSystemGL::GUIShaderGetModel()
{
  if (m_pGUIshader[m_method])
    return m_pGUIshader[m_method]->GetModelLoc();

  return -1;
}
#endif

#endif
