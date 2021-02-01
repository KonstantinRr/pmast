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

#include <engine/internal.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/norm.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>

NYREM_NAMESPACE_BEGIN

namespace detail {
    // Some dark template magic is happening in here...
    // WARNING:: It is better if you don't take a look
    // unless you are a master of the dark arkane forces.
    template<typename T> struct identity { typedef T type; };
    template<size_t Index> struct index { };

    template<size_t Size, typename Type,
        template<size_t, class> typename VertexType,
        template<size_t, class> typename... Args>
    struct DefaultComponent :
        public DefaultComponent<Size + 0, Type, VertexType>,
        public DefaultComponent<Size + 1, Type, Args...>
    {
        using ParentData = DefaultComponent<Size + 0, Type, VertexType>;
        using ParentRecursion = DefaultComponent<Size + 1, Type, Args...>;

        template<template<size_t, class> typename CheckType>
        bool hasTypeImpl() const {
            return DefaultComponent<Size + 0, Type, VertexType>::hasTypeImpl<CheckType>() ||
                ParentRecursion::hasTypeImpl<CheckType>();
        }
    };

    template<size_t Size, typename Type,
        template<size_t, class> typename VertexType>
    struct DefaultComponent<Size, Type, VertexType> :
        public VertexType<Size, Type>
    {
        template<template<size_t, class> typename CheckType>
        bool hasTypeImpl() const {
            return std::is_same<VertexType<Size, Type>, CheckType<Size, Type>>::value;
        }

        // Index Getters //
        inline VertexType<Size, Type>& getter(index<Size>) noexcept {
            return *static_cast<VertexType<Size, Type>*>(this);
        }
        inline const VertexType<Size, Type>& getter(index<Size>) const noexcept {
            return *static_cast<const VertexType<Size, Type>*>(this);
        }
        // Type Getters //
        inline VertexType<Size, Type>& getter(identity<VertexType<Size, Type>>) noexcept {
            return *static_cast<VertexType<Size, Type>*>(this);
        }
        inline const VertexType<Size, Type>& getter(identity<VertexType<Size, Type>>) const noexcept {
            return *static_cast<const VertexType<Size, Type>*>(this);
        }
    };
}

template<typename Type> class Spacer1{ Type data[1]; };
template<typename Type> class Spacer2{ Type data[2]; };
template<typename Type> class Spacer3{ Type data[3]; };
template<typename Type> class Spacer4{ Type data[4]; };

struct BaseComponent {
    inline void notImplemented(const char *result) {
        //throw std::runtime_error("BaseComponent: Not Implemented"s + result);
    }

    constexpr bool hasV2() const noexcept { return false; }
    constexpr bool hasV3() const noexcept { return false; }
    constexpr bool hasN2() const noexcept { return false; }
    constexpr bool hasN3() const noexcept { return false; }
    constexpr bool hasT2() const noexcept { return false; }
    constexpr bool hasT3() const noexcept { return false; }

    inline void setV2(vec2 vec) { notImplemented("V2"); }
    inline void setV3(vec3 vec) { notImplemented("V3"); }
    inline void setN2(vec2 vec) { notImplemented("N2"); }
    inline void setN3(vec3 vec) { notImplemented("N3"); }
    inline void setTX2(vec2 vec) { notImplemented("TX2"); }
    inline void setTX3(vec3 vec) { notImplemented("TX3"); }
};

template<typename Type, template<size_t, class> typename... Args>
struct GenericVertex :
    public detail::DefaultComponent<0, Type, Args...>
{
    //template<typename PType>
    //constexpr bool hasTypeImpl(detail::identity<PType>) const noexcept { return false; }

    template<template<size_t, class> typename CheckType>
    bool hasType() const {
        return detail::DefaultComponent<0, Type, Args...>
            ::hasTypeImpl<CheckType>();
    }

    // Type getters
    template<typename GetType>
    inline GetType& get() noexcept { return getter(detail::identity<GetType>()); }
    template<typename GetType>
    inline const GetType& get() const noexcept { return getter(detail::identity<GetType>()); }
    // Index getters
    template<size_t Index>
    inline auto& get() noexcept { return getter(detail::index<Index>()); }
    template<size_t Index>
    inline const auto& get() const noexcept { return getter(detail::index<Index>()); }
};

// definition of components //

template<size_t Size, typename Type>
struct VertexComponent2D : public BaseComponent {
    static constexpr size_t INDEX_X2 = 0, INDEX_Y2 = 1;
    Type dataV2[2];

    constexpr bool hasV2() const noexcept { return true; }

