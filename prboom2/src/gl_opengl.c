/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 *
 *  PrBoom: a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2000 by
 *  Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian Schulze
 *  Copyright 2005, 2006 by
 *  Florian Schulze, Colin Phipps, Neil Stevens, Andrey Budko
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 *  02111-1307, USA.
 *
 * DESCRIPTION:
 *   Thanks Roman "Vortex" Marchenko
 *---------------------------------------------------------------------
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include <SDL.h>
#include "gl_opengl.h"

#include "doomtype.h"
#include "lprintf.h"

int gl_version;

static dboolean gl_compatibility_mode;

int GLEXT_CLAMP_TO_EDGE = GL_CLAMP;
int gl_max_texture_size = 0;

SDL_PixelFormat RGBAFormat;

// obsolete?
int gl_use_paletted_texture = 0;
int gl_use_shared_texture_palette = 0;
int gl_paletted_texture = 0;
int gl_shared_texture_palette = 0;

dboolean gl_ext_texture_filter_anisotropic = false;
dboolean gl_arb_texture_non_power_of_two = false;
dboolean gl_arb_multitexture = false;
dboolean gl_arb_texture_compression = false;
dboolean gl_ext_framebuffer_object = false;
dboolean gl_ext_packed_depth_stencil = false;
dboolean gl_ext_blend_color = false;
dboolean gl_use_stencil = false;
dboolean gl_ext_arb_vertex_buffer_object = false;
dboolean gl_arb_pixel_buffer_object = false;
dboolean gl_arb_shader_objects = false;

// cfg values
int gl_ext_texture_filter_anisotropic_default;
int gl_arb_texture_non_power_of_two_default;
int gl_arb_multitexture_default;
int gl_arb_texture_compression_default;
int gl_ext_framebuffer_object_default;
int gl_ext_packed_depth_stencil_default;
int gl_ext_blend_color_default;
int gl_use_stencil_default;
int gl_ext_arb_vertex_buffer_object_default;
int gl_arb_pixel_buffer_object_default;
int gl_arb_shader_objects_default;

int active_texture_enabled[32];
int clieant_active_texture_enabled[32];

// obsolete?
PFNGLCOLORTABLEEXTPROC              GLEXT_glColorTableEXT              = NULL;

#ifdef USE_FBO_TECNIQUE
/* EXT_framebuffer_object */
PFNGLBINDFRAMEBUFFEREXTPROC         GLEXT_glBindFramebufferEXT         = NULL;
PFNGLGENFRAMEBUFFERSEXTPROC         GLEXT_glGenFramebuffersEXT         = NULL;
PFNGLGENRENDERBUFFERSEXTPROC        GLEXT_glGenRenderbuffersEXT        = NULL;
PFNGLBINDRENDERBUFFEREXTPROC        GLEXT_glBindRenderbufferEXT        = NULL;
PFNGLRENDERBUFFERSTORAGEEXTPROC     GLEXT_glRenderbufferStorageEXT     = NULL;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC GLEXT_glFramebufferRenderbufferEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC    GLEXT_glFramebufferTexture2DEXT    = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC  GLEXT_glCheckFramebufferStatusEXT  = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC      GLEXT_glDeleteFramebuffersEXT      = NULL;
PFNGLDELETERENDERBUFFERSEXTPROC     GLEXT_glDeleteRenderbuffersEXT     = NULL;
#endif

/* ARB_multitexture command function pointers */
PFNGLACTIVETEXTUREARBPROC        GLEXT_glActiveTextureARB              = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC  GLEXT_glClientActiveTextureARB        = NULL;
PFNGLMULTITEXCOORD2FARBPROC      GLEXT_glMultiTexCoord2fARB            = NULL;
PFNGLMULTITEXCOORD2FVARBPROC     GLEXT_glMultiTexCoord2fvARB           = NULL;

/* ARB_texture_compression */
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC GLEXT_glCompressedTexImage2DARB       = NULL;

PFNGLBLENDCOLOREXTPROC              GLEXT_glBlendColorEXT              = NULL;

/* VBO */
PFNGLGENBUFFERSARBPROC              GLEXT_glGenBuffersARB              = NULL;
PFNGLDELETEBUFFERSARBPROC           GLEXT_glDeleteBuffersARB           = NULL;
PFNGLBINDBUFFERARBPROC              GLEXT_glBindBufferARB              = NULL;
PFNGLBUFFERDATAARBPROC              GLEXT_glBufferDataARB              = NULL;

/* PBO */
PFNGLBUFFERSUBDATAARBPROC           GLEXT_glBufferSubDataARB           = NULL;
PFNGLGETBUFFERPARAMETERIVARBPROC    GLEXT_glGetBufferParameterivARB    = NULL;
PFNGLMAPBUFFERARBPROC               GLEXT_glMapBufferARB               = NULL;
PFNGLUNMAPBUFFERARBPROC             GLEXT_glUnmapBufferARB             = NULL;

