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

#ifndef NYREM_RESOURCE_HPP
#define NYREM_RESOURCE_HPP

#include "internal.hpp"
#include <glm/glm.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>

class FIBITMAP; // forward declaration of FreeImage types

namespace nyrem {
    /// <summary> class PointVertex2D
    /// This data struct stores 2 dimensional vertex
    /// data in the following layout:
    /// float32/float64 x, float32/float64 y
    /// </summary>
    template<typename Type>
    struct PointVertex2DGeneric {
        static constexpr size_t
            INDEX_X = 0, INDEX_Y = 1;
        Type data[2];

        PointVertex2DGeneric() noexcept;
        PointVertex2DGeneric(Type x, Type y) noexcept;

        inline Type x() const noexcept { return data[INDEX_X]; }
        inline Type y() const noexcept { return data[INDEX_Y]; }
    };

    using FloatPointVertex2D = PointVertex2DGeneric<float>;
    using DoublePointVertex2D = PointVertex2DGeneric<double>;
    using PointVertex2D = FloatPointVertex2D;

    /// <summary> class Vertex2D
    /// This data struct stores 2 dimensional vertex
    /// and texture data in the following layout:
    /// float32/float64 x, float32/float64 y
    /// float32/float64 tx, float32/float64 ty
    /// </summary>
    template<typename Type>
    struct Vertex2DGeneric {
        static constexpr size_t
            INDEX_X = 0, INDEX_Y = 1,
            INDEX_TX = 2, INDEX_TY = 3;
        Type data[4];

        Vertex2DGeneric() noexcept;
        explicit Vertex2DGeneric(Type x, Type y) noexcept;
        explicit Vertex2DGeneric(Type x, Type y, Type tx, Type ty) noexcept;

        inline Type x() const noexcept { return data[INDEX_X]; }
        inline Type y() const noexcept { return data[INDEX_Y]; }
        inline Type tx() const noexcept { return data[INDEX_TX]; }
        inline Type ty() const noexcept { return data[INDEX_TY]; }
    };

    using FloatVertex2D = Vertex2DGeneric<float>;
    using DoubleVertex2D = Vertex2DGeneric<double>;
    using Vertex2D = FloatVertex2D;

    /// <summary> class Point Vertex
    /// This data struct stores 3 dimensional vertex
    /// data in the following layout:
    /// float32 x, float32 y, float32 z
    /// </summary>
    template<typename Type>
    struct PointVertexGeneric {
        static constexpr size_t
            INDEX_X = 0, INDEX_Y = 1, INDEX_Z = 2;
        Type data[3];

        PointVertexGeneric() noexcept;
        PointVertexGeneric(Type x, Type y, Type z) noexcept;

        inline Type x() const noexcept { return data[INDEX_X]; }
        inline Type y() const noexcept { return data[INDEX_Y]; }
        inline Type z() const noexcept { return data[INDEX_Z]; }
    };

    using FloatPointVertex = PointVertexGeneric<float>;
    using DoublePointVertex = PointVertexGeneric<double>;
    using PointVertex = FloatPointVertex;

    /// <summary> class NormalVertex
    /// This data struct stores 3 dimensional vertex,
    /// and normal data in the following layout:
    /// float32 x, float32 y, float32 z
    /// float32 nx, float32 ny, float32 nz
    /// </summary>
    template<typename Type>
    struct NormalVertexGeneric {
        static constexpr size_t
            INDEX_X = 0, INDEX_Y = 1, INDEX_Z = 2,
            INDEX_NX = 3, INDEX_NY = 4, INDEX_NZ = 5;
        Type data[6];

        NormalVertexGeneric() noexcept;
        NormalVertexGeneric(
            Type x, Type y, Type z,
            Type nx, Type ny, Type nz) noexcept;

        inline Type x() const noexcept { return data[INDEX_X]; }
        inline Type y() const noexcept { return data[INDEX_Y]; }
        inline Type z() const noexcept { return data[INDEX_Z]; }
        inline Type nx() const noexcept { return data[INDEX_NX]; }
        inline Type ny() const noexcept { return data[INDEX_NY]; }
        inline Type nz() const noexcept { return data[INDEX_NZ]; }
    };