    VertexComponent2D() noexcept :
        dataV2{
            static_cast<Type>(0.0),
            static_cast<Type>(0.0)
        } { }
    VertexComponent2D(vec2 vec) noexcept :
        dataV2{ vec.x, vec.y } { }
    explicit VertexComponent2D(Type x, Type y) noexcept :
        dataV2{ x, y } { }

    inline vec2 asVecV2() const noexcept {
        return vec2(dataV2[INDEX_X2], dataV2[INDEX_Y2]);
    }

    inline void setV2(vec2 vec) noexcept {
        dataV2[INDEX_X2] = vec.x;
        dataV2[INDEX_Y2] = vec.y;
    }

    inline vec2 get() const noexcept { return asVecV2(); }
    inline void set(vec2 vec) noexcept { setV2(vec); }

    inline void setX2(Type x) const noexcept { dataV2[INDEX_X2] = x; }
    inline void setY2(Type y) const noexcept { dataV2[INDEX_Y2] = y; }

    inline Type x2() const noexcept { return dataV2[INDEX_X2]; }
    inline Type y2() const noexcept { return dataV2[INDEX_Y2]; }
};

using VertexComponent2DFloat = VertexComponent2D<0, float>;
using VertexComponent2DDouble = VertexComponent2D<0, double>;
using VertexComponent2DDefault = VertexComponent2DFloat;

template<size_t Size, typename Type>
struct TextureComponent2D : public BaseComponent {
    static constexpr size_t INDEX_TX2 = 0, INDEX_TY2 = 1;
    Type dataT2[2];

    TextureComponent2D() noexcept :
        dataT2 {
            static_cast<Type>(0),
            static_cast<Type>(0)
        } { }
    TextureComponent2D(vec2 vec) noexcept :
        dataT2{ vec.x, vec.y } { }
    explicit TextureComponent2D(Type tx, Type ty) noexcept :
        dataT2{ tx, ty } { }

    inline vec2 asVecT2() const noexcept {
        return vec2(dataT2[INDEX_TX2], dataT2[INDEX_TY2]);
    }

    inline void setT2(vec2 vec) noexcept {
        dataT2[INDEX_TX2] = vec.x;
        dataT2[INDEX_TY2] = vec.y;
    }

    void setTX2(Type tx) const noexcept { dataT2[INDEX_TX2] = tx; }
    void setTY2(Type ty) const noexcept { dataT2[INDEX_TY2] = ty; }
    
    inline Type tx2() const noexcept { return dataT2[INDEX_TX2]; }
    inline Type ty2() const noexcept { return dataT2[INDEX_TY2]; }
};

using TextureComponent2DFloat = TextureComponent2D<0, float>;
using TextureComponent2DDouble = TextureComponent2D<0, double>;
using TextureComponent2DDefault = TextureComponent2DFloat;

template<size_t Size, typename Type>
struct VertexComponent3D : public BaseComponent {
    static constexpr size_t
        INDEX_X3 = 0, INDEX_Y3 = 1, INDEX_Z3 = 2;
    Type dataV3[3];

    VertexComponent3D() noexcept :
        dataV3{
            static_cast<Type>(0),
            static_cast<Type>(0),
            static_cast<Type>(0)
        } { }
    VertexComponent3D(vec3 vec) noexcept :
        dataV3{ vec.x, vec.y, vec.z } { }
    explicit VertexComponent3D(Type x, Type y, Type z) noexcept :
        dataV3{ x, y, z} { }

    inline vec3 asVecV3() const noexcept {
        return vec3(dataV3[INDEX_X3], dataV3[INDEX_Y3], dataV3[INDEX_Z3]);
    }

    inline void setV3(vec3 vec) noexcept {
        dataV3[INDEX_X3] = vec.x;
        dataV3[INDEX_Y3] = vec.y;
        dataV3[INDEX_Z3] = vec.z;
    }

    inline void setX3(Type x) noexcept { dataV3[INDEX_X3] = x; }
    inline void setY3(Type y) noexcept { dataV3[INDEX_Y3] = y; }
    inline void setZ3(Type z) noexcept { dataV3[INDEX_Z3] = z; }

    inline Type x3() const noexcept { return dataV3[INDEX_X3]; }
    inline Type y3() const noexcept { return dataV3[INDEX_Y3]; }
    inline Type z3() const noexcept { return dataV3[INDEX_Z3]; }
};

using VertexComponent3DFloat = VertexComponent3D<0, float>;
using VertexComponent3DDouble = VertexComponent3D<0, double>;
using VertexComponent3DDefault = VertexComponent3DFloat;

template<size_t Size, typename Type>
struct NormalComponent3D {
    static constexpr size_t
        INDEX_NX3 = 0, INDEX_NY3 = 1, INDEX_NZ3 = 2;
    Type dataN3[3];