/* GL_ARB_shader_objects */
#ifdef USE_SHADERS
PFNGLDELETEOBJECTARBPROC        GLEXT_glDeleteObjectARB = NULL;
PFNGLGETHANDLEARBPROC           GLEXT_glGetHandleARB = NULL;
PFNGLDETACHOBJECTARBPROC        GLEXT_glDetachObjectARB = NULL;
PFNGLCREATESHADEROBJECTARBPROC  GLEXT_glCreateShaderObjectARB = NULL;
PFNGLSHADERSOURCEARBPROC        GLEXT_glShaderSourceARB = NULL;
PFNGLCOMPILESHADERARBPROC       GLEXT_glCompileShaderARB = NULL;
PFNGLCREATEPROGRAMOBJECTARBPROC GLEXT_glCreateProgramObjectARB = NULL;
PFNGLATTACHOBJECTARBPROC        GLEXT_glAttachObjectARB = NULL;
PFNGLLINKPROGRAMARBPROC         GLEXT_glLinkProgramARB = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC    GLEXT_glUseProgramObjectARB = NULL;
PFNGLVALIDATEPROGRAMARBPROC     GLEXT_glValidateProgramARB = NULL;

PFNGLUNIFORM1FARBPROC           GLEXT_glUniform1fARB = NULL;
PFNGLUNIFORM2FARBPROC           GLEXT_glUniform2fARB = NULL;
PFNGLUNIFORM1IARBPROC           GLEXT_glUniform1iARB = NULL;

PFNGLGETOBJECTPARAMETERFVARBPROC GLEXT_glGetObjectParameterfvARB = NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC GLEXT_glGetObjectParameterivARB = NULL;
PFNGLGETINFOLOGARBPROC           GLEXT_glGetInfoLogARB = NULL;
PFNGLGETATTACHEDOBJECTSARBPROC   GLEXT_glGetAttachedObjectsARB = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC   GLEXT_glGetUniformLocationARB = NULL;
PFNGLGETACTIVEUNIFORMARBPROC     GLEXT_glGetActiveUniformARB = NULL;
PFNGLGETUNIFORMFVARBPROC         GLEXT_glGetUniformfvARB = NULL;
#endif

#ifdef __vita__
# define getProcAddress(name) (NULL)
#else
# define getProcAddress(name) SDL_GL_GetProcAddress(name)
#endif

void gld_InitOpenGLVersion(void)
{
#ifdef __vita__
  // close enough
  gl_version = OPENGL_VERSION_1_1;
#else
  int MajorVersion, MinorVersion;
  gl_version = OPENGL_VERSION_1_0;
  if (sscanf((const char*)glGetString(GL_VERSION), "%d.%d", &MajorVersion, &MinorVersion) == 2)
  {
    if (MajorVersion > 1)
    {
      gl_version = OPENGL_VERSION_2_0;
      if (MinorVersion > 0) gl_version = OPENGL_VERSION_2_1;
    }
    else
    {
      gl_version = OPENGL_VERSION_1_0;
      if (MinorVersion > 0) gl_version = OPENGL_VERSION_1_1;
      if (MinorVersion > 1) gl_version = OPENGL_VERSION_1_2;
      if (MinorVersion > 2) gl_version = OPENGL_VERSION_1_3;
      if (MinorVersion > 3) gl_version = OPENGL_VERSION_1_4;
      if (MinorVersion > 4) gl_version = OPENGL_VERSION_1_5;
    }
  }
#endif
}

#define isExtensionSupported(ext) strstr(extensions, ext)

