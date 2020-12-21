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
#include <upng.h>

typedef struct {
    float x, y, tx, ty;
} Vertex2D;

static const Vertex2D rect[] = {
    { -1.0, -1.0,  0.0,  1.0 - 0.0 },
    {  1.0,  1.0,  1.0,  1.0 - 1.0 },
    { -1.0,  1.0,  0.0,  1.0 - 1.0 },
    {  1.0,  1.0,  1.0,  1.0 - 1.0 },
    { -1.0, -1.0,  0.0,  1.0 - 0.0 },
    {  1.0, -1.0,  1.0,  1.0 - 0.0 },

};

static const char * vert =
"#version 330 core\n"
"layout(location = 0) in vec2 vertexPosition;\n"
"layout(location = 1) in vec2 vertexTexturePosition;\n"

"out vec2 texturePosition;\n"
"// Values that stay constant for the whole mesh.\n"
"uniform mat3 transform;\n"
"uniform float numberOfRows;\n"
"uniform vec2 offset;\n"
"void main() {\n"
"    gl_Position = vec4(transform * vec3(vertexPosition, -1.0), 1.0);\n"
"    texturePosition = (vertexTexturePosition / numberOfRows) + offset;\n"
"}\0";

static const char * frag = 
"#version 330 core\n"
"in vec2 texturePosition;\n"
"out vec3 color;\n"

"uniform sampler2D textureSampler;\n"

"void main() {\n"
"    color = texture(textureSampler, texturePosition).xyz;\n"
"}\0";

static void showShaderLog(GLuint shader)
{
    fprintf(stderr, "Shader compilation result\n");
    GLint logSize = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
    GLchar errorLog[1024];
    errorLog[logSize] = '\0';
    glGetShaderInfoLog(shader, logSize, &logSize, &errorLog[0]);
    fprintf(stderr, "%s\n", errorLog);
}

static void showProgramLinkLog(GLuint program)
{
    fprintf(stderr, "Program linkage failed\n");
    GLint logSize = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
    GLchar errorLog[1024];
    errorLog[logSize] = '\0';
    glGetProgramInfoLog(program, logSize, &logSize, &errorLog[0]);
    fprintf(stderr, "%s\n", errorLog);
}

int Create2DShader(Shader2D* shader)
{
    GLuint vertex, fragment;
    GLint successVertex, successFragment, success;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    CGL(glShaderSource(vertex, 1, (const GLchar**) &vert, 0));
    CGL(glShaderSource(fragment, 1, (const GLchar**) &frag, 0));
    CGL(glCompileShader(vertex));
    CGL(glCompileShader(fragment));

    CGL(glGetShaderiv(vertex, GL_COMPILE_STATUS, &successVertex));
    CGL(glGetShaderiv(fragment, GL_COMPILE_STATUS, &successFragment));
    if(!successVertex || !successFragment) {
        showShaderLog(vertex);
        showShaderLog(fragment);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return 0;
    }
    shader->program = glCreateProgram();
    CGL(glAttachShader(shader->program, vertex));
    CGL(glAttachShader(shader->program, fragment));

    CGL(glLinkProgram(shader->program));
    CGL(glGetProgramiv(shader->program, GL_LINK_STATUS, &success));
    if (!success) {
        showProgramLinkLog(shader->program);
        CGL(glDeleteProgram(shader->program));
        CGL(glDeleteShader(vertex));
        CGL(glDeleteShader(fragment));
        return 1;
    }

    CGL(glDeleteShader(vertex));
    CGL(glDeleteShader(fragment));

    shader->uniformTexture = glGetUniformLocation(
        shader->program, "textureSampler");
    shader->uniformTransform = glGetUniformLocation(
        shader->program, "transform");
    shader->uniformOffset = glGetUniformLocation(
        shader->program, "offset");
    shader->uniformNumberOfRows = glGetUniformLocation(
        shader->program, "numberOfRows");
    if (shader->uniformTexture == -1) {
        fprintf(stderr, "Could not find uniform: textureSampler");
        return 2;
    }
    if (shader->uniformTransform == -1) {
        fprintf(stderr, "Could not find uniform: transform");
        return 3;
    }
    if (shader->uniformNumberOfRows == -1) {
        fprintf(stderr, "Could not find uniform: numberOfRows");
        return 4;
    }
    if (shader->uniformOffset == -1) {
        fprintf(stderr, "Could not find uniform: offset");
        return 5;
    }

    // generate the VAOs
    CGL(glGenVertexArrays(1, &shader->rectVAO));
    CGL(glBindVertexArray(shader->rectVAO));

    // generate the VBO
    CGL(glGenBuffers(1, &shader->rectVBO));
    CGL(glBindBuffer(GL_ARRAY_BUFFER, shader->rectVBO));
    CGL(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex2D) * 6, rect, GL_STATIC_DRAW));

    CGL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), 0));
    CGL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*) (sizeof(float) * 2)));
    
    CGL(glEnableVertexAttribArray(0));
    CGL(glEnableVertexAttribArray(1));

    return 0;
}

