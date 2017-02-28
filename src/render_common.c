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

#include "render_common.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/keysym.h>

#include <GLES3/gl3ext.h>
#include <EGL/eglext.h>

/* EGL helpers */
EGLint egl_query_surface_int(const EglInfo egl, const EGLenum key) {
  EGLint value;
  eglQuerySurface(egl.display, egl.surface, key, &value);
  return value;
}

EGLint egl_get_config_attrib_int(const EglInfo egl, const EGLenum key) {
  EGLint value;
  eglGetConfigAttrib(egl.display, egl.config, key, &value);
  return value;
}

EGLint egl_query_context_int(const EglInfo egl, const EGLenum key) {
  EGLint value;
  eglQueryContext(egl.display, egl.context, key, &value);
  return value;
}

void egl_print_infos(const EglInfo egl) {
  char const *version = eglQueryString(egl.display, EGL_VERSION);
  char const *vendor = eglQueryString(egl.display, EGL_VENDOR);
  char const *extensions = eglQueryString(egl.display, EGL_EXTENSIONS);
  char const *apis = eglQueryString(egl.display, EGL_CLIENT_APIS);

  printf("EGL_VERSION = %s\n", version);
  printf("EGL_VENDOR = %s\n", vendor);
  printf("EGL_EXTENSIONS = %s\n", extensions);
  printf("EGL_CLIENT_APIS = %s\n", apis);
}

EGLDisplay egl_get_display(Display *display) {
  EGLDisplay egl_display = eglGetDisplay(display);
  if (!egl_display) {
    fprintf(stderr, "Error: eglGetDisplay() failed\n");
    exit(-1);
  }

  return egl_display;
}

void egl_init(EglInfo *egl) {
  if (!eglInitialize(egl->display, &egl->major, &egl->minor)) {
    printf("Error: eglInitialize() failed\n");
    exit(-2);
  }

  egl_print_infos(*egl);
}

EGLConfig egl_choose_config(EGLDisplay egl_dpy, const EGLint *attribs) {
  EGLConfig config;
  EGLint num_configs;

  if (!eglChooseConfig(egl_dpy, attribs, &config, 1, &num_configs)) {
    fprintf(stderr, "Error: couldn't get an EGL visual config\n");
    exit(1);
  }

  assert(config);
  assert(num_configs > 0);

  return config;
}

EGLConfig egl_create_context(EglInfo egl, const EGLint *attribs) {
  EGLContext ctx = eglCreateContext(egl.display, egl.config, EGL_NO_CONTEXT, attribs);
  if (!ctx) {
    fprintf(stderr, "Error: eglCreateContext failed\n");
    exit(1);
  }

  return ctx;
}

void egl_make_current(const EglInfo egl) {
  if (!eglMakeCurrent(egl.display, egl.surface, egl.surface, egl.context)) {
    fprintf(stderr, "Error: eglMakeCurrent() failed\n");
    exit(-1);
  }
}


EGLSurface egl_create_window_surface(EglInfo egl, Window win) {
  EGLSurface surface = eglCreateWindowSurface(egl.display, egl.config, win, NULL);
  if (!surface) {
    fprintf(stderr, "Error: eglCreateWindowSurface failed\n");
    exit(1);
  }
  return surface;
}

void egl_do_checks(const EglInfo egl, const window_size_t window_size) {
  EGLint egl_version = egl_query_context_int(egl, EGL_CONTEXT_CLIENT_VERSION);
  printf("Using EGL v %d\n", egl_version);

  EGLint surface_width = egl_query_surface_int(egl, EGL_WIDTH);
  EGLint surface_height = egl_query_surface_int(egl, EGL_HEIGHT);
  EGLint surface_type = egl_get_config_attrib_int(egl, EGL_SURFACE_TYPE);

  assert(surface_width == window_size.width);
  assert(surface_height == window_size.height);
  assert(surface_type & EGL_WINDOW_BIT);
}

/* GL helpers */
void gl_print_info() {
    printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
    printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
    printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
    printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
}

void reshape(window_size_t win_size) {
  glViewport(0, 0, (GLint)win_size.width, (GLint)win_size.height);
}


/* X helpers */
XVisualInfo *get_visual_info(Display *x_dpy, EGLint vid) {
  // The X window visual must match the EGL config
  int num_visuals;
  XVisualInfo visTemplate;
  visTemplate.visualid = vid;

  XVisualInfo *visInfo = XGetVisualInfo(x_dpy, VisualIDMask, &visTemplate, &num_visuals);
  if (!visInfo) {
    printf("Error: couldn't get X visual\n");
    exit(1);
  }

  return visInfo;
}