void gld_InitOpenGL(dboolean compatibility_mode)
{
  GLenum texture;
  const char *extensions = (const char*)glGetString(GL_EXTENSIONS);

  gl_compatibility_mode = compatibility_mode;

  gld_InitOpenGLVersion();

  gl_ext_texture_filter_anisotropic = gl_ext_texture_filter_anisotropic_default &&
    isExtensionSupported("GL_EXT_texture_filter_anisotropic") != NULL;
  if (gl_ext_texture_filter_anisotropic)
    lprintf(LO_INFO, "using GL_EXT_texture_filter_anisotropic\n");

  // Any textures sizes are allowed
  gl_arb_texture_non_power_of_two = gl_arb_texture_non_power_of_two_default &&
    isExtensionSupported("GL_ARB_texture_non_power_of_two") != NULL;
  if (gl_arb_texture_non_power_of_two)
    lprintf(LO_INFO, "using GL_ARB_texture_non_power_of_two\n");

  // Paletted textures
  if (isExtensionSupported("GL_EXT_paletted_texture") != NULL)
  {
    if (gl_use_paletted_texture)
    {
      gl_paletted_texture = true;
      GLEXT_glColorTableEXT = getProcAddress("glColorTableEXT");
      if (GLEXT_glColorTableEXT == NULL)
        gl_paletted_texture = false;
      else
        lprintf(LO_INFO,"using GL_EXT_paletted_texture\n");
    }
  }
  else if (isExtensionSupported("GL_EXT_shared_texture_palette") != NULL)
  {
    if (gl_use_shared_texture_palette)
    {
      gl_shared_texture_palette = true;
      GLEXT_glColorTableEXT = getProcAddress("glColorTableEXT");
      if (GLEXT_glColorTableEXT == NULL)
        gl_shared_texture_palette = false;
      else
        lprintf(LO_INFO,"using GL_EXT_shared_texture_palette\n");
    }
  }

  //
  // ARB_multitexture command function pointers
  //

  gl_arb_multitexture = gl_arb_multitexture_default &&
    isExtensionSupported("GL_ARB_multitexture") != NULL;
  if (gl_arb_multitexture)
  {
    GLEXT_glActiveTextureARB        = getProcAddress("glActiveTextureARB");
    GLEXT_glClientActiveTextureARB  = getProcAddress("glClientActiveTextureARB");
    GLEXT_glMultiTexCoord2fARB      = getProcAddress("glMultiTexCoord2fARB");
    GLEXT_glMultiTexCoord2fvARB     = getProcAddress("glMultiTexCoord2fvARB");

    if (!GLEXT_glActiveTextureARB   || !GLEXT_glClientActiveTextureARB ||
        !GLEXT_glMultiTexCoord2fARB || !GLEXT_glMultiTexCoord2fvARB)
      gl_arb_multitexture = false;
  }
  if (gl_arb_multitexture)
    lprintf(LO_INFO,"using GL_ARB_multitexture\n");

#ifdef __vita__
  // just in case
  GLEXT_glActiveTextureARB = glActiveTexture;
  GLEXT_glClientActiveTextureARB = glClientActiveTexture;
#endif

  //
  // ARB_texture_compression
  //

  gl_arb_texture_compression = gl_arb_texture_compression_default &&
    isExtensionSupported("GL_ARB_texture_compression") != NULL;
  if (gl_arb_texture_compression)
  {
    GLEXT_glCompressedTexImage2DARB = getProcAddress("glCompressedTexImage2DARB");

    if (!GLEXT_glCompressedTexImage2DARB)
      gl_arb_texture_compression = false;
  }
  if (gl_arb_texture_compression)
    lprintf(LO_INFO,"using GL_ARB_texture_compression\n");

#ifdef USE_FBO_TECNIQUE
  //
  // EXT_framebuffer_object
  //
  gl_ext_framebuffer_object = gl_ext_framebuffer_object_default &&
    isExtensionSupported("GL_EXT_framebuffer_object") != NULL;
  if (gl_ext_framebuffer_object)
  {
    GLEXT_glGenFramebuffersEXT         = getProcAddress("glGenFramebuffersEXT");
    GLEXT_glBindFramebufferEXT         = getProcAddress("glBindFramebufferEXT");
    GLEXT_glGenRenderbuffersEXT        = getProcAddress("glGenRenderbuffersEXT");
    GLEXT_glBindRenderbufferEXT        = getProcAddress("glBindRenderbufferEXT");
    GLEXT_glRenderbufferStorageEXT     = getProcAddress("glRenderbufferStorageEXT");
    GLEXT_glFramebufferRenderbufferEXT = getProcAddress("glFramebufferRenderbufferEXT");
    GLEXT_glFramebufferTexture2DEXT    = getProcAddress("glFramebufferTexture2DEXT");
    GLEXT_glCheckFramebufferStatusEXT  = getProcAddress("glCheckFramebufferStatusEXT");
    GLEXT_glDeleteFramebuffersEXT      = getProcAddress("glDeleteFramebuffersEXT");
    GLEXT_glDeleteRenderbuffersEXT     = getProcAddress("glDeleteRenderbuffersEXT");

    if (!GLEXT_glGenFramebuffersEXT || !GLEXT_glBindFramebufferEXT ||
        !GLEXT_glGenRenderbuffersEXT || !GLEXT_glBindRenderbufferEXT ||
        !GLEXT_glRenderbufferStorageEXT || !GLEXT_glFramebufferRenderbufferEXT ||
        !GLEXT_glFramebufferTexture2DEXT || !GLEXT_glCheckFramebufferStatusEXT ||
        !GLEXT_glDeleteFramebuffersEXT || !GLEXT_glDeleteRenderbuffersEXT)
      gl_ext_framebuffer_object = false;
  }
  if (gl_ext_framebuffer_object)
    lprintf(LO_INFO,"using GL_EXT_framebuffer_object\n");
#endif

  gl_ext_packed_depth_stencil = gl_ext_packed_depth_stencil_default &&
    isExtensionSupported("GL_EXT_packed_depth_stencil") != NULL;
  if (gl_ext_packed_depth_stencil)
    lprintf(LO_INFO,"using GL_EXT_packed_depth_stencil\n");

  //
  // Blending
  //

  gl_ext_blend_color = gl_ext_blend_color_default &&
    isExtensionSupported("GL_EXT_blend_color") != NULL;
  if (gl_ext_blend_color)
  {
    GLEXT_glBlendColorEXT = getProcAddress("glBlendColorEXT");

    if (!GLEXT_glBlendColorEXT)
      gl_ext_blend_color = false;
  }
  if (gl_ext_blend_color)
    lprintf(LO_INFO,"using GL_EXT_blend_color\n");

  // VBO
#ifdef USE_VBO
  gl_ext_arb_vertex_buffer_object = gl_ext_arb_vertex_buffer_object_default &&
    isExtensionSupported("GL_ARB_vertex_buffer_object") != NULL;
  if (gl_ext_arb_vertex_buffer_object)
  {
    GLEXT_glGenBuffersARB = getProcAddress("glGenBuffersARB");
    GLEXT_glDeleteBuffersARB = getProcAddress("glDeleteBuffersARB");
    GLEXT_glBindBufferARB = getProcAddress("glBindBufferARB");
    GLEXT_glBufferDataARB = getProcAddress("glBufferDataARB");

    if (!GLEXT_glGenBuffersARB || !GLEXT_glDeleteBuffersARB ||
        !GLEXT_glBindBufferARB || !GLEXT_glBufferDataARB)
      gl_ext_arb_vertex_buffer_object = false;
  }
  if (gl_ext_arb_vertex_buffer_object)
    lprintf(LO_INFO,"using GL_ARB_vertex_buffer_object\n");
#else
  gl_ext_arb_vertex_buffer_object = false;
#endif

  gl_arb_pixel_buffer_object = gl_arb_pixel_buffer_object_default &&
    isExtensionSupported("GL_ARB_pixel_buffer_object") != NULL;
  if (gl_arb_pixel_buffer_object)
  {
    GLEXT_glGenBuffersARB = getProcAddress("glGenBuffersARB");
    GLEXT_glBindBufferARB = getProcAddress("glBindBufferARB");
    GLEXT_glBufferDataARB = getProcAddress("glBufferDataARB");
    GLEXT_glBufferSubDataARB = getProcAddress("glBufferSubDataARB");
    GLEXT_glDeleteBuffersARB = getProcAddress("glDeleteBuffersARB");
    GLEXT_glGetBufferParameterivARB = getProcAddress("glGetBufferParameterivARB");
    GLEXT_glMapBufferARB = getProcAddress("glMapBufferARB");
    GLEXT_glUnmapBufferARB = getProcAddress("glUnmapBufferARB");

    if (!GLEXT_glGenBuffersARB || !GLEXT_glBindBufferARB ||
        !GLEXT_glBufferDataARB || !GLEXT_glBufferSubDataARB ||
        !GLEXT_glDeleteBuffersARB || !GLEXT_glGetBufferParameterivARB ||
        !GLEXT_glMapBufferARB || !GLEXT_glUnmapBufferARB)
      gl_arb_pixel_buffer_object = false;
  }
  if (gl_arb_pixel_buffer_object)
    lprintf(LO_INFO,"using GL_ARB_pixel_buffer_object\n");

  //
  // Stencil support
  //

  gl_use_stencil = gl_use_stencil_default;

  //
  // GL_ARB_shader_objects
  //
#ifdef USE_SHADERS
  gl_arb_shader_objects = gl_arb_shader_objects_default &&
    (gl_version >= OPENGL_VERSION_2_0) &&
    isExtensionSupported ("GL_ARB_shader_objects") &&
    isExtensionSupported ("GL_ARB_vertex_shader") &&
    isExtensionSupported ("GL_ARB_fragment_shader") &&
    isExtensionSupported ("GL_ARB_shading_language_100");
  if (gl_arb_shader_objects)
  {
		GLEXT_glDeleteObjectARB        = getProcAddress("glDeleteObjectARB");
		GLEXT_glGetHandleARB           = getProcAddress("glGetHandleARB");
		GLEXT_glDetachObjectARB        = getProcAddress("glDetachObjectARB");
		GLEXT_glCreateShaderObjectARB  = getProcAddress("glCreateShaderObjectARB");
		GLEXT_glShaderSourceARB        = getProcAddress("glShaderSourceARB");
		GLEXT_glCompileShaderARB       = getProcAddress("glCompileShaderARB");
		GLEXT_glCreateProgramObjectARB = getProcAddress("glCreateProgramObjectARB");
		GLEXT_glAttachObjectARB        = getProcAddress("glAttachObjectARB");
		GLEXT_glLinkProgramARB         = getProcAddress("glLinkProgramARB");
		GLEXT_glUseProgramObjectARB    = getProcAddress("glUseProgramObjectARB");
		GLEXT_glValidateProgramARB     = getProcAddress("glValidateProgramARB");

		GLEXT_glUniform1fARB = getProcAddress("glUniform1fARB");
		GLEXT_glUniform2fARB = getProcAddress("glUniform2fARB");
		GLEXT_glUniform1iARB = getProcAddress("glUniform1iARB");

		GLEXT_glGetObjectParameterfvARB = getProcAddress("glGetObjectParameterfvARB");
		GLEXT_glGetObjectParameterivARB = getProcAddress("glGetObjectParameterivARB");
		GLEXT_glGetInfoLogARB           = getProcAddress("glGetInfoLogARB");
		GLEXT_glGetAttachedObjectsARB   = getProcAddress("glGetAttachedObjectsARB");
		GLEXT_glGetUniformLocationARB   = getProcAddress("glGetUniformLocationARB");
		GLEXT_glGetActiveUniformARB     = getProcAddress("glGetActiveUniformARB");
		GLEXT_glGetUniformfvARB         = getProcAddress("glGetUniformfvARB");

    if (!GLEXT_glDeleteObjectARB || !GLEXT_glGetHandleARB ||
        !GLEXT_glDetachObjectARB || !GLEXT_glCreateShaderObjectARB ||
        !GLEXT_glShaderSourceARB || !GLEXT_glCompileShaderARB ||
        !GLEXT_glCreateProgramObjectARB || !GLEXT_glAttachObjectARB ||
        !GLEXT_glLinkProgramARB || !GLEXT_glUseProgramObjectARB ||
        !GLEXT_glValidateProgramARB ||
        !GLEXT_glUniform1fARB || !GLEXT_glUniform2fARB ||
        !GLEXT_glUniform1iARB ||
        !GLEXT_glGetObjectParameterfvARB || !GLEXT_glGetObjectParameterivARB ||
        !GLEXT_glGetInfoLogARB || !GLEXT_glGetAttachedObjectsARB ||
        !GLEXT_glGetUniformLocationARB || !GLEXT_glGetActiveUniformARB ||
        !GLEXT_glGetUniformfvARB)
      gl_arb_shader_objects = false;
  }
  if (gl_arb_shader_objects)
  {
    lprintf(LO_INFO,"using GL_ARB_shader_objects\n");
    lprintf(LO_INFO,"using GL_ARB_vertex_shader\n");
    lprintf(LO_INFO,"using GL_ARB_fragment_shader\n");
    lprintf(LO_INFO,"using GL_ARB_shading_language_100\n");
  }
#else
  gl_arb_shader_objects = false;
#endif

  // GL_CLAMP_TO_EDGE
  GLEXT_CLAMP_TO_EDGE = (gl_version >= OPENGL_VERSION_1_2 ? GL_CLAMP_TO_EDGE : GL_CLAMP);

  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &gl_max_texture_size);
  lprintf(LO_INFO,"GL_MAX_TEXTURE_SIZE=%i\n", gl_max_texture_size);

  // Additional checks
  if (gl_version < OPENGL_VERSION_1_3)
  {
    gl_ext_framebuffer_object = false;
    gl_ext_blend_color = false;
  }

  if ((compatibility_mode) || (gl_version <= OPENGL_VERSION_1_1))
  {
    lprintf(LO_INFO, "gld_InitOpenGL: Compatibility mode is used.\n");
    gl_arb_texture_non_power_of_two = false;
    gl_arb_multitexture = false;
    gl_arb_texture_compression = false;
    gl_ext_framebuffer_object = false;
    gl_ext_packed_depth_stencil = false;
    gl_ext_blend_color = false;
    gl_use_stencil = false;
    gl_ext_arb_vertex_buffer_object = false;
    gl_arb_pixel_buffer_object = false;
    gl_arb_shader_objects = false;
    GLEXT_CLAMP_TO_EDGE = GL_CLAMP;
    gl_version = OPENGL_VERSION_1_1;
  }