    using FloatNormalVertex = NormalVertexGeneric<float>;
    using DoubleNormalVertex = NormalVertexGeneric<double>;
    using NormalVertex = FloatNormalVertex;

    /// <summary> class NormalVertex
    /// This data struct stores 3 dimensional vertex,
    /// normal and texture data in the following layout:
    /// float32 x, float32 y, float32 z
    /// float32 nx, float32 ny, float32 nz
    /// float32 tx, float32 ty
    /// </summary>
    template<typename Type>
    struct VertexGeneric {
        static constexpr size_t
            INDEX_X = 0, INDEX_Y = 1, INDEX_Z = 2,
            INDEX_NX = 3, INDEX_NY = 4, INDEX_NZ = 5;
        Type data[8];
        
        // Constructors
        VertexGeneric() noexcept;
        VertexGeneric(
            Type x, Type y, Type z,
            Type nx, Type ny, Type nz,
            Type tx1, Type ty1) noexcept;
    };

    using FloatVertex = VertexGeneric<float>;
    using DoubleVertex = VertexGeneric<double>;
    using Vertex = FloatVertex;

    struct Indice {
        int v, t, n;
        
        Indice(int v, int t, int n);
    };

    class HeightMap {
    protected:
        std::vector<std::vector<float>> heightMap;

    public:
        HeightMap(size_t size);
        HeightMap(const std::string &filename);

        void fillRandom();

        void scaleHeight(float scale);

        inline size_t getSize() const { return heightMap.size(); }
        inline auto& getHeightMap() { return heightMap; }
        inline const auto& getHeightMap() const { return heightMap; }
        inline auto& operator[](size_t index) { return heightMap[index]; }
        inline const auto& operator[](size_t index) const { return heightMap[index]; }
    };

    enum ExportType {
        EXPORT_VERTEX,
        EXPORT_VERTEX_INDEXED,
        EXPORT_TEXTURE,
        EXPORT_TEXTURE_INDEXED,
        EXPORT_COLOR,
        EXPORT_COLOR_INDEXED,
        EXPORT_VERTEX_3D,
        EXPORT_VERTEX_INDEXED_3D,
        EXPORT_NORMAL_3D,
        EXPORT_NORMAL_INDEXED_3D,
    };
 
    struct ExportMacro {
        ExportType type;
        size_t size;

        ExportMacro(ExportType type);
    };

    struct ExportFile {
        std::vector<float> data;
        std::vector<ExportMacro> exp;

        size_t strideSize();

        ExportFile() = default;
        ~ExportFile() = default;
        ExportFile(const ExportFile&) = delete;
        ExportFile(ExportFile&&) = default;
        
        ExportFile& operator=(const ExportFile&) = delete;
        ExportFile& operator=(ExportFile &&) = default;
    };

    class MeshBuilderBase {
    protected:
        std::vector<ExportType> exp;

    public:
        MeshBuilderBase() = default;
        virtual ~MeshBuilderBase() = default;

        ExportFile exportData() const;

    protected:
        virtual bool resolveTypes(std::vector<float> &data, size_t i) const = 0;
    };

    class MeshBuilder2D {
    public:
        std::vector<glm::vec2> vertices;
        std::vector<glm::vec2> texCoords;
        std::vector<glm::vec3> colors;

        std::vector<int> v_indices;
        std::vector<int> vt_indices;
        std::vector<int> vc_indices;

    public:
        class Exporter2D : public MeshBuilderBase {
        protected:
            const MeshBuilder2D* builder;

        public: // Can only created by parent class
            Exporter2D(const MeshBuilder2D *builder);

        public:
            inline Exporter2D& addVertex() { exp.push_back(EXPORT_VERTEX); return *this; }
            inline Exporter2D& addTexture() { exp.push_back(EXPORT_TEXTURE); return *this; }
            inline Exporter2D& addColor() { exp.push_back(EXPORT_COLOR); return *this; }