    NormalComponent3D() noexcept :
        dataN3{
            static_cast<Type>(0),
            static_cast<Type>(0),
            static_cast<Type>(0)
        } { }
    NormalComponent3D(vec3 vec) noexcept :
        dataN3{vec.x, vec.y, vec.z} { }
    explicit NormalComponent3D(Type nx, Type ny, Type nz) noexcept :
        dataN3{nx, ny, nz} { }

    inline vec3 asVecN3() const noexcept {
        return vec3(dataN3[INDEX_NX3], dataN3[INDEX_NY3], dataN3[INDEX_NZ3]);
    }

    inline void setN3(vec3 n) noexcept {
        dataN3[INDEX_NX3] = n.x;
        dataN3[INDEX_NY3] = n.y;
        dataN3[INDEX_NZ3] = n.z;
    }

    inline void setNX3(Type val) noexcept { dataN3[INDEX_NX3] = val; }
    inline void setNY3(Type val) noexcept { dataN3[INDEX_NY3] = val; }
    inline void setNZ3(Type val) noexcept { dataN3[INDEX_NZ3] = val; }

    inline Type nx() const noexcept { return dataN3[INDEX_NX3]; }
    inline Type ny() const noexcept { return dataN3[INDEX_NY3]; }
    inline Type nz() const noexcept { return dataN3[INDEX_NZ3]; }
};

using NormalComponent3DFloat = NormalComponent3D<0, float>;
using NormalComponent3DDouble = NormalComponent3D<0, double>;
using NormalComponent3DDefault = NormalComponent3DFloat;

using IndexType = unsigned int;

class Triangulizable2D {
public:
    virtual ~Triangulizable2D() = default;

    virtual std::vector<vec2> generateMesh() const noexcept = 0;
    virtual std::vector<vec2> generateStripeMesh() const noexcept = 0;

    virtual std::pair<std::vector<vec2>, std::vector<IndexType>>
        generateIndexedMesh() const noexcept = 0;
    virtual std::pair<std::vector<vec2>, std::vector<IndexType>>
        generateIndexedStripeMesh() const noexcept = 0;
};

class Textureable {
public:
    virtual ~Textureable() = default;

    virtual std::vector<vec2> generateTextueCoords() const noexcept = 0;
    //virtual std
};

//template<typename Type, template<class> typename... Args>


template<typename Type>
class Triangle3D {
public:
    Triangle3D() noexcept :
    	k_v1(-1.0f, -1.0f, -1.0f),
	    k_v2(1.0f, 1.0f, 1.0f),
	    k_v3(-1.0f, 1.0f, 1.0f) { }
    Triangle3D(const vec3 v1, const vec3 v2, const vec3 v3) noexcept :
        k_v1(v1), k_v2(v2), k_v3(v3) { };

    inline void setV1(vec3 v1) noexcept { k_v1 = v1; }
    inline void setV2(vec3 v2) noexcept { k_v2 = v2; }
    inline void setV3(vec3 v3) noexcept { k_v3 = v3; }

    inline const vec3 v1() const noexcept { return k_v1; }
    inline const vec3 v2() const noexcept { return k_v2; }
    inline const vec3 v3() const noexcept { return k_v3; }

    constexpr size_t size() const noexcept{  return 3; }

    template<typename VertexType>
    void exportMesh(std::vector<VertexType>& meshObject) const noexcept {
        meshObject.reserve(meshObject.size() + size());

        Type vertex;
        // normal coordinate is the same for all vertices
        vertex.setN3(normal());

        vertex.setV3(k_v1);
        vertex.setT2({0.0f, 0.0f});
        meshObject.push_back(vertex);
        vertex.setV3(k_v2);
        vertex.setT2({1.0f, 1.0f});
        meshObject.push_back(vertex);
        vertex.setV3(k_v3);
        vertex.setT2({0.0f, 1.0f});
        meshObject.push_back(vertex);
    }

    template<typename VertexType>
    void exportStripeMesh(std::vector<VertexType>& meshObject) const noexcept {
        // triangle strip is the same as default triangle
        exportMesh<Type>(meshObject);
    }

    vec3 center() const noexcept {
        return {
	    	(k_v1.x + k_v2.x + k_v3.x) / 3.0f,
	    	(k_v1.y + k_v2.y + k_v3.y) / 3.0f,
	    	(k_v1.z + k_v2.z + k_v3.z) / 3.0f
	    };
    }
    float area() const noexcept {
        const vec3 ab = k_v3 - k_v1;
	    const vec3 ac = k_v2 - k_v1;
	    float dist = sqrtf(glm::length2(ab) * glm::length2(ac));
	    float cosVal = glm::dot(ab, ac) / dist;
	    return 0.5 * dist * sqrtf(1.0 - cosVal * cosVal);
    }