#ifdef __vita__
  // wrong some rights
  GLEXT_CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE;
  gl_arb_texture_non_power_of_two = true;
  gl_arb_texture_compression = true;
  // gl_use_stencil = true;
  GLEXT_glCompressedTexImage2DARB = glCompressedTexImage2D;
#endif

  //init states manager
  gld_EnableMultisample(true);
  gld_EnableMultisample(false);

  for (texture = GL_TEXTURE0_ARB; texture <= GL_TEXTURE31_ARB; texture++)
  {
    gld_EnableTexture2D(texture, true);
    gld_EnableTexture2D(texture, false);

    gld_EnableClientCoordArray(texture, true);
    gld_EnableClientCoordArray(texture, false);
  }

  //init global variables
  RGBAFormat.palette = 0;
  RGBAFormat.BitsPerPixel = 32;
  RGBAFormat.BytesPerPixel = 4;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  RGBAFormat.Rmask = 0xFF000000; RGBAFormat.Rshift = 0; RGBAFormat.Rloss = 0;
  RGBAFormat.Gmask = 0x00FF0000; RGBAFormat.Gshift = 8; RGBAFormat.Gloss = 0;
  RGBAFormat.Bmask = 0x0000FF00; RGBAFormat.Bshift = 16; RGBAFormat.Bloss = 0;
  RGBAFormat.Amask = 0x000000FF; RGBAFormat.Ashift = 24; RGBAFormat.Aloss = 0;
