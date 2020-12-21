// MIT License
// 
// Copyright (c) 2020 Konstantin Rolf
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <engine/engine.h>
#include <math.h>
// ---- Vector Creation ---- //

void CreateVector2f(Vec2f *vec, float val)
{
    vec->data[0] = val;
    vec->data[1] = val;
}
void CreateVector3f(Vec3f *vec, float val)
{
    vec->data[0] = val;
    vec->data[1] = val;
    vec->data[2] = val;
}
void CreateVector4f(Vec4f *vec, float val)
{
    vec->data[0] = val;
    vec->data[1] = val;
    vec->data[2] = val;
    vec->data[3] = val;
}

// ---- Matrix Creation ---- //

const Matrix2f identity2f = {{
    1.0f, 0.0f,
    0.0f, 1.0f
}};
const Matrix3f identity3f = {{
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
}};
const Matrix4f identity4f = {{
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
}};

void CreateMatrix2f(Matrix2f *mat) { *mat = identity2f; }
void CreateMatrix3f(Matrix3f *mat) { *mat = identity3f; }
void CreateMatrix4f(Matrix4f *mat) { *mat = identity4f; }

// ---- Matrix multiplication ---- //

void MulMatrix2f(const Matrix2f *mat1, const Matrix2f *mat2, Matrix2f *result)
{
    const float *m1 = mat1->data;
    const float *m2 = mat2->data;

    Matrix2f temp;
    temp.data[0] = m1[0] * m2[0] + m1[1] * m2[2];
    temp.data[1] = m1[0] * m2[1] + m1[1] * m2[3];
    temp.data[2] = m1[2] * m2[0] + m1[3] * m2[2];
    temp.data[3] = m1[2] * m2[1] + m1[3] * m2[3];
    *result = temp;
}
void MulMatrix3f(const Matrix3f *mat1, const Matrix3f *mat2, Matrix3f *result)
{
    const float *m1 = mat1->data;
    const float *m2 = mat2->data;

    Matrix3f temp;
    temp.data[0] = m1[0] * m2[0] + m1[1] * m2[3] + m1[2] * m2[6]; 
    temp.data[1] = m1[0] * m2[1] + m1[1] * m2[4] + m1[2] * m2[7]; 
    temp.data[2] = m1[0] * m2[2] + m1[1] * m2[5] + m1[2] * m2[8]; 
    temp.data[3] = m1[3] * m2[0] + m1[4] * m2[3] + m1[5] * m2[6]; 
    temp.data[4] = m1[3] * m2[1] + m1[4] * m2[4] + m1[5] * m2[7]; 
    temp.data[5] = m1[3] * m2[2] + m1[4] * m2[5] + m1[5] * m2[8]; 
    temp.data[6] = m1[6] * m2[0] + m1[7] * m2[3] + m1[8] * m2[6]; 
    temp.data[7] = m1[6] * m2[1] + m1[7] * m2[4] + m1[8] * m2[7]; 
    temp.data[8] = m1[6] * m2[2] + m1[7] * m2[5] + m1[8] * m2[8]; 
    *result = temp;
}
void MulMatrix4f(const Matrix4f *mat1, const Matrix4f *mat2, Matrix4f *result)
{
    const float *m1 = mat1->data;
    const float *m2 = mat2->data;

    Matrix4f temp;
    *result = temp;
}

// ---- Transformations ---- //

void CreateTransformScale3f(Matrix3f *out, float xzoom, float yzoom)
{
    *out = identity3f;
    out->data[0] = xzoom;
    out->data[4] = yzoom;
}
void CreateTransformTranslate3f(Matrix3f *out, float x, float y)
{
    *out = identity3f;
    out->data[6] = x;
    out->data[7] = y;
}
void CreateTransformRotate3f(Matrix3f *out, float angle)
{
    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);
    *out = identity3f;
    out->data[0] = cosAngle;
    out->data[1] = sinAngle;
    out->data[3] = -sinAngle;
    out->data[4] = cosAngle;
}

void TransformScale3f(const Matrix3f *in, Matrix3f *out, float xzoom, float yzoom)
{
    if (in == 0) {
        CreateTransformScale3f(out, xzoom, yzoom);
    } else {
        Matrix3f trans;
        CreateTransformScale3f(&trans, xzoom, yzoom);
        MulMatrix3f(in, &trans, out);
    }
}
void TransformTranslate3f(const Matrix3f *in, Matrix3f *out, float x, float y)
{
    if (in == 0) {
        CreateTransformTranslate3f(out, x, y);
    } else {
        Matrix3f trans;
        CreateTransformTranslate3f(&trans, x, y);
        MulMatrix3f(in , &trans, out);
    }
}
void TransformRotate3f(const Matrix3f *in, Matrix3f *out, float angle)
{
    if (in == 0) {
        CreateTransformRotate3f(out, angle);
    } else {
        Matrix3f trans;
        CreateTransformRotate3f(&trans, angle);
        MulMatrix3f(in , &trans, out);
    }
}


void AddMatrix(const float *mat1, const float* mat2, float *res, int dim)
{
    for (int i = 0; i < dim; i++)
        res[i] = mat1[i] + mat2[i];
}
void SubMatrix(const float *mat1, const float* mat2, float *res, int dim)
{
    for (int i = 0; i < dim; i++)
        res[i] = mat1[i] - mat2[i];
}
void NegMatrix(const float *mat1, float *res, int dim)
{
    for (int i = 0; i < dim; i++)
        res[i] = -mat1[i];
}

// ---- Matrix Printing ---- //

static void pMatrix(const float *data, int width, int height)
{
    int index = 0;
    for (int w = 0; w < width; w++) {
        for (int h = 0; h < height; h++) {
            printf("%f ", data[index]);
            index++;
        }
        printf("\n");
    }
}

void PrintMatrix2f(const Matrix2f *mat) { pMatrix(mat->data, 2, 2); }
void PrintMatrix3f(const Matrix3f *mat) { pMatrix(mat->data, 3, 3); }
void PrintMatrix4f(const Matrix4f *mat) { pMatrix(mat->data, 4, 4); }