Window x_create_window(const EglInfo egl, Display *x_dpy, window_size_t win_size, const char *name) {
  EGLint vid = egl_get_config_attrib_int(egl, EGL_NATIVE_VISUAL_ID);
  XVisualInfo *visInfo = get_visual_info(x_dpy, vid);
  int scrnum = DefaultScreen(x_dpy);
  Window root = RootWindow(x_dpy, scrnum);

  /* window attributes */
  XSetWindowAttributes attr;
  attr.background_pixel = 0;
  attr.border_pixel = 0;
  attr.colormap = XCreateColormap(x_dpy, root, visInfo->visual, AllocNone);
  attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
  unsigned long mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

  Window win = XCreateWindow(x_dpy, root, 0, 0, win_size.width, win_size.height, 0, visInfo->depth, InputOutput, visInfo->visual, mask, &attr);

  XFree(visInfo);

  /* set hints and properties */
  {
    XSizeHints sizehints;
    sizehints.x = 0;
    sizehints.y = 0;
    sizehints.width = win_size.width;
    sizehints.height = win_size.height;
    sizehints.flags = USSize | USPosition;
    XSetNormalHints(x_dpy, win, &sizehints);
    XSetStandardProperties(x_dpy, win, name, name, None, (char **) NULL, 0, &sizehints);
  }

  return win;
}


Display *x_open_display(const char* dpyName) {
  Display *display = XOpenDisplay(dpyName);
  if (!display) {
    fprintf(stderr, "Error: couldn't open display %s\n", dpyName ? dpyName : getenv("DISPLAY"));
    exit(-1);
  }

  return display;
}

/* Render helpers */
void render_event_loop(RenderContext renderCtx, void *user_data) {
  static view_rotation_t view_rotation = { 0.0, 0.0 };

  while (1) {
    int redraw = 0;
    XEvent event;

    XNextEvent(renderCtx.X.display, &event);

    switch (event.type) {
    case Expose:
      redraw = 1;
      break;
    case ConfigureNotify: {
      window_size_t win_size = { event.xconfigure.width, event.xconfigure.height };
      reshape(win_size);
      break;
    }
    case KeyPress:
      {
        char buffer[10];
        int r, code;
        code = XLookupKeysym(&event.xkey, 0);
        if (code == XK_Left) {
          view_rotation.y += 5.0;
        } else if (code == XK_Right) {
          view_rotation.y -= 5.0;
        } else if (code == XK_Up) {
          view_rotation.x += 5.0;
        } else if (code == XK_Down) {
          view_rotation.x -= 5.0;
        } else {
          r = XLookupString(&event.xkey, buffer, sizeof(buffer), NULL, NULL);
          if (buffer[0] == 27) {
            /* escape */
            return;
          }
        }
      }
      redraw = 1;
      break;
    default:
      ; /*no-op*/
    }

    if (redraw || 1) {
      renderCtx.callbacks.draw(view_rotation, user_data);
      eglSwapBuffers(renderCtx.Egl.display, renderCtx.Egl.surface);
    }
  }
}


void render_create_context(RenderContext *renderCtx) {
  static const EGLint attribs[] = {
    EGL_RED_SIZE, 1,
    EGL_GREEN_SIZE, 1,
    EGL_BLUE_SIZE, 1,
    EGL_DEPTH_SIZE, 1,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
    EGL_NONE
  };

  static const EGLint ctx_attribs[] = {
    EGL_CONTEXT_CLIENT_VERSION, 3,
    EGL_CONTEXT_FLAGS_KHR, EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR,
    EGL_NONE
  };

  eglBindAPI(EGL_OPENGL_ES_API);

  renderCtx->Egl.config = egl_choose_config(renderCtx->Egl.display, attribs);
  renderCtx->X.window = x_create_window(renderCtx->Egl, renderCtx->X.display, renderCtx->window_size, "EGL - OpenGL ES 3.x");
  renderCtx->Egl.context = egl_create_context(renderCtx->Egl, ctx_attribs);
  renderCtx->Egl.surface = egl_create_window_surface(renderCtx->Egl, renderCtx->X.window);
  egl_do_checks(renderCtx->Egl, renderCtx->window_size);
}

void render_cleanup(RenderContext renderCtx) {
  eglDestroyContext(renderCtx.Egl.display, renderCtx.Egl.context);
  eglDestroySurface(renderCtx.Egl.display, renderCtx.Egl.surface);
  eglTerminate(renderCtx.Egl.display);

  XDestroyWindow(renderCtx.X.display, renderCtx.X.window);
  XCloseDisplay(renderCtx.X.display);
}

int render_main(int argc, char *argv[], RenderCallbacks callbacks) {
  RenderContext renderCtx;
  renderCtx.window_size.height = 300;
  renderCtx.window_size.width = 300;
  renderCtx.callbacks = callbacks;

  char *dpyName = NULL;
  GLboolean printInfo = GL_FALSE;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-display") == 0) {
      dpyName = argv[i + 1];
      i++;
    } else if (strcmp(argv[i], "-info") == 0) {
      printInfo = GL_TRUE;
    } else {
      printf("Usage:\n");
      printf("  -display <displayname>  set the display to run on\n");
      printf("  -info                   display OpenGL renderer info\n");
      exit(-1);
    }
  }

  renderCtx.X.display = x_open_display(dpyName);
  renderCtx.Egl.display = egl_get_display(renderCtx.X.display);

  egl_init(&renderCtx.Egl);
  render_create_context(&renderCtx);
  XMapWindow(renderCtx.X.display, renderCtx.X.window);

  egl_make_current(renderCtx.Egl);

  if (printInfo) {
    gl_print_info();
  }

  void *user_data;
  renderCtx.callbacks.initializer(renderCtx, &user_data);

  reshape(renderCtx.window_size);

  render_event_loop(renderCtx, user_data);
  render_cleanup(renderCtx);

  return 0;
}