#else
  RGBAFormat.Rmask = 0x000000FF; RGBAFormat.Rshift = 24; RGBAFormat.Rloss = 0;
  RGBAFormat.Gmask = 0x0000FF00; RGBAFormat.Gshift = 16; RGBAFormat.Gloss = 0;
  RGBAFormat.Bmask = 0x00FF0000; RGBAFormat.Bshift = 8; RGBAFormat.Bloss = 0;
  RGBAFormat.Amask = 0xFF000000; RGBAFormat.Ashift = 0; RGBAFormat.Aloss = 0;
#endif
}

void gld_EnableTexture2D(GLenum texture, int enable)
{
  int arb;

  if (!gl_arb_multitexture && texture != GL_TEXTURE0_ARB)
    return;

  arb = texture - GL_TEXTURE0_ARB;

#ifdef RANGECHECK
  if (arb < 0 || arb > 31)
    I_Error("gld_EnableTexture2D: wronge ARB texture unit %d", arb);
#endif

  if (enable)
  {
    if (!active_texture_enabled[arb])
    {
      if (arb != 0)
      {
        GLEXT_glActiveTextureARB(texture);
        glEnable(GL_TEXTURE_2D);
        GLEXT_glActiveTextureARB(GL_TEXTURE0_ARB);
      }
      else
      {
        glEnable(GL_TEXTURE_2D);
      }
      active_texture_enabled[arb] = enable;
    }
  }
  else
  {
    if (active_texture_enabled[arb])
    {
      if (arb != 0)
      {
        GLEXT_glActiveTextureARB(texture);
        glDisable(GL_TEXTURE_2D);
        GLEXT_glActiveTextureARB(GL_TEXTURE0_ARB);
      }
      else
      {
        glDisable(GL_TEXTURE_2D);
      }
      active_texture_enabled[arb] = enable;
    }
  }
}