        protected:
            virtual bool resolveTypes(std::vector<float> &data, size_t i) const;
        };


        MeshBuilder2D();
        MeshBuilder2D(
            const std::vector<glm::vec2> &vertices,
            const std::vector<glm::vec2> &texCoords,
            const std::vector<glm::vec3> &colors,
            const std::vector<int> &v_indices,
            const std::vector<int> &vt_indices);

        void clear();

        Exporter2D exporter() const;

        
        float maxExtent() const;
        float minExtent() const;

        float maxXExtent() const;
        float minXExtent() const;
        float maxYExtent() const;
        float minYExtent() const;

        size_t maxExtentIndex() const;
        size_t minExtentIndex() const;

        size_t maxXExtentIndex() const;
        size_t minXExtentIndex() const;
        size_t maxYExtentIndex() const;
        size_t minYExtentIndex() const;

        glm::vec2 center() const;
        glm::vec2 massCenter() const;

        MeshBuilder2D& scale(float scale);
        MeshBuilder2D& scale(float scaleX, float scaleY);
        MeshBuilder2D& translate(glm::vec2 translation);
        MeshBuilder2D& rotate(float angle);

        MeshBuilder2D& transform(const glm::mat2x2 &mat);
        MeshBuilder2D& transform(const glm::mat3x3 &mat);
        MeshBuilder2D& transform(const glm::mat4x4 &mat);
        //void rotate(glm::vec2 rotation);

        glm::vec2 unitize(float unitScale=1.0f);
        glm::vec2 unitizeAxis(float unitScale=1.0f, bool keepProportion=true);
        glm::vec2 centerModel();

        glm::mat3x3 unitizeMatrix(float unitScale=1.0f, bool keepProportion=true) const;
        glm::mat3x3 centerMatrix() const;

        void addVertex(glm::vec2 vertex);
        void addTextureCoord(glm::vec2 texture);
        void addColor(glm::vec3 color);

        template<typename Iterator>
        MeshBuilder2D& addVertices(Iterator begin, Iterator end) {
            vertices.insert(vertices.end(), begin, end);
            return *this;
        }
        template<typename Iterator>
        MeshBuilder2D& addTextureCoords(Iterator begin, Iterator end) {
            texCoords.insert(texCoords.end(), begin, end);
            return *this;
        }
        template<typename Iterator>
        MeshBuilder2D& addColors(Iterator begin, Iterator end) {
            colors.insert(colors.end(), begin, end);
            return *this;
        }

        MeshBuilder2D& addVertices(std::initializer_list<glm::vec2> init);
        MeshBuilder2D& addTextureCoords(std::initializer_list<glm::vec2> init);
        MeshBuilder2D& addColors(std::initializer_list<glm::vec3> init);

        void setVertices(const std::vector<glm::vec2> &vertices);
        void setTextureCoords(const std::vector<glm::vec2> &textureCoords);
        void setColors(const std::vector<glm::vec3> &colors);
        void setVertices(std::vector<glm::vec2>&& vertices);
        void setTextureCoords(std::vector<glm::vec2>&& textureCoords);
        void setColors(std::vector<glm::vec3>&& colors);


        void setVIndices(const std::vector<int> &v_indices);
        void setVTIndices(const std::vector<int> &vt_indices);
        void setVIndices(std::vector<int>&& v_indices);
        void setVTIndices(std::vector<int>&& vt_indices);

        const std::vector<glm::vec2>& getVertices() const;
        const std::vector<glm::vec2>& getTextureCoords() const;
        const std::vector<glm::vec3>& getColors() const;

        const std::vector<int>& getV_indices() const;
        const std::vector<int>& getVt_indices() const;

        std::vector<Vertex2D> toVertexArray(float scaleModif=1.0f);
        std::vector<Vertex2D> toVertexArrayIndexed(float scaleModif=1.0f);

        std::string info();
    };

    //usin
    
