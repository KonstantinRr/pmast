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

#pragma once

#ifndef NYREM_GLMODEL_HPP
#define NYREM_GLMODEL_HPP

#include <engine/internal.hpp>
#include <engine/graphics.hpp>
#include <engine/resource.hpp>

#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <string>

NYREM_NAMESPACE_BEGIN

enum ModelType {
    VERTEX,                 // struct Vertex
    POINT_VERTEX,           // struct PointVertex
    NORMAL_VERTEX,          // struct NormalVertex
    VERTEX2D,               // struct Vertex2D
    VERTEX_INDEXED,         // struct Vertex Indexed
    POINT_VERTEX_INDEXED,   // struct PointVertex Indexed
    NORMAL_VERTEX_INDEXED   // struct NormalVertex Indexed
};

class GLModel {
protected:
    GLuint vao, vbo, vio;
    GLsizei modelSize, indexSize;
    ModelType type;

public:
    GLModel(GLsizei modelSize, GLuint vao, GLuint vbo);
    GLModel(const ExportFile &file);

    GLModel(const std::vector<Vertex2D> &vertices);
    GLModel(const std::vector<Vertex> &vertices);
    GLModel(const std::vector<PointVertex> &vertices);
    GLModel(const std::vector<NormalVertex> &vertices);

    GLModel(const std::vector<Vertex> &vertices, const std::vector<size_t> &index);
    GLModel(const std::vector<PointVertex> &vertices, const std::vector<size_t> &index);
    GLModel(const std::vector<NormalVertex> &vertices, const std::vector<size_t> &index);

    void generateVAO();
    void generateVIO(const std::vector<size_t> &index);
    void generateVBOVertexArray2D(const std::vector<Vertex2D> &vertices);
    void generateVBOVertexArray(const std::vector<Vertex> &vertices);
    void generateVBOPointVertexArray(const std::vector<PointVertex> &vertices);
    void generateVBONormalVertexArray(const std::vector<NormalVertex> &vertices);

    void cleanUp();
    void bind();
    void unbind();

    GLsizei getSize() const;
    GLuint getVAO() const;
    GLuint getVBO() const;
};


class GLTexture2D {
protected:
    GLuint texture; 
    bool hasTexture;

protected:
    void applyFilters();
    void genTexture();
public:
    GLTexture2D();

    GLTexture2D(const ImageRGB8 &image);
    GLTexture2D(const ImageBGR8 &image);
    GLTexture2D(const ImageRGBA8 &image);
    //GLTexture2D(const BitmapImage &image);

    ~GLTexture2D();

    void cleanup();
    void bind();

    GLuint getTexture() const;
};

class GLMaterial {
protected:
    glm::vec4 material;

public:
    GLMaterial(const glm::vec4 &material = {0.5F, 0.5F, 0.5F, 5.0F});

    const glm::vec4& getMaterial() const;
};

NYREM_NAMESPACE_END

#endif // NYREM_GLMODEL_HPP