void gld_EnableClientCoordArray(GLenum texture, int enable)
{
#ifdef USE_VERTEX_ARRAYS
  int arb;

  if (!gl_arb_multitexture)
    return;

  arb = texture - GL_TEXTURE0_ARB;

#ifdef RANGECHECK
  if (arb < 0 || arb > 31)
    I_Error("gld_EnableTexture2D: wronge ARB texture unit %d", arb);
#endif

  if (enable)
  {
    if (!clieant_active_texture_enabled[arb])
    {
      GLEXT_glClientActiveTextureARB(texture);
      gld_glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      GLEXT_glClientActiveTextureARB(GL_TEXTURE0_ARB);

      clieant_active_texture_enabled[arb] = enable;
    }
  }
  else
  {
    if (clieant_active_texture_enabled[arb])
    {
      GLEXT_glClientActiveTextureARB(texture);
      gld_glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      GLEXT_glClientActiveTextureARB(GL_TEXTURE0_ARB);

      clieant_active_texture_enabled[arb] = enable;
    }
  }
#endif
}

void gld_EnableMultisample(int enable)
{
  static int multisample_is_enabled = 0;
  if (enable)
  {
    if (!multisample_is_enabled)
    {
      glEnable(GL_MULTISAMPLE_ARB);

      multisample_is_enabled = enable;
    }
  }
  else
  {
    if (multisample_is_enabled)
    {
      glDisable(GL_MULTISAMPLE_ARB);

      multisample_is_enabled = enable;
    }
  }
}

void SetTextureMode(tex_mode_e type)
{
  if (gl_compatibility_mode)
  {
    type = TM_MODULATE;
  }

  if (type == TM_MASK)
  {
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PRIMARY_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);

    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE); 
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PRIMARY_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, GL_TEXTURE0);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
  }
  else if (type == TM_OPAQUE)
  {
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE0);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PRIMARY_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);

    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE); 
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PRIMARY_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
  }
  else if (type == TM_INVERT)
  {
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE0);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PRIMARY_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_ONE_MINUS_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);

    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE); 
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PRIMARY_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, GL_TEXTURE0);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
  }
  else if (type == TM_INVERTOPAQUE)
  {
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE0);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PRIMARY_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_ONE_MINUS_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);

    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE); 
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PRIMARY_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
  }
  else // if (type == TM_MODULATE)
  {
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  }
}