    /*
    class Material
    {
        std::string name;

        float ambient[3];
        float diffuse[3];
        float specular[3];
        float transmittance[3];
        float emission[3];
        float shininess;
        float ior;       // index of refraction
        float dissolve;  // 1 == opaque; 0 == fully transparent
        // illumination model (see http://www.fileformat.info/format/material/)
        int illum;

        int dummy;  // Suppress padding warning.

        std::string ambient_texname;             // map_Ka
        std::string diffuse_texname;             // map_Kd
        std::string specular_texname;            // map_Ks
        std::string specular_highlight_texname;  // map_Ns
        std::string bump_texname;                // map_bump, map_Bump, bump
        std::string displacement_texname;        // disp
        std::string alpha_texname;               // map_d
        std::string reflection_texname;          // refl

        texture_option_t ambient_texopt;
        texture_option_t diffuse_texopt;
        texture_option_t specular_texopt;
        texture_option_t specular_highlight_texopt;
        texture_option_t bump_texopt;
        texture_option_t displacement_texopt;
        texture_option_t alpha_texopt;
        texture_option_t reflection_texopt;

        // PBR extension
        // http://exocortex.com/blog/extending_wavefront_mtl_to_support_pbr
        real_t roughness;            // [0, 1] default 0
        real_t metallic;             // [0, 1] default 0
        real_t sheen;                // [0, 1] default 0
        real_t clearcoat_thickness;  // [0, 1] default 0
        real_t clearcoat_roughness;  // [0, 1] default 0
        real_t anisotropy;           // aniso. [0, 1] default 0
        real_t anisotropy_rotation;  // anisor. [0, 1] default 0
        real_t pad0;
        std::string roughness_texname;  // map_Pr
        std::string metallic_texname;   // map_Pm
        std::string sheen_texname;      // map_Ps
        std::string emissive_texname;   // map_Ke
        std::string normal_texname;     // norm. For normal mapping.

        texture_option_t roughness_texopt;
        texture_option_t metallic_texopt;
        texture_option_t sheen_texopt;
        texture_option_t emissive_texopt;
        texture_option_t normal_texopt;

        int pad2;

        std::map<std::string, std::string> unknown_parameter;
    };
    */
    

    class MeshBuilder {
    public:
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texcoords;
        std::vector<glm::vec3> colors;
        std::vector<int> v_indices;
        std::vector<int> vn_indices;
        std::vector<int> vt_indices;
        std::vector<int> vc_indices;

        // TODO
        //std::vector<tinyobj::material_t> materials;

    public:
        class Exporter3D : public MeshBuilderBase {
        protected:
            const MeshBuilder* builder;

        public: // Can only created by parent class
            Exporter3D(const MeshBuilder *builder);
            virtual ~Exporter3D() = default;

        public:
            Exporter3D& addVertex(bool indexed=false);
            Exporter3D& addTexture(bool indexed=false);
            Exporter3D& addColor(bool indexed=false);
            Exporter3D& addNormal(bool indexed=false);

        protected:
            virtual bool resolveTypes(std::vector<float> &data, size_t i) const;
        };

    public:
        MeshBuilder();
        MeshBuilder(
            const std::vector<glm::vec3> &vertices,
            const std::vector<glm::vec3> &normals,
            const std::vector<glm::vec2> &texcoords,
            const std::vector<int>& v_indices,
            const std::vector<int>& vn_indices,
            const std::vector<int>& vt_indices);
            
        static MeshBuilder fromOBJ(const std::string &file, const std::string &material);

        void clear();
        Exporter3D exporter() const;

        float maxExtent() const;
        float minExtent() const;

        // Values calculated as ABS
        float maxXExtent() const;
        float minXExtent() const;
        float maxYExtent() const;
        float minYExtent() const;
        float maxZExtent() const;
        float minZExtent() const;


        size_t maxExtentIndex() const;
        size_t minExtentIndex() const;

        size_t maxXExtentIndex() const;
        size_t minXExtentIndex() const;
        size_t maxYExtentIndex() const;
        size_t minYExtentIndex() const;
        size_t maxZExtentIndex() const;
        size_t minZExtentIndex() const;
        

        void scale(float scale);
        void unitize(float unitScale=1.0f);

