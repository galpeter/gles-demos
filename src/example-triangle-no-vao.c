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

#include "matrix.h"
#include "render_common.h"
#include "shaders.h"

static const char *shader_vertex = SHADER_GLSLV(320,
  uniform int hasMVP;
  uniform mat4 modelviewProjection;
  out vec4 v_color;
  void main() {
    const vec4 colors[3] = vec4[3](
      vec4(1.0, 0.0, 0.0, 1.0),
      vec4(0.0, 1.0, 0.0, 1.0),
      vec4(0.0, 0.0, 1.0, 1.0)
    );
    const vec4 verts[3] = vec4[3](
      vec4(-1.0, -1.0, 1.0, 1.0),
      vec4( 1.0, -1.0, 1.0, 1.0),
      vec4( 0.0, 1.0, 1.0, 1.0)
    );
    vec4 pos = verts[gl_VertexID];
    vec4 color = colors[gl_VertexID];
    if (hasMVP == 1) {
      gl_Position = modelviewProjection * pos;
    } else {
      gl_Position = pos;
    }
    v_color = color;
  };
);

static void init(const RenderContext renderCtx, void **user_data) {
  GLuint program = shader_program_create(shader_vertex, shader_get(SHADER_FRAGMENT_PASSTHROUGH));
  glUseProgram(program);

#ifdef WITH_ROTATION
  static GLuint u_matrix;
  u_matrix = glGetUniformLocation(program, "modelviewProjection");
  GLuint u_hasMVP = glGetUniformLocation(program, "hasMVP");
  glUniform1i(u_hasMVP, 1);

  (*user_data) = (void*)&u_matrix;
#endif

  glClearColor(0.4, 0.4, 0.4, 0.0);
}

static void draw(view_rotation_t rotation, void *user_data) {
#ifdef WITH_ROTATION
  GLfloat mat[16], rot[16], scale[16];
  GLuint u_matrix = *((GLuint*)user_data);

  /* Set modelview/projection matrix */
  matrix_make_rotate_z(rot, rotation.x);
  matrix_make_scale(scale, 0.5, 0.5, 0.5);
  matrix_mul(mat, rot, scale);

  glUniformMatrix4fv(u_matrix, 1, GL_FALSE, mat);
#endif

  glClearColor(0.4, 0.4, 0.4, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
}

int main(int argc, char *argv[]) {
  RenderCallbacks callbacks;
  callbacks.initializer = init;
  callbacks.draw = draw;

  return render_main(argc, argv, callbacks);
}