#ifdef __vita__

#define MAX_VERTICES 16384

static unsigned short *vtx_idx;
static float *vtx_pos, *vtx_posptr, *vtx_posstart;
static float *vtx_tex, *vtx_texptr, *vtx_texstart;
static float *vtx_col, *vtx_colptr, *vtx_colstart;
static float *vtx_tmp, *vtx_tmpptr, *vtx_tmpstart;
static float vtx_curcol[4] = { 1.f, 1.f, 1.f, 1.f };
static int vtx_num = 0;
static GLenum vtx_curprim = 0;

void gld_ResetWrapper(void)
{
  vtx_posptr = vtx_posstart = vtx_pos;
  vtx_texptr = vtx_texstart = vtx_tex;
  vtx_colptr = vtx_colstart = vtx_col;
  vtx_tmpptr = vtx_tmpstart = vtx_tmp;
}

void gld_glBegin(GLenum prim)
{
  if (vtx_pos == NULL)
  {
    unsigned int i;
    vtx_pos = malloc(sizeof(float) * 3 * MAX_VERTICES);
    vtx_tex = malloc(sizeof(float) * 2 * MAX_VERTICES);
    vtx_col = malloc(sizeof(float) * 4 * MAX_VERTICES);
    vtx_tmp = malloc(sizeof(float) * 4 * MAX_VERTICES);
    vtx_idx = malloc(sizeof(short) * 1 * MAX_VERTICES);
    if (!vtx_pos || !vtx_tex || !vtx_col || !vtx_idx || !vtx_tmp)
      I_Error("could not allocate vertex buffers");
    for (i = 0; i < MAX_VERTICES; ++i)
      vtx_idx[i] = i; // indices are always sequential here
    gld_ResetWrapper();
  }

  vtx_num = 0;
  vtx_curprim = prim;
}

void gld_glVertex2f(float x, float y) { gld_glVertex3f(x, y, 0.f); }
void gld_glVertex2i(int x, int y) { gld_glVertex3f(x, y, 0.f); }
void gld_glVertex3fv(const float *v) { gld_glVertex3f(v[0], v[1], v[2]); }
void gld_glVertex3f(float x, float y, float z)
{
  *(vtx_colptr++) = vtx_curcol[0];
  *(vtx_colptr++) = vtx_curcol[1];
  *(vtx_colptr++) = vtx_curcol[2];
  *(vtx_colptr++) = vtx_curcol[3];
  *(vtx_posptr++) = x;
  *(vtx_posptr++) = y;
  *(vtx_posptr++) = z;
  vtx_num++;
}

void gld_glTexCoord2fv(const float *v) { gld_glTexCoord2f(v[0], v[1]); }
void gld_glTexCoord2f(float u, float v)
{
  *(vtx_texptr++) = u;
  *(vtx_texptr++) = v;
}

void gld_glColor3f(float r, float g, float b) { gld_glColor4f(r, g, b, 1.f); }
void gld_glColor4fv(const float *v) { gld_glColor4f(v[0], v[1], v[2], v[3]); }
void gld_glColor4ubv(const unsigned char *v) { gld_glColor4f(v[0] / 255.f, v[1] / 255.f, v[2] / 255.f, v[3] / 255.f); }
void gld_glColor4f(float r, float g, float b, float a)
{
  vtx_curcol[0] = r;
  vtx_curcol[1] = g;
  vtx_curcol[2] = b;
  vtx_curcol[3] = a;
}

void gld_glEnd(void)
{
  if (!vtx_num || !vtx_curprim)
    return;

  vglIndexPointerMapped(vtx_idx);
  glEnableClientState(GL_VERTEX_ARRAY);
  vglVertexPointerMapped(vtx_posstart);
  glEnableClientState(GL_COLOR_ARRAY);
  vglColorPointerMapped(GL_FLOAT, vtx_colstart);
  if (vtx_texptr != vtx_texstart)
  {
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    vglTexCoordPointerMapped(vtx_texstart);
  }
  else
  {
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }

  vglDrawObjects(vtx_curprim, vtx_num, GL_TRUE);

  if (vtx_texptr != vtx_texstart)
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);

  vtx_curprim = 0;
  vtx_num = 0;
  vtx_posstart = vtx_posptr;
  vtx_colstart = vtx_colptr;
  vtx_texstart = vtx_texptr;
}

// custom array drawing functions

static struct VtxArray
{
  GLboolean enabled;
  GLint size;
  GLenum type;
  GLsizei stride;
  const GLchar *ptr;
} vtx_arrays[3];

