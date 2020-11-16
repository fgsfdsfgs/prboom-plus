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

#ifndef _GL_OPENGL_H
#define _GL_OPENGL_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define USE_VERTEX_ARRAYS
//#define USE_VBO

#include <SDL.h>

#ifdef __vita__

#include <vitaGL/source/vitaGL.h>
#include <libtess/glu.h>

// some extension function types that we won't use
typedef void (*PFNGLCOLORTABLEEXTPROC) (GLenum target, GLenum internalFormat, GLsizei width, GLenum format, GLenum type, const GLvoid *table);
typedef void (*PFNGLACTIVETEXTUREARBPROC)(GLenum texture);
typedef void (*PFNGLCLIENTACTIVETEXTUREARBPROC) (GLenum texture);
typedef void (*PFNGLMULTITEXCOORD2FARBPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (*PFNGLMULTITEXCOORD2FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (*PFNGLBLENDCOLOREXTPROC) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (*PFNGLCOMPRESSEDTEXIMAGE2DARBPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (*PFNGLGENBUFFERSARBPROC) (GLsizei n, GLuint *buffers);
typedef void (*PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef void (*PFNGLBINDBUFFERARBPROC) (GLenum target, GLuint buffer);
typedef void (*PFNGLBUFFERDATAARBPROC) (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
typedef void (*PFNGLBUFFERSUBDATAARBPROC) (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
typedef void (*PFNGLGETBUFFERPARAMETERIVARBPROC) (GLenum target, GLenum pname, GLint *params);
typedef void *(*PFNGLMAPBUFFERARBPROC) (GLenum target, GLenum access);
typedef GLboolean (*PFNGLUNMAPBUFFERARBPROC) (GLenum target);

// don't have display lists

#define glGenLists(n) (0)
#define glNewList(i, m) do {} while (0)
#define glEndList() do {} while (0)
#define glDeleteLists(x, y) do {} while (0)
#define glCallList(n) do {} while (0)
#define glPixelStorei(x, y) do {} while (0)

// don't have these either

#define glFlush() do {} while (0)
#define glHint(x, y) do {} while (0)
#define glTexGenf(x, y, z) do {} while (0)
#define glTexGenfv(x, y, z) do {} while (0)
#define glShadeModel(x) do {} while (0)

// some missing constants

/* base */
#define GL_POLYGON_SMOOTH   0x0B41
#define GL_PACK_ALIGNMENT   0x0D05
#define GL_UNPACK_ALIGNMENT 0x0CF5
#define GL_SHADE_MODEL      0x0B54
#define GL_FLAT             0x1D00
#define GL_SMOOTH           0x1D01
#define GL_TEXTURE_GEN_S    0x0C60
#define GL_TEXTURE_GEN_T    0x0C61
#define GL_TEXTURE_GEN_R    0x0C62
#define GL_TEXTURE_GEN_Q    0x0C63
#define GL_GENERATE_MIPMAP  0x8191
#define GL_RGBA2            0x8055
#define GL_RGBA4            0x8056
#define GL_RGB5_A1          0x8057
#define GL_RGBA8            0x8058

/* GL_NV_depth_clamp */
#define GL_DEPTH_CLAMP_NV   0x864F

/* GL_EXT_texture_compression_s3tc */
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2

/* GL_EXT_texture_filter_anisotropic */
#define GL_TEXTURE_MAX_ANISOTROPY_EXT     0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

/* GL_EXT_shared_texture_palette */
#define GL_SHARED_TEXTURE_PALETTE_EXT 0x81FB

/* GL_EXT_texture_env_combine */
#define GL_COMBINE        0x8570
#define GL_COMBINE_RGB    0x8571
#define GL_COMBINE_ALPHA  0x8572
#define GL_RGB_SCALE      0x8573
#define GL_ADD_SIGNED     0x8574
#define GL_INTERPOLATE    0x8575
#define GL_CONSTANT       0x8576
#define GL_PRIMARY_COLOR  0x8577
#define GL_PREVIOUS       0x8578
#define GL_SOURCE0_RGB    0x8580
#define GL_SOURCE1_RGB    0x8581
#define GL_SOURCE2_RGB    0x8582
#define GL_SOURCE0_ALPHA  0x8588
#define GL_SOURCE1_ALPHA  0x8589
#define GL_SOURCE2_ALPHA  0x858A
#define GL_OPERAND0_RGB   0x8590
#define GL_OPERAND1_RGB   0x8591
#define GL_OPERAND2_RGB   0x8592
#define GL_OPERAND0_ALPHA 0x8598
#define GL_OPERAND1_ALPHA 0x8599
#define GL_OPERAND2_ALPHA 0x859A

/* GL_ARB_multisample */
#define GL_MULTISAMPLE_ARB 0x809D

/* GL_EXT_texture_env_dot3 */
#define GL_DOT3_RGB       0x8740
#define GL_DOT3_RGBA      0x8741

/* some ARB postfixed stuff */
#define GL_STATIC_DRAW_ARB GL_STATIC_DRAW
#define GL_TEXTURE0_ARB    GL_TEXTURE0
#define GL_TEXTURE1_ARB    GL_TEXTURE1
#define GL_TEXTURE31_ARB   GL_TEXTURE31

#define R_GL_MIPMAP_LINEAR_FILTER GL_LINEAR

#else // __vita__

#include <SDL_opengl.h>

#if SDL_VERSION_ATLEAST(1, 3, 0)
#if defined(__MACOSX__)
#include <OpenGL/gl.h>	/* Header File For The OpenGL Library */
#include <OpenGL/glu.h>	/* Header File For The GLU Library */
#elif defined(__MACOS__)
#include <gl.h>		/* Header File For The OpenGL Library */
#include <glu.h>	/* Header File For The GLU Library */
#else
#include <GL/gl.h>	/* Header File For The OpenGL Library */
#include <GL/glu.h>	/* Header File For The GLU Library */
#endif
#endif

#define R_GL_MIPMAP_LINEAR_FILTER GL_LINEAR_MIPMAP_LINEAR

#endif // __vita__

#include "doomtype.h"

#if !defined(GL_DEPTH_STENCIL_EXT)
#define GL_DEPTH_STENCIL_EXT              0x84F9
#endif

#define isExtensionSupported(ext) strstr(extensions, ext)

//e6y: OpenGL version
typedef enum {
  OPENGL_VERSION_1_0,
  OPENGL_VERSION_1_1,
  OPENGL_VERSION_1_2,
  OPENGL_VERSION_1_3,
  OPENGL_VERSION_1_4,
  OPENGL_VERSION_1_5,
  OPENGL_VERSION_2_0,
  OPENGL_VERSION_2_1,
} glversion_t;

extern int gl_version;

extern int GLEXT_CLAMP_TO_EDGE;
extern int gl_max_texture_size;

extern SDL_PixelFormat RGBAFormat;

// obsolete?
extern int gl_use_paletted_texture;
extern int gl_use_shared_texture_palette;
extern int gl_paletted_texture;
extern int gl_shared_texture_palette;

extern dboolean gl_ext_texture_filter_anisotropic;
extern dboolean gl_arb_texture_non_power_of_two;
extern dboolean gl_arb_multitexture;
extern dboolean gl_arb_texture_compression;
extern dboolean gl_ext_framebuffer_object;
extern dboolean gl_ext_packed_depth_stencil;
extern dboolean gl_ext_blend_color;
extern dboolean gl_use_stencil;
extern dboolean gl_ext_arb_vertex_buffer_object;
extern dboolean gl_arb_pixel_buffer_object;
extern dboolean gl_arb_shader_objects;

// obsolete?
extern PFNGLCOLORTABLEEXTPROC              GLEXT_glColorTableEXT;

#ifdef USE_FBO_TECHNIQUE
extern PFNGLBINDFRAMEBUFFEREXTPROC         GLEXT_glBindFramebufferEXT;
extern PFNGLGENFRAMEBUFFERSEXTPROC         GLEXT_glGenFramebuffersEXT;
extern PFNGLGENRENDERBUFFERSEXTPROC        GLEXT_glGenRenderbuffersEXT;
extern PFNGLBINDRENDERBUFFEREXTPROC        GLEXT_glBindRenderbufferEXT;
extern PFNGLRENDERBUFFERSTORAGEEXTPROC     GLEXT_glRenderbufferStorageEXT;
extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC GLEXT_glFramebufferRenderbufferEXT;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC    GLEXT_glFramebufferTexture2DEXT;
extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC  GLEXT_glCheckFramebufferStatusEXT;
extern PFNGLDELETEFRAMEBUFFERSEXTPROC      GLEXT_glDeleteFramebuffersEXT;
extern PFNGLDELETERENDERBUFFERSEXTPROC     GLEXT_glDeleteRenderbuffersEXT;
#endif

/* ARB_multitexture command function pointers */
extern PFNGLACTIVETEXTUREARBPROC           GLEXT_glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC     GLEXT_glClientActiveTextureARB;
extern PFNGLMULTITEXCOORD2FARBPROC         GLEXT_glMultiTexCoord2fARB;
extern PFNGLMULTITEXCOORD2FVARBPROC        GLEXT_glMultiTexCoord2fvARB;

extern PFNGLBLENDCOLOREXTPROC              GLEXT_glBlendColorEXT;

/* ARB_texture_compression */
extern PFNGLCOMPRESSEDTEXIMAGE2DARBPROC    GLEXT_glCompressedTexImage2DARB;

/* VBO */
extern PFNGLGENBUFFERSARBPROC              GLEXT_glGenBuffersARB;
extern PFNGLDELETEBUFFERSARBPROC           GLEXT_glDeleteBuffersARB;
extern PFNGLBINDBUFFERARBPROC              GLEXT_glBindBufferARB;
extern PFNGLBUFFERDATAARBPROC              GLEXT_glBufferDataARB;

/* PBO */
extern PFNGLBUFFERSUBDATAARBPROC           GLEXT_glBufferSubDataARB;
extern PFNGLGETBUFFERPARAMETERIVARBPROC    GLEXT_glGetBufferParameterivARB;
extern PFNGLMAPBUFFERARBPROC               GLEXT_glMapBufferARB;
extern PFNGLUNMAPBUFFERARBPROC             GLEXT_glUnmapBufferARB;

/* GL_ARB_shader_objects */
#ifdef USE_SHADERS
extern PFNGLDELETEOBJECTARBPROC            GLEXT_glDeleteObjectARB;
extern PFNGLGETHANDLEARBPROC               GLEXT_glGetHandleARB;
extern PFNGLDETACHOBJECTARBPROC            GLEXT_glDetachObjectARB;
extern PFNGLCREATESHADEROBJECTARBPROC      GLEXT_glCreateShaderObjectARB;
extern PFNGLSHADERSOURCEARBPROC            GLEXT_glShaderSourceARB;
extern PFNGLCOMPILESHADERARBPROC           GLEXT_glCompileShaderARB;
extern PFNGLCREATEPROGRAMOBJECTARBPROC     GLEXT_glCreateProgramObjectARB;
extern PFNGLATTACHOBJECTARBPROC            GLEXT_glAttachObjectARB;
extern PFNGLLINKPROGRAMARBPROC             GLEXT_glLinkProgramARB;
extern PFNGLUSEPROGRAMOBJECTARBPROC        GLEXT_glUseProgramObjectARB;
extern PFNGLVALIDATEPROGRAMARBPROC         GLEXT_glValidateProgramARB;

extern PFNGLUNIFORM1FARBPROC               GLEXT_glUniform1fARB;
extern PFNGLUNIFORM2FARBPROC               GLEXT_glUniform2fARB;
extern PFNGLUNIFORM1IARBPROC               GLEXT_glUniform1iARB;

extern PFNGLGETOBJECTPARAMETERFVARBPROC     GLEXT_glGetObjectParameterfvARB;
extern PFNGLGETOBJECTPARAMETERIVARBPROC     GLEXT_glGetObjectParameterivARB;
extern PFNGLGETINFOLOGARBPROC               GLEXT_glGetInfoLogARB;
extern PFNGLGETATTACHEDOBJECTSARBPROC       GLEXT_glGetAttachedObjectsARB;
extern PFNGLGETUNIFORMLOCATIONARBPROC       GLEXT_glGetUniformLocationARB;
extern PFNGLGETACTIVEUNIFORMARBPROC         GLEXT_glGetActiveUniformARB;
extern PFNGLGETUNIFORMFVARBPROC             GLEXT_glGetUniformfvARB;
#endif
  
void gld_InitOpenGL(dboolean compatibility_mode);

//states
void gld_EnableTexture2D(GLenum texture, int enable);
void gld_EnableClientCoordArray(GLenum texture, int enable);
void gld_EnableMultisample(int enable);

// wrappers
void gld_glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void gld_glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void gld_glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void gld_glEnableClientState(GLenum array);
void gld_glDisableClientState(GLenum array);
void gld_glDrawArrays(GLenum mode, GLint first, GLsizei count);

typedef enum
{
  TMF_MASKBIT = 1,
  TMF_OPAQUEBIT = 2,
  TMF_INVERTBIT = 4,

  TM_MODULATE = 0,
  TM_MASK = TMF_MASKBIT,
  TM_OPAQUE = TMF_OPAQUEBIT,
  TM_INVERT = TMF_INVERTBIT,
  //TM_INVERTMASK = TMF_MASKBIT | TMF_INVERTBIT
  TM_INVERTOPAQUE = TMF_INVERTBIT | TMF_OPAQUEBIT,
} tex_mode_e;
void SetTextureMode(tex_mode_e type);

#endif // _GL_OPENGL_H
