/// MIT License
/// 
/// Copyright (c) 2020 Konstantin Rolf
/// 
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
/// 
/// Written by Konstantin Rolf (konstantin.rolf@gmail.com)
/// July 2020

#include <engine/glmodel.hpp>

NYREM_USE_NAMESPACE

GLModel::GLModel(GLsizei modelSize, GLuint vao, GLuint vbo) {
	this->modelSize = modelSize;
	this->vao = vao;
	this->vbo = vbo;
}

GLModel::GLModel(const ExportFile& file)
{
    generateVAO();
    CGL(glGenBuffers(1, &vbo));
    CGL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    CGL(glBufferData(GL_ARRAY_BUFFER, file.data.size() * sizeof(float),
        file.data.data(), GL_STATIC_DRAW));

    CGL(glBindBuffer(GL_ARRAY_BUFFER, vbo));

    // Creates the needed glVertexAttribPointers
    size_t strideElem = 0;
    for (size_t i = 0; i < file.exp.size(); i++)
        strideElem += file.exp[i].size;

    size_t offset = 0;
    for (size_t i = 0; i < file.exp.size(); i++) {
        CGL(glVertexAttribPointer(
            i, file.exp[i].size, GL_FLOAT, GL_FALSE,
            strideElem * sizeof(float), // stride
            (void*)(sizeof(float) * offset) // initial 
        ));
        offset += file.exp[i].size;
    }
    modelSize = static_cast<GLsizei>(file.data.size() / strideElem);
    indexSize = 0;

    // Enables all used VertexAttribPointers
    for (size_t i = 0; i < file.exp.size(); i++)
        CGL(glEnableVertexAttribArray(i));

    CGL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    CGL(glBindVertexArray(0));
}

void GLModel::generateVAO() {
    CGL(glGenVertexArrays(1, &vao));
    CGL(glBindVertexArray(vao));
}

void GLModel::generateVIO(const std::vector<size_t> &index) {
    glGenBuffers(1, &vio);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vio);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * 2, index.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GLModel::bind() {
    CGL(glBindVertexArray(vao));
    CGL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
}

void GLModel::unbind() {
    CGL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    CGL(glBindVertexArray(0));
}

void GLModel::cleanUp() {
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
    vbo = std::numeric_limits<GLuint>::max();
    vao = std::numeric_limits<GLuint>::max();
}

GLsizei GLModel::getSize() const { return modelSize; }
GLuint GLModel::getVAO() const { return vao; }
GLuint GLModel::getVBO() const { return vbo; }

// ---- GLTexture2D ---- //
void GLTexture2D::applyFilters() {
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void GLTexture2D::genTexture() {
    hasTexture = true;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
}

GLTexture2D::GLTexture2D() {}
/*
GLTexture2D::GLTexture2D(
    const std::shared_ptr<GLModel> &model,
    const std::string &textureFile
) {
    this->model = model;
    genTexture();

    // Push image data to texture.
    LTImage image = loadImage(textureFile);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
        static_cast<GLsizei>(image.getWidth()),
        static_cast<GLsizei>(image.getHeight()),
        0, GL_RGBA, GL_UNSIGNED_BYTE, image.getData());

    applyFilters();
}
*/

/*
GLTexture2D::GLTexture2D(const BitmapImage &image) {
    genTexture();
    using uchar = unsigned char;
    uchar *bits = new uchar[image.width() * image.height() * 4];
    uchar *pixels = (uchar*) image.data();
    for (int pix = 0; pix<image.width() * image.height(); pix++)
    {
        bits[pix * 4 + 0] = pixels[pix * 4 + 2];
        bits[pix * 4 + 1] = pixels[pix * 4 + 1];
        bits[pix * 4 + 2] = pixels[pix * 4 + 0];
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0,
        GL_RGBA, GL_UNSIGNED_BYTE, bits);
    applyFilters();
    delete[] bits;
}
*/

GLTexture2D::GLTexture2D(const ImageRGB8 &image) {
    genTexture();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,
        static_cast<GLsizei>(image.getXExtent()),
        static_cast<GLsizei>(image.getYExtent()),
        0, GL_RGB, GL_UNSIGNED_BYTE, image.getData());

    applyFilters();
}


GLTexture2D::GLTexture2D(const ImageBGR8 &image) {
    genTexture();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,
        static_cast<GLsizei>(image.getXExtent()),
        static_cast<GLsizei>(image.getYExtent()),
        0, GL_BGR, GL_UNSIGNED_BYTE, image.getData());

    applyFilters();
}


GLTexture2D::GLTexture2D(const ImageRGBA8 &image) {
    genTexture();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
        static_cast<GLsizei>(image.getXExtent()),
        static_cast<GLsizei>(image.getYExtent()),
        0, GL_RGBA, GL_UNSIGNED_BYTE, image.getData());

    applyFilters();
} 


GLTexture2D::~GLTexture2D() {
    cleanup();
}

void GLTexture2D::cleanup() {
    if (hasTexture) {
        glDeleteTextures(1, &texture);
    }
}

void GLTexture2D::bind() {
    if (hasTexture) {
        glBindTexture(GL_TEXTURE_2D, texture);
    }
}

GLuint GLTexture2D::getTexture() const { return texture; }


GLMaterial::GLMaterial(const glm::vec4 &material)
    : material(material) { }

const glm::vec4& GLMaterial::getMaterial() const { return material; }