void gld_glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
  vtx_arrays[0].size = size;
  vtx_arrays[0].type = type;
  vtx_arrays[0].stride = stride;
  vtx_arrays[0].ptr = pointer;
}

void gld_glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
  vtx_arrays[1].size = size;
  vtx_arrays[1].type = type;
  vtx_arrays[1].stride = stride;
  vtx_arrays[1].ptr = pointer;
}

void gld_glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
  vtx_arrays[2].size = size;
  vtx_arrays[2].type = type;
  vtx_arrays[2].stride = stride;
  vtx_arrays[2].ptr = pointer;
}

void gld_glEnableClientState(GLenum array)
{
  switch (array) {
    case GL_VERTEX_ARRAY:        vtx_arrays[0].enabled = GL_TRUE; break;
    case GL_COLOR_ARRAY:         vtx_arrays[1].enabled = GL_TRUE; break;
    case GL_TEXTURE_COORD_ARRAY: vtx_arrays[2].enabled = GL_TRUE; break;
    default:                     break;
  }
}

void gld_glDisableClientState(GLenum array)
{
  switch (array) {
    case GL_VERTEX_ARRAY:        vtx_arrays[0].enabled = GL_FALSE; break;
    case GL_COLOR_ARRAY:         vtx_arrays[1].enabled = GL_FALSE; break;
    case GL_TEXTURE_COORD_ARRAY: vtx_arrays[2].enabled = GL_FALSE; break;
    default:                     break;
  }
}

void gld_glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  int i;

  vglIndexPointerMapped(vtx_idx);

  // bind colors if enabled, otherwise fill with current color and enable anyway
  glEnableClientState(GL_COLOR_ARRAY);
  if (vtx_arrays[1].enabled)
  {
    vglColorPointer(vtx_arrays[1].size, vtx_arrays[1].type, vtx_arrays[1].stride,
      count, vtx_arrays[1].ptr + first * vtx_arrays[1].stride);
  }
  else
  {
    for (i = 0; i < count; ++i)
    {
      *(vtx_tmpptr++) = vtx_curcol[0];
      *(vtx_tmpptr++) = vtx_curcol[1];
      *(vtx_tmpptr++) = vtx_curcol[2];
      *(vtx_tmpptr++) = vtx_curcol[3];
    }
    vglColorPointerMapped(GL_FLOAT, vtx_tmpstart);
    vtx_tmpstart = vtx_tmpptr;
  }

  if (vtx_arrays[2].enabled)
  {
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    vglTexCoordPointer(vtx_arrays[2].size, vtx_arrays[2].type, vtx_arrays[2].stride,
      count, vtx_arrays[2].ptr + first * vtx_arrays[2].stride);
  }
  else
  {
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }

  // bind vertices and draw
  if (vtx_arrays[0].enabled)
  {
    glEnableClientState(GL_VERTEX_ARRAY);
    vglVertexPointer(vtx_arrays[0].size, vtx_arrays[0].type, vtx_arrays[0].stride,
      count, vtx_arrays[0].ptr + first * vtx_arrays[0].stride);
    vglDrawObjects(mode, count, GL_TRUE);
    glDisableClientState(GL_VERTEX_ARRAY);
  }

  if (vtx_arrays[2].enabled)
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
}

#else

void gld_ResetWrapper(void) { }
void gld_glBegin(GLenum prim) { glBegin(prim); }
void gld_glVertex2f(float x, float y) { glVertex2f(x, y); }
void gld_glVertex2i(int x, int y) { glVertex2i(x, y); }
void gld_glVertex3f(float x, float y, float z) { glVertex3f(x, y, z); }
void gld_glVertex3fv(const float *v) { glVertex3fv(v); }
void gld_glTexCoord2f(float u, float v) { glTexCoord2f(u, v); }
void gld_glTexCoord2fv(const float *v) { glTexCoord2fv(v); }
void gld_glColor3f(float r, float g, float b) { glColor3f(r, g, b); }
void gld_glColor4f(float r, float g, float b, float a) { glColor4f(r, g, b, a); }
void gld_glColor4fv(const float *v) { glColor4fv(v); }
void gld_glColor4ubv(const unsigned char *v) { glColor4ubv(v); }
void gld_glEnd(void) { glEnd(); }
void gld_glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) { glVertexPointer(size, type, stride, pointer); }
void gld_glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) { glColorPointer(size, type, stride, pointer); }
void gld_glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) { glTexCoordPointer(size, type, stride, pointer); }
void gld_glEnableClientState(GLenum array) { glEnableClientState(array); }
void gld_glDisableClientState(GLenum array) { glDisableClientState(array); }
void gld_glDrawArrays(GLenum mode, GLint first, GLsizei count) { glDrawArrays(mode, first, count); }

#endif

