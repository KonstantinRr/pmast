
#include <stdio.h>
#include <stdlib.h>

#include <glad/gl.h>

#if defined(TARGET_RELEASE)
    #define CGL(cmd) cmd
#else
    #define CGL(cmd)                          \
    do {                                    \
        cmd;                                \
        (void) lt_check_gl_error(#cmd, __LINE__, __FILE__); \
    } while (0)
#endif

// ---- Vector definitions ---- //
typedef union {
    float data[2];
    struct { float x, y; } values;
} Vec2f;

typedef struct {
    float data[3];
    struct { float x, y, z; } values;
} Vec3f;

typedef struct {
    float data[4];
    struct { float x, y, z, w; } values;
} Vec4f;

void CreateVector2f(Vec2f *vec, float val);
void CreateVector3f(Vec3f *vec, float val);
void CreateVector4f(Vec4f *vec, float val);

// ---- Matrix definitions ---- //

typedef union {
    float data[4];
    struct {
        float
        xx, xy,
        yx, yy;
    } names;
} Matrix2f;

typedef union {
    float data[9];
    struct {
        float
        xx, xy, xz,
        yx, yy, yz,
        zx, zy, zz;
    } names;
} Matrix3f;

typedef union {
    float data[16];
    struct {
        float
        xx, xy, xz, xw,
        yx, yy, yz, yw,
        zx, zy, zz, zw,
        wx, wy, wz, ww;
    } names;
} Matrix4f;

extern const Matrix2f identity2f;
extern const Matrix3f identity3f;
extern const Matrix4f identity4f;

void CreateMatrix2f(Matrix2f *mat);
void CreateMatrix3f(Matrix3f *mat);
void CreateMatrix4f(Matrix4f *mat);

void CreateTransformScale3f(Matrix3f *out, float xzoom, float yzoom);
void CreateTransformTranslate3f(Matrix3f *out, float x, float y);
void CreateTransformRotate3f(Matrix3f *out, float angle);
void TransformScale3f(const Matrix3f *in, Matrix3f *out, float xzoom, float yzoom);
void TransformTranslate3f(const Matrix3f *in, Matrix3f *out, float x, float y);
void TransformRotate3f(const Matrix3f *in, Matrix3f *out, float angle);

void PrintMatrix2f(const Matrix2f *mat);
void PrintMatrix3f(const Matrix3f *mat);
void PrintMatrix4f(const Matrix4f *mat);

void AddMatrix(const float *mat1, const float* mat2, float *res, int dim);
void SubMatrix(const float *mat1, const float* mat2, float *res, int dim);
void NegMatrix(const float *mat1, float *res, int dim);

void MulMatrix2f(const Matrix2f *mat, const Matrix2f *mat2, Matrix2f *result);
#define AddMatrix2f(a, b, c) AddMatrix(a->data, b->data, c->data, 4)
#define SubMatrix2f(a, b, c) SubMatrix(a->data, b->data, c->data, 4)
#define NegMatrix2f(a, c) NegMatrix(a->data, c->data, 4)

void MulMatrix3f(const Matrix3f *mat, const Matrix3f *mat2, Matrix3f *result);
#define AddMatrix3f(a, b, c) AddMatrix(a->data, b->data, c->data, 9)
#define SubMatrix3f(a, b, c) SubMatrix(a->data, b->data, c->data, 9)
#define NegMatrix3f(a, c) NegMatrix(a->data, c->data, 9)

void MulMatrix4f(const Matrix4f *mat, const Matrix4f *mat2, Matrix4f *result);
#define AddMatrix4f(a, b, c) AddMatrix(a->data, b->data, c->data, 16)
#define SubMatrix4f(a, b, c) SubMatrix(a->data, b->data, c->data, 16)
#define NegMatrix4f(a, c) NegMatrix(a->data, c->data, 16)

typedef struct {
    GLuint glTexture;
    int width, height;
} Texture2D;

typedef struct {
    const Texture2D *texture;
    int elemWidth, elemHeight;
    int elemSize;
} TileAtlas;

typedef struct {
    GLuint program;
    GLuint rectVAO, rectVBO;
    GLint uniformTexture;
	GLint uniformTransform;
    GLint uniformOffset;
    GLint uniformNumberOfRows;
} Shader2D;

typedef struct {
    int tileIndex;
    const TileAtlas *atlas;

    float positionX;
    float positionY;
    float scale;
    float rotation;
} Entity;

typedef struct {
    Entity queue[4096];
    int current;
} EntityQueue;


// ---- TileAtlas ---- //
void TileAtlasCreate(TileAtlas *atlas, const Texture2D *texture, int elemSize);
int TileAtlasGetIndex(const TileAtlas *atlas, int x, int y);
int TileAtlasGetPosXIndex(const TileAtlas *atlas, int tile);
int TileAtlasGetPosYIndex(const TileAtlas *atlas, int tile);
int TileAtlasGetPosX(const TileAtlas *atlas, int tile);
int TileAtlasGetPosY(const TileAtlas *atlas, int tile);
float TileAtlasGetPosXScaled(const TileAtlas *atlas, int tile);
float TileAtlasGetPosYScaled(const TileAtlas *atlas, int tile);

// ---- Entity ---- //
void EntityCreate(Entity *entity, float positionX, float positionY, float rotation, float scale);
void EntityCreateTiling(Entity *entity, int tileIndex, const TileAtlas *atlas,
    float positionX, float positionY, float rotation, float scale);
// ---- Queue ---- //
void QueueCreate(EntityQueue *queue);
void QueueClear(EntityQueue *queue);
void QueuePush(EntityQueue *queue, Entity *entity);
void QueuePop(EntityQueue *queue);
Entity* QueuePointer(EntityQueue *queue);
Entity* QueuePointerNext(EntityQueue *queue);

// ---- Shader ---- //
int Create2DTextureFromPNGFile(Texture2D *texture, const char *pngPath);
int Create2DTextureFromPNG(Texture2D *texture, const unsigned char *pngData, int length);
int Create2DTextureFromRAW(Texture2D *texture, const unsigned char *data, int width, int height);

int Create2DShader(Shader2D *shader);
int Delete2DShader(Shader2D *shader);
void Render2DShader(const Shader2D *shader, const Texture2D *texture, const EntityQueue *queue);