int Delete2DShader(Shader2D *shader) {
    CGL(glDeleteVertexArrays(1, &shader->rectVAO));
    CGL(glDeleteBuffers(1, &shader->rectVBO));
    CGL(glDeleteProgram(shader->program));
    return 0;
}

static int createPNG(Texture2D *texture, upng_t* upng)
{
    if (upng == NULL) {
        fprintf(stderr, "Could not load UPNG struct\n");
        return -1;
    }
    upng_decode(upng);
    if (upng_get_error(upng) != UPNG_EOK) {
        fprintf(stderr, "Could not decode UPNG object\n");
        upng_free(upng);
        return -2;
    }

    upng_format format = upng_get_format(upng);
    if (format != UPNG_RGBA8) {
        fprintf(stderr, "The only supported format is RGBA8\n");
        return -3;
    }

    int width = upng_get_width(upng);
    int height = upng_get_height(upng);
    const unsigned char * buffer = upng_get_buffer(upng);
    int status = Create2DTextureFromRAW(texture, buffer, width, height);
    upng_free(upng);
    return status;
}

int Create2DTextureFromPNGFile(Texture2D *texture, const char *pngPath)
{
    return createPNG(texture, upng_new_from_file(pngPath));
}

int Create2DTextureFromPNG(Texture2D *texture, const unsigned char *pngData, int length)
{
    return createPNG(texture, upng_new_from_bytes(pngData, length));
}

int Create2DTextureFromRAW(Texture2D *texture, const unsigned char *rawData, int width, int height)
{
    texture->width = width;
    texture->height = height;
    CGL(glGenTextures(1, &texture->glTexture));
    CGL(glBindTexture(GL_TEXTURE_2D, texture->glTexture));
    CGL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, rawData));

    CGL(glGenerateMipmap(GL_TEXTURE_2D));
    CGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    CGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    CGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    CGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    return 0;
}

void Render2DShader(const Shader2D *shader, const Texture2D *texture, const EntityQueue *queue)
{
    // actives the shader program
    CGL(glUseProgram(shader->program));
    // configures the general OpenGL parameters
    CGL(glDisable(GL_DEPTH_TEST));
    CGL(glDisable(GL_CULL_FACE));
    // configures the shader local parameters
    CGL(glActiveTexture(GL_TEXTURE0));
    CGL(glUniform1i(shader->uniformTexture, 0));
    // binds the required texture and VAO
    CGL(glBindTexture(GL_TEXTURE_2D, texture->glTexture));
    CGL(glBindVertexArray(shader->rectVAO));

    for (int i = 0; i < queue->current; i++)
    {
        Entity *entity = &(queue->queue[i]);

        // creates the transformation matrix
        Matrix3f mat = identity3f;
        TransformScale3f(&mat, &mat, entity->scale, entity->scale);
        TransformRotate3f(&mat, &mat, entity->rotation);
        TransformTranslate3f(&mat, &mat, entity->positionX, entity->positionY);

        CGL(glUniformMatrix3fv(shader->uniformTransform, 1, 0, mat.data));
        CGL(glUniform2f(shader->uniformOffset,
            TileAtlasGetPosXScaled(entity->atlas, entity->tileIndex),
            TileAtlasGetPosYScaled(entity->atlas, entity->tileIndex)
        ));
        CGL(glUniform1f(shader->uniformNumberOfRows,
            entity->atlas->elemHeight));
        CGL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }
    CGL(glUseProgram(0));
}

int lt_check_gl_error(const char *cmd, int line, const char *file) {
     GLenum err = glGetError();
     const char* msg = NULL;

     switch (err)
     {
     case GL_NO_ERROR: return 0;
     case GL_INVALID_ENUM: msg = "invalid enumeration"; break;
     case GL_INVALID_VALUE: msg = "invalid value"; break;
     case GL_INVALID_OPERATION: msg = "invalid operation"; break;
     case GL_INVALID_FRAMEBUFFER_OPERATION: msg = "invalid framebuffer operation"; break;
     case GL_OUT_OF_MEMORY: msg = "out of memory"; break;
     case GL_STACK_UNDERFLOW: msg = "stack underflow"; break;
     case GL_STACK_OVERFLOW: msg = "stack overflow"; break;
     default: msg = "unknown error"; break;
     }

     fprintf(stderr, "OpenGL error (%s) during operation \"%s\" at %s:%d!\n",
        msg, cmd, file, line);
     return 1;
}
