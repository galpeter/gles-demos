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
#include <math.h>
#include <string.h>

#include "matrix.h"

void matrix_make_rotate_z(GLfloat *matrix, GLfloat angle) {
  // Set the input matrix to a simple rotation matrix
  assert(matrix != 0);
  float c = cos(angle * M_PI / 180.0);
  float s = sin(angle * M_PI / 180.0);

  for (int i = 0; i < 16; i++) {
    matrix[i] = 0.0;
  }
  matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0;

  matrix[0] = c;
  matrix[1] = s;
  matrix[4] = -s;
  matrix[5] = c;
}

void matrix_make_scale(GLfloat *matrix, GLfloat xs, GLfloat ys, GLfloat zs) {
  assert(matrix != 0);
  for (int i = 0; i < 16; i++) {
    matrix[i] = 0.0;
  }
  matrix[0] = xs;
  matrix[5] = ys;
  matrix[10] = zs;
  matrix[15] = 1.0;
}

void matrix_mul(GLfloat *prod, const GLfloat *a, const GLfloat *b) {
#define A(row,col)  a[(col<<2)+row]
#define B(row,col)  b[(col<<2)+row]
#define P(row,col)  p[(col<<2)+row]
  GLfloat p[16];
  for (int i = 0; i < 4; i++) {
    const GLfloat ai0 = A(i, 0), ai1 = A(i, 1), ai2 = A(i, 2), ai3 = A(i, 3);
    P(i,0) = ai0 * B(0,0) + ai1 * B(1,0) + ai2 * B(2,0) + ai3 * B(3,0);
    P(i,1) = ai0 * B(0,1) + ai1 * B(1,1) + ai2 * B(2,1) + ai3 * B(3,1);
    P(i,2) = ai0 * B(0,2) + ai1 * B(1,2) + ai2 * B(2,2) + ai3 * B(3,2);
    P(i,3) = ai0 * B(0,3) + ai1 * B(1,3) + ai2 * B(2,3) + ai3 * B(3,3);
  }
  memcpy(prod, p, sizeof(p));
#undef A
#undef B
#undef PROD
}