        void setVertices(const std::vector<glm::vec3> &vertices);
        void setNormals(const std::vector<glm::vec3> &normals);
        void setTexCoords(const std::vector<glm::vec2> &texCoords);
        void setColors(const std::vector<glm::vec3> &colors);

        void setVIndices(const std::vector<int> &indices);
        void setVNIndices(const std::vector<int> &indices);
        void setVTIndices(const std::vector<int> &indices);
        void setVCIndices(const std::vector<int> &indices);

        const std::vector<glm::vec3>& getVertices() const;
        const std::vector<glm::vec3>& getNormals() const; 
        const std::vector<glm::vec2>& getTextureCoords() const;
        const std::vector<glm::vec3>& getColors() const;

        const std::vector<int>& getVIndices() const;
        const std::vector<int>& getVNIndices() const;
        const std::vector<int>& getVTIndices() const;
        const std::vector<int>& getVCIndices() const;

        std::vector<Vertex> toVertexArray();
        std::vector<PointVertex> toPointVertexArray();
        std::vector<NormalVertex> toNormalVertexArray();

        std::vector<Vertex> toVertexArrayIndexed();
        std::vector<PointVertex> toPointVertexArrayIndexed();
        std::vector<NormalVertex> toNormalVertexArrayIndexed();
    };

    class Triangle {
    public:
        Triangle();
        Triangle(const glm::vec3 v1,
            const glm::vec3 v2, const glm::vec3 v3);

        glm::vec3& v1();
        glm::vec3& v2();
        glm::vec3& v3();

        const glm::vec3& v1() const;
        const glm::vec3& v2() const;
        const glm::vec3& v3() const;

        glm::vec3 center() const;
        float area() const;
        glm::vec3 normal() const;

    protected:
        glm::vec3 k_v1, k_v2, k_v3;
    };

    class TriangleBuilder {
    public:
        void clear();

        void scale();
        void unify();
        void center();
        
        void addTriangle(const Triangle &triangle);
        void addTriangle(const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3);
    
        const std::vector<Triangle>& triangles();
    protected:
        std::vector<Triangle> k_triangles;
    };

    class DataBlob {
        void *k_data;
        size_t k_size;

    public:
        DataBlob();
        template<typename Type=char>
        DataBlob(size_t elements) {
            k_size = elements * sizeof(Type);
            k_data = new Type[elements];
        }

        template<typename Type>
        DataBlob(size_t elements, const Type& value) {
            k_size = elements * sizeof(Type);
            Type *vdata = new Type[elements];
            for (size_t i = 0; i < elements; i++)
                vdata[i] = value;
            k_data = vdata;
        }
        DataBlob(void *data, size_t size);

        ~DataBlob();
        DataBlob(const DataBlob&) = delete;
        DataBlob(DataBlob &&);

        DataBlob& operator=(const DataBlob&) = delete;
        DataBlob& operator=(DataBlob &&);

        inline size_t size() { return k_size; }

        template<typename Type>
        inline Type* as() { return static_cast<Type*>(k_data); }
    };

    class BitmapImage {
    protected:

        FIBITMAP *k_image = nullptr;

    public:
        BitmapImage() = default;
        BitmapImage(const std::string &type, const std::string &path);
        ~BitmapImage();

        BitmapImage(const BitmapImage &) = delete;
        BitmapImage(BitmapImage &&img);
        static BitmapImage loadPNG(const std::string &file);
        static BitmapImage loadJPG(const std::string &file);

        BitmapImage& operator=(const BitmapImage&) = delete;
        BitmapImage& operator=(BitmapImage&&);

        DataBlob exportImage();

        const unsigned char *data() const;
        size_t width() const;
        size_t height() const;
    };

    MeshBuilder loadMesh(const std::string &file, const std::string &material);
    MeshBuilder loadCube();
    MeshBuilder loadTriangle();
    MeshBuilder2D loadTriangle2D();
    MeshBuilder2D loadRect2D();

    std::vector<char> readFile(const std::string &file);

    //LTImage loadImage(const std::string &file);
}

#endif
