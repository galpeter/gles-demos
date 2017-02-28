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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "shaders.h"

static const char* shader_type_str(GLenum type) {
  switch(type) {
    case GL_FRAGMENT_SHADER: return "Fragment";
    case GL_VERTEX_SHADER: return "Vertex";
    default:
      return "<Unknown Shader type>";
  }
}

static GLuint shader_create(GLenum type, const char* src) {
  GLint shader = glCreateShader(type);
  glShaderSource(shader, 1, (const char **) &src, NULL);
  glCompileShader(shader);

  GLint stat = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &stat);

  if (!stat) {
    GLint required_length = 0x0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &required_length);
    assert(required_length != 0x0);

    char *error_log = (char*)malloc(sizeof(char) * (required_length + 1));
    glGetShaderInfoLog(shader, required_length, NULL, error_log);
    error_log[required_length] = 0;

    fprintf(stderr, "Error: %s shader (%d) did not compile!\n", shader_type_str(type), shader);
    fprintf(stderr, "Error returned:\n%s\n", error_log);
    free(error_log);
    exit(1);
  }

  return shader;
}

GLuint shader_program_create(const char *vertex_src, const char *fragment_src) {
  GLuint fragment_shader = shader_create(GL_FRAGMENT_SHADER, fragment_src);
  GLuint vertex_shader = shader_create(GL_VERTEX_SHADER, vertex_src);

  GLint program = glCreateProgram();
  glAttachShader(program, fragment_shader);
  glAttachShader(program, vertex_shader);
  glLinkProgram(program);

  GLint stat = 0;

  glGetProgramiv(program, GL_LINK_STATUS, &stat);
  if (!stat) {
    GLint required_length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &required_length);
    assert(required_length != 0);

    char *error_log = (char*)malloc(sizeof(char) * (required_length + 1));
    glGetProgramInfoLog(program, required_length, NULL, error_log);
    fprintf(stderr, "Error: linking shaders(%d, %d)\n", vertex_shader, fragment_shader);
    fprintf(stderr, "Error returned:\n%s\n", error_log);
    free(error_log);
    exit(1);
  }

  return program;
}

/* Some common & simple shaders */
static const char *shader_fragment_passtrough = SHADER_GLSLV(320,
  precision mediump float;
  in vec4 v_color;
  out vec4 color_out;
  void main() {
    color_out = v_color;
  };
);

static const char *shader_vertex_MVP = SHADER_GLSLV(320,
  uniform mat4 modelviewProjection;
  in vec4 pos;
  in vec4 color;
  out vec4 v_color;
  void main() {
    gl_Position = modelviewProjection * pos;
    v_color = color;
  };
);

const char* shader_get(enum shader_select select) {
  switch(select) {
    case SHADER_VERTEX_MVP: return shader_vertex_MVP;
    case SHADER_FRAGMENT_PASSTHROUGH: return shader_fragment_passtrough;
    default: break;
  }
  assert(0); /* Should not reach! */
  return 0;
}