    vec3 normal() const noexcept {
        return glm::normalize(glm::cross(k_v3 - k_v1, k_v2 - k_v1));
    }
    vec3 inverseNormal() const noexcept { 
        return -normal();
    }

protected:
    vec3 k_v1, k_v2, k_v3;
};

template<typename Type>
class Circle2D  {
protected:
    float m_radius;
    vec2 pos;

public:
    Circle2D() noexcept : 
        m_radius(static_cast<Type>(1)),
        pos(static_cast<Type>(0)) { }
    Circle2D(Type radius) noexcept :
        m_radius(radius),
        pos(static_cast<Type>(0)) { }
    Circle2D(float radius, vec2 position) noexcept :
        m_radius(radius), pos(position) { }
    Circle2D(float radius, Type x, Type y) noexcept :
        m_radius(radius, {x, y}) { }

    template<typename VertexType> 
    void exportMesh(std::vector<VertexType> &meshObject) const noexcept {

    }

    template<typename VertexType> 
    void exportStripeMesh(std::vector<VertexType> &meshObject) const noexcept {

    }
};

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
    EXPORT_NONE,
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

    size_t size() const noexcept;

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
        vertices.reserve(vertices.size() + std::distance(begin, end));
        vertices.insert(vertices.end(), begin, end);
        return *this;
    }
    template<typename Iterator>
    MeshBuilder2D& addTextureCoords(Iterator begin, Iterator end) {
        texCoords.reserve(texCoords.size() + std::distance(begin, end));
        texCoords.insert(texCoords.end(), begin, end);
        return *this;
    }
    template<typename Iterator>
    MeshBuilder2D& addColors(Iterator begin, Iterator end) {
        colors.reserve(colors.size() + std::distance(begin, end));
        colors.insert(colors.end(), begin, end);
        return *this;
    }

    MeshBuilder2D& addMesh(const MeshBuilder2D &mesh) noexcept;
    MeshBuilder2D& addCircle(size_t pcount, float radius, bool strip=false);
    MeshBuilder2D& addRect(float x, float y, float width, float height);
    MeshBuilder2D& addPolygon(
        const std::vector<vec2> &polygon,
        const std::vector<std::vector<vec2>> &holes = {}, bool indexed=false);

    MeshBuilder2D& addVertices(std::initializer_list<glm::vec2> init);
    MeshBuilder2D& addTextureCoords(std::initializer_list<glm::vec2> init);
    MeshBuilder2D& addColors(std::initializer_list<glm::vec3> init);

    void setVertices(const std::vector<glm::vec2> &vertices);
    void setTextureCoords(const std::vector<glm::vec2> &textureCoords);
    void setColors(const std::vector<glm::vec3> &colors);
    void setVertices(std::vector<glm::vec2>&& vertices);
    void setTextureCoords(std::vector<glm::vec2>&& textureCoords);
    void setColors(std::vector<glm::vec3>&& colors);

    void generateDefaultIndices();
    void generateNormals(bool indexed=false);

    void setVIndices(const std::vector<int> &v_indices);
    void setVTIndices(const std::vector<int> &vt_indices);
    void setVIndices(std::vector<int>&& v_indices);
    void setVTIndices(std::vector<int>&& vt_indices);

    const std::vector<glm::vec2>& getVertices() const;
    const std::vector<glm::vec2>& getTextureCoords() const;
    const std::vector<glm::vec3>& getColors() const;

    const std::vector<int>& getV_indices() const;
    const std::vector<int>& getVc_indices() const;
    const std::vector<int>& getVt_indices() const;


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
    using IndexType = unsigned int;

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

    void generateNormals(bool indexed=false, bool side=true) noexcept;
    void generateDefaultIndices();

    const std::vector<glm::vec3>& getVertices() const;
    const std::vector<glm::vec3>& getNormals() const; 
    const std::vector<glm::vec2>& getTextureCoords() const;
    const std::vector<glm::vec3>& getColors() const;

    const std::vector<int>& getVIndices() const;
    const std::vector<int>& getVNIndices() const;
    const std::vector<int>& getVTIndices() const;
    const std::vector<int>& getVCIndices() const;
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


MeshBuilder loadMesh(const std::string &file, const std::string &material);
MeshBuilder loadCube();
MeshBuilder loadTriangle();
MeshBuilder2D loadTriangle2D();
MeshBuilder2D loadRect2D();

std::vector<char> readFile(const std::string &file);

//LTImage loadImage(const std::string &file);


NYREM_NAMESPACE_END

#endif
