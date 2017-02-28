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

struct ProgramData {
  GLint attr_pos;
  GLint attr_color;
  GLint u_matrix;
  GLuint program;
};

static void config_shaders(GLuint program, struct ProgramData *data) {
  data->attr_pos = 0;
  data->attr_color = 1;

  glBindAttribLocation(program, data->attr_pos, "pos");
  glBindAttribLocation(program, data->attr_color, "color");
  glLinkProgram(program); /* needed to put attribs into effect */

  data->u_matrix = glGetUniformLocation(program, "modelviewProjection");
}

static void create_vao(struct ProgramData data) {
  static const GLfloat buffer_data[3][5] = {
    { -1, -1, 1, 0, 0 }, /* position (2 float), color (3 float) */
    {  1, -1, 0, 1, 0 },
    {  0,  1, 0, 0, 1 },
  };

  GLvoid *offset_ptr;
#ifdef WITH_PTR_DATA
  offset_ptr = (GLvoid*)&buffer_data;
#else
  GLuint VAO_id, VBO_id;

  glGenVertexArrays(1, &VAO_id);
  glBindVertexArray(VAO_id);

  glGenBuffers(1, &VBO_id);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data), buffer_data, GL_STATIC_DRAW);
  offset_ptr = NULL;
#endif

  glVertexAttribPointer(data.attr_pos, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, offset_ptr); /* position */
  glVertexAttribPointer(data.attr_color, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*) offset_ptr + (sizeof(float) * 2)); /* color */

  glEnableVertexAttribArray(data.attr_pos);
  glEnableVertexAttribArray(data.attr_color);
}

static void init(const RenderContext renderCtx, void **user_data) {
  static struct ProgramData data;

  glClearColor(0.4, 0.4, 0.4, 0.0);
  data.program = shader_program_create(shader_get(SHADER_VERTEX_MVP),
                                       shader_get(SHADER_FRAGMENT_PASSTHROUGH));
  config_shaders(data.program, &data);
  create_vao(data);

  glUseProgram(data.program);
  (*user_data) = (void*)&data;
}

static void draw(view_rotation_t rotation, void *user_data) {
  struct ProgramData *data = (struct ProgramData*)user_data;

  GLfloat mat[16], rot[16], scale[16];

  /* Set modelview/projection matrix */
  matrix_make_rotate_z(rot, rotation.x);
  matrix_make_scale(scale, 0.5, 0.5, 0.5);
  matrix_mul(mat, rot, scale);

  glUniformMatrix4fv(data->u_matrix, 1, GL_FALSE, mat);

  glClearColor(0.4, 0.4, 0.4, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main(int argc, char *argv[]) {
  RenderCallbacks callbacks;
  callbacks.initializer = init;
  callbacks.draw = draw;

  return render_main(argc, argv, callbacks);
}
