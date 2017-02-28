/* MIT License
 *
 * Copyright (c) 2017 Péter Gál
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef RENDER_COMMON_H
#define RENDER_COMMON_H

#include <EGL/egl.h>
#include <GLES3/gl31.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct {
  GLfloat x;
  GLfloat y;
} view_rotation_t;

typedef struct RenderContext RenderContext;

typedef void (*render_init_callback_t)(const RenderContext renderCtx, void** user_data);
typedef void (*render_draw_callback_t)(view_rotation_t rotation, void* user_data);

typedef struct {
  int width;
  int height;
} window_size_t;

typedef struct {
  EGLDisplay display;
  EGLSurface surface;
  EGLContext context;
  EGLConfig config;
  EGLint major;
  EGLint minor;
} EglInfo;

typedef struct {
  render_init_callback_t initializer;
  render_draw_callback_t draw;
} RenderCallbacks;

typedef struct RenderContext {
  window_size_t window_size;
  struct {
    Display* display;
    Window window;
  } X;
  EglInfo Egl;
  RenderCallbacks callbacks;
} RenderContext;


/* EGL helpers */
EGLint egl_query_surface_int(const EglInfo egl, const EGLenum key);
EGLint egl_get_config_attrib_int(const EglInfo egl, const EGLenum key);
EGLint egl_query_context_int(const EglInfo egl, const EGLenum key);
void egl_print_infos(const EglInfo egl);
EGLDisplay egl_get_display(Display *display);
void egl_init(EglInfo *egl);
EGLConfig egl_choose_config(EGLDisplay egl_dpy, const EGLint *attribs);
EGLConfig egl_create_context(EglInfo egl, const EGLint *attribs);
void egl_make_current(const EglInfo egl);
EGLSurface egl_create_window_surface(EglInfo egl, Window win);

void egl_do_checks(const EglInfo egl, const window_size_t window_size);

/* GL helpers */
void gl_print_info();

void reshape(window_size_t win_size);

/* X helpers */
XVisualInfo *get_visual_info(Display *x_dpy, EGLint vid);
Window x_create_window(const EglInfo egl, Display *x_dpy, window_size_t win_size, const char *name);
Display *x_open_display(const char* dpyName);

/* Render helpers */
void render_event_loop(RenderContext renderCtx, void *user_data);
void render_create_context(RenderContext *renderCtx);
void render_cleanup(RenderContext renderCtx);
int render_main(int argc, char *argv[], RenderCallbacks callbacks);

#endif /* RENDER_COMMON_H */
