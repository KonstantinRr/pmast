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
    template<size_t Index> struct index { };

    template<size_t Size,
        template<size_t> typename VertexType,
        template<size_t> typename... Args>
    struct DefaultComponent :
        public DefaultComponent<Size + 0, VertexType>,
        public DefaultComponent<Size + 1, Args...>
    {
        using ParentData = DefaultComponent<Size + 0, VertexType>;
        using ParentRecursion = DefaultComponent<Size + 1, Args...>;

        /// Checks whether this or a parent object is of type CheckType
        template<template<size_t> typename CheckType>
        static constexpr bool hasComponent() noexcept {
            return ParentData::template hasComponent<CheckType>() ||
                ParentRecursion::template hasComponent<CheckType>();
        }

        /// Finds the component index that is of type CheckType
        template<template<size_t> typename CheckType>
        static constexpr int componentIndex() noexcept {
            int result = ParentData::template componentIndex<CheckType>();
            if (result >= 0) return result;
            return ParentRecursion::template componentIndex<CheckType>();
        }

        // const version
        template<template<size_t> typename CheckType>
        constexpr inline const auto& component() const noexcept {
            if constexpr(ParentData::template hasComponent<CheckType>())
                return ParentData::template componentAt<CheckType>();
            else
                return ParentRecursion::template hasComponent<CheckType>();
        }

        // non const version
        template<template<size_t> typename CheckType>
        constexpr inline auto& component() noexcept {
            if constexpr(ParentData::template hasComponent<CheckType>())
                return ParentData::template component<CheckType>();
            else
                return ParentRecursion::template component<CheckType>();
        }
    };

    template<size_t Size, template<size_t> typename VertexType>
    struct DefaultComponent<Size, VertexType> :
        public VertexType<Size>
    {
        using ExplicitVertexType = VertexType<Size>;

        /// Checks whether this or a parent object is of type CheckType
        /// NOTE: See the general information at hasComponent()
        template<template<size_t> typename CheckType>
        static constexpr inline bool hasComponent() noexcept {
            return std::is_same<VertexType<Size>, CheckType<Size>>::value;
        }

        /// Finds the component index that is of type CheckType
        /// NOTE: See the general information at componentIndex()
        template<template<size_t> typename CheckType>
        static constexpr inline int componentIndex() noexcept {
            if (!hasComponent<CheckType>()) return -1;
            return static_cast<int>(Size);
        }

        // non const version
        template<template<size_t> typename CheckType>
        constexpr inline const VertexType<Size>& component() const noexcept {
            static_assert(hasComponent<CheckType>(), "No Component of given type");
            return *static_cast<const VertexType<Size>*>(this);
        }
        // const version
        template<template<size_t> typename CheckType>
        constexpr inline VertexType<Size>& component() noexcept {
            static_assert(hasComponent<CheckType>(), "No Component of given type");
            return *static_cast<VertexType<Size>*>(this);
        }
    };
}

template<size_t Size, typename Type, size_t Padding>
class Spacer{ Type data[Padding]; };

template<size_t Size> using Spacer1Float = Spacer<Size, float, 1>;
template<size_t Size> using Spacer2Float = Spacer<Size, float, 2>;
template<size_t Size> using Spacer3Float = Spacer<Size, float, 3>;
template<size_t Size> using Spacer4Float = Spacer<Size, float, 4>;

template<template<size_t> typename... Args>
struct GenericVertex :
    public detail::DefaultComponent<0, Args...>
{
    using ThisType = GenericVertex<Args...>;
    using GenericVertexType = ThisType;

    using Parent = detail::DefaultComponent<0, Args...>;

    // Special functions
    template<template<size_t> typename VertexType, typename Arg>
    constexpr inline void assignIf(Arg&& arg) noexcept {
        if constexpr(hasComponent<VertexType>())
            component<VertexType>() = std::forward<Arg>(arg);
    }
};

// definition of components //
template<size_t Size, typename Type>
struct VertexComponent2D {
    using ThisType = VertexComponent2D<Size, Type>;
    using VertexComponent2DType = ThisType;

    constexpr static size_t INDEX_X2 = 0, INDEX_Y2 = 1;
    Type dataV2[2];

    constexpr bool hasV2() const noexcept { return true; }

    constexpr VertexComponent2D() noexcept :
        dataV2{
            static_cast<Type>(0.0),
            static_cast<Type>(0.0)
        } { }
    constexpr VertexComponent2D(vec2 vec) noexcept :
        dataV2{ vec.x, vec.y } { }
    constexpr explicit VertexComponent2D(Type x, Type y) noexcept :
        dataV2{ x, y } { }

    // Vector Get
    constexpr inline vec2 getV2() const noexcept {
        return vec2(dataV2[INDEX_X2], dataV2[INDEX_Y2]);
    }
    constexpr inline vec2 get() const noexcept { return getV2(); }

    // Vector Set
    constexpr inline void setV2(vec2 vec) noexcept {
        dataV2[INDEX_X2] = vec.x;
        dataV2[INDEX_Y2] = vec.y;
    }
    constexpr inline void operator=(vec2 vec) noexcept {
        setV2(vec);
    }

    // X, Y set
    constexpr inline void setX2(Type x) const noexcept { dataV2[INDEX_X2] = x; }
    constexpr inline void setY2(Type y) const noexcept { dataV2[INDEX_Y2] = y; }
    // X, Y get
    constexpr inline Type x2() const noexcept { return dataV2[INDEX_X2]; }
    constexpr inline Type y2() const noexcept { return dataV2[INDEX_Y2]; }
};

template<size_t Size> using VertexComponent2DFloat = VertexComponent2D<Size, float>;
template<size_t Size> using VertexComponent2DDouble = VertexComponent2D<Size, double>;
template<size_t Size> using VertexComponent2DDefault = VertexComponent2DFloat<Size>;

template<size_t Size, typename Type>
struct TextureComponent2D {
    using ThisType = TextureComponent2D<Size, Type>;
    using TextureComponent2DType = ThisType;

    constexpr static size_t INDEX_TX2 = 0, INDEX_TY2 = 1;
    vec2 dataT2;

    constexpr TextureComponent2D() noexcept :
        dataT2(static_cast<Type>(0)) { }
    constexpr TextureComponent2D(vec2 vec) noexcept :
        dataT2(vec) { }
    constexpr explicit TextureComponent2D(Type tx, Type ty) noexcept :
        dataT2(tx, ty) { }

    // Vector get
    constexpr inline vec2 getT2() const noexcept {
        return dataT2;
    }
    constexpr inline vec2 get() const noexcept {
        return dataT2;
    }

    // Vector set
    constexpr inline void setT2(vec2 vec) noexcept {
        dataT2 = vec;
    }

    constexpr inline ThisType operator=(vec2 vec) noexcept {
        dataT2 = vec;
        return *this;
    }
    constexpr inline ThisType operator+=(vec2 vec) noexcept {
        dataT2 += vec;
        return *this;
    }
    constexpr inline ThisType operator-=(vec2 vec) noexcept {
        dataT2 -= vec;
        return *this;
    }

    // TX, TY set
    constexpr void setTX2(Type tx) const noexcept { dataT2.x = tx; }
    constexpr void setTY2(Type ty) const noexcept { dataT2.y = ty; }
    // TX, TY get
    constexpr inline Type tx2() const noexcept { return dataT2.x; }
    constexpr inline Type ty2() const noexcept { return dataT2.y; }
};

template<size_t Size> using TextureComponent2DFloat = TextureComponent2D<Size, float>;
template<size_t Size> using TextureComponent2DDouble = TextureComponent2D<Size, double>;
template<size_t Size> using TextureComponent2DDefault = TextureComponent2DFloat<Size>;

template<size_t Size, typename Type>
struct VertexComponent3D {
    using ThisType = VertexComponent3D<Size, Type>;
    using VertexComponent3DType = ThisType;

    constexpr static size_t
        INDEX_X3 = 0, INDEX_Y3 = 1, INDEX_Z3 = 2;
    vec3 dataV3;

    constexpr VertexComponent3D() noexcept :
        dataV3(static_cast<Type>(0.0f)) { }
    constexpr VertexComponent3D(vec3 vec) noexcept :
        dataV3(vec) { }
    constexpr explicit VertexComponent3D(Type x, Type y, Type z) noexcept :
        dataV3(x, y, z) { }

    // Vector get
    constexpr inline vec3 getV3() const noexcept {
        return dataV3;
    }
    constexpr inline vec3 get() const noexcept {
        return dataV3;
    }

    // Vector set
    constexpr inline void setV3(vec3 vec) noexcept {
        dataV3 = vec;
    }
    constexpr inline ThisType& operator=(vec3 vec) noexcept {
        dataV3 = vec;
        return *this;
    }
    constexpr inline ThisType& operator+=(vec3 vec) noexcept {
        dataV3 += vec;
        return *this;
    }
    constexpr inline ThisType& operator-=(vec3 vec) noexcept {
        dataV3 -= vec;
        return *this;
    }

    // X, Y, Z set
    constexpr inline void setX3(Type x) noexcept { dataV3.x = x; }
    constexpr inline void setY3(Type y) noexcept { dataV3.y = y; }
    constexpr inline void setZ3(Type z) noexcept { dataV3.z = z; }
    // X, Y, Z get
    constexpr inline Type x3() const noexcept { return dataV3.x; }
    constexpr inline Type y3() const noexcept { return dataV3.y; }
    constexpr inline Type z3() const noexcept { return dataV3.z; }
};

template<size_t Size> using VertexComponent3DFloat = VertexComponent3D<Size, float>;
template<size_t Size> using VertexComponent3DDouble = VertexComponent3D<Size, double>;
template<size_t Size> using VertexComponent3DDefault = VertexComponent3DFloat<Size>;

template<size_t Size, typename Type>
struct NormalComponent3D {
    using ThisType = NormalComponent3D<Size, Type>;
    using NormalComponent3DType = ThisType;

    constexpr static size_t INDEX_NX3 = 0, INDEX_NY3 = 1, INDEX_NZ3 = 2;
    vec3 dataN3;

    constexpr NormalComponent3D() noexcept :
        dataN3(static_cast<Type>(0)) { }
    constexpr NormalComponent3D(vec3 vec) noexcept :
        dataN3(vec.x, vec.y, vec.z) { }
    constexpr explicit NormalComponent3D(Type nx, Type ny, Type nz) noexcept :
        dataN3(nx, ny, nz) { }

    // Vec Get
    constexpr inline vec3 getN3() const noexcept {
        return dataN3;
    }
    constexpr inline vec3 get() const noexcept {
        return dataN3;
    }

    // Vec Set
    constexpr inline void setN3(vec3 n) noexcept {
        dataN3 = n;
    }

    constexpr inline ThisType& operator=(vec3 n) noexcept {
        dataN3 = n;
        return *this;
    }
    constexpr inline ThisType& operator+=(vec3 n) noexcept {
        dataN3 += n;
        return *this;
    }
    constexpr inline ThisType& operator-=(vec3 n) noexcept {
        dataN3 -= n;
        return *this;
    }

    // NX, NY, NZ set
    constexpr inline void setNX3(Type val) noexcept { dataN3.x = val; }
    constexpr inline void setNY3(Type val) noexcept { dataN3.y = val; }
    constexpr inline void setNZ3(Type val) noexcept { dataN3.z = val; }
    // NX, NY, NZ get
    inline Type nx() const noexcept { return dataN3.x; }
    inline Type ny() const noexcept { return dataN3.y; }
    inline Type nz() const noexcept { return dataN3.z; }
};

template<size_t Size> using NormalComponent3DFloat = NormalComponent3D<Size, float>;
template<size_t Size> using NormalComponent3DDouble = NormalComponent3D<Size, double>;
template<size_t Size> using NormalComponent3DDefault = NormalComponent3DFloat<Size>;

// Supplies helpers for dealing with GenericVertex
template<typename Type, typename VertexType>
struct ComponentType {
    static constexpr bool
        hasTexture2D = VertexType::template hasComponent<TextureComponent2DFloat>(),
        hasTexture3D = false, // 3D textures are currently not supported
        hasVertex2D = VertexType::template hasComponent<VertexComponent2DFloat>(),
        hasVertex3D = VertexType::template hasComponent<VertexComponent3DFloat>(),
        hasNormal2D = false, // 2D normals are currently not supported
        hasNormal3D = VertexType::template hasComponent<NormalComponent3DFloat>();
    static constexpr int
        locTexture2D = VertexType::template componentIndex<TextureComponent2DFloat>(),
        locTexture3D = -1, // 3D textures are currently not supported
        locVertex2D = VertexType::template componentIndex<VertexComponent2DFloat>(),
        locVertex3D = VertexType::template componentIndex<VertexComponent3DFloat>(),
        locNormal2D = -1, // 2D normals are currently not supported
        locNormal3D = VertexType::template componentIndex<NormalComponent3DFloat>();

    static constexpr inline void setT2If(VertexType &tp, vec2 vec) noexcept { if constexpr(hasTexture2D) tp.setT2(vec); }
    static constexpr inline void setT3If(VertexType &tp, vec3 vec) noexcept { if constexpr(hasTexture3D) tp.setT3(vec); }
    static constexpr inline void setV2If(VertexType &tp, vec2 vec) noexcept { if constexpr(hasVertex2D) tp.setV2(vec); }
    static constexpr inline void setV3If(VertexType &tp, vec3 vec) noexcept { if constexpr(hasVertex3D) tp.setV3(vec); }
    static constexpr inline void setN2If(VertexType &tp, vec2 vec) noexcept { if constexpr(hasNormal2D) tp.setN2(vec); }
    static constexpr inline void setN3If(VertexType &tp, vec3 vec) noexcept { if constexpr(hasNormal3D) tp.setN3(vec); }
};

using IndexType = unsigned int;

template<typename Type, template<size_t> typename... Components> 
class PolygonTypes {
public:
    using VertexType = GenericVertex<Components...>;
    using VComponentType = ComponentType<float, VertexType>;

    static constexpr vec2
        TEXTURE_V1 = vec2(static_cast<Type>(0), static_cast<Type>(0)),
        TEXTURE_V2 = vec2(static_cast<Type>(1), static_cast<Type>(0)),
        TEXTURE_V3 = vec2(static_cast<Type>(1), static_cast<Type>(1)),
        TEXTURE_V4 = vec2(static_cast<Type>(0), static_cast<Type>(1));

    static void setVertex(VertexType& m_v, vec2 v, vec2 t) noexcept {
        VComponentType::setV3If(m_v, vec3(v, 0.0f));
        VComponentType::setV2If(m_v, v);
        VComponentType::setN3If(m_v, {0.0f, 1.0f, 0.0f}); // up
        VComponentType::setT2If(m_v, t);
    }

    static void setVertex(VertexType& m_v, vec3 v, vec3 n, vec2 t) noexcept {
        VComponentType::setV3If(m_v, v);
        VComponentType::setV2If(m_v, {v.x, v.y});
        VComponentType::setN3If(m_v, n);
        VComponentType::setT2If(m_v, t);
    }


    static constexpr bool use3D =
        VComponentType::hasVertex3D && (!VComponentType::hasVertex2D ||
        VComponentType::locVertex3D < VComponentType::locVertex2D);
    static constexpr bool use2D =
        VComponentType::hasVertex2D && (!VComponentType::hasVertex3D ||
        VComponentType::locVertex2D < VComponentType::locVertex3D);

    static void checkVertexComponent() noexcept {
        static_assert(
            VComponentType::hasVertex3D | VComponentType::hasVertex2D,
            "Cannot calculate function without vertex component.");
    }
};

//template<typename Type, template<class> typename... Args>
template<typename Type, template<size_t> typename... Components>
class Triangle : public PolygonTypes<Type, Components...> {
public:
    using ParentPolygonTypes = PolygonTypes<Type, Components...>;
    using VertexType = typename ParentPolygonTypes::VertexType;
    using VComponentType = typename ParentPolygonTypes::VComponentType;
protected:
    VertexType m_vertices[3];

public:
    Triangle() noexcept {
        ParentPolygonTypes::setVertex(m_vertices[0], {0, 0}, ParentPolygonTypes::TEXTURE_V1);
        ParentPolygonTypes::setVertex(m_vertices[1], {1, 0}, ParentPolygonTypes::TEXTURE_V2);
        ParentPolygonTypes::setVertex(m_vertices[2], {1, 1}, ParentPolygonTypes::TEXTURE_V3);
    }

    Triangle(vec2 v1, vec2 v2, vec2 v3) noexcept :
        Triangle(v1, v2, v3,
            ParentPolygonTypes::TEXTURE_V1,
            ParentPolygonTypes::TEXTURE_V2,
            ParentPolygonTypes::TEXTURE_V3) { }
    Triangle(vec2 v1, vec2 v2, vec2 v3, vec2 t1, vec2 t2, vec2 t3) noexcept {
        ParentPolygonTypes::setVertex(m_vertices[0], v1, t1);
        ParentPolygonTypes::setVertex(m_vertices[1], v2, t2);
        ParentPolygonTypes::setVertex(m_vertices[2], v3, t3);
    }
    
    Triangle(vec3 v1, vec3 v2, vec3 v3) noexcept :
        Triangle(v1, v2, v3,
            ParentPolygonTypes::TEXTURE_V1,
            ParentPolygonTypes::TEXTURE_V2,
            ParentPolygonTypes::TEXTURE_V3) { }
    Triangle(vec3 v1, vec3 v2, vec3 v3, vec2 t1, vec2 t2, vec2 t3) noexcept {
        vec3 n = glm::normalize(glm::cross(v3 - v1, v2 - v1));
        ParentPolygonTypes::setVertex(m_vertices[0], v1, n, t1);
        ParentPolygonTypes::setVertex(m_vertices[1], v2, n, t2);
        ParentPolygonTypes::setVertex(m_vertices[2], v3, n, t3);
    }

    constexpr size_t size() const noexcept { return 3; }

    constexpr inline void setV1(const VertexType &val) noexcept { m_vertices[0] = val; }
    constexpr inline void setV2(const VertexType &val) noexcept { m_vertices[1] = val; }
    constexpr inline void setV3(const VertexType &val) noexcept { m_vertices[2] = val; }

    constexpr inline const VertexType& v1() const noexcept { return m_vertices[0]; }
    constexpr inline const VertexType& v2() const noexcept { return m_vertices[1]; }
    constexpr inline const VertexType& v3() const noexcept { return m_vertices[2]; }

    constexpr Type area() const noexcept {
        ParentPolygonTypes::checkVertexComponent();
        if constexpr(ParentPolygonTypes::use3D)
        {
            const vec3 ab = m_vertices[2].getV3() - m_vertices[0].getV3();
	        const vec3 ac = m_vertices[1].getV3() - m_vertices[0].getV3();
	        float dist = sqrt(glm::length2(ab) * glm::length2(ac));
	        float cosVal = glm::dot(ab, ac) / dist;
	        return 0.5 * dist * sqrtf(1.0 - cosVal * cosVal);
        }
        else if constexpr(ParentPolygonTypes::use2D)
        {
            const vec2& pv1 = m_vertices[0].getV2();
            const vec2& pv2 = m_vertices[1].getV2();
            const vec2& pv3 = m_vertices[2].getV2();
            Type area =
                pv1.x * (pv2.y - pv3.x) +
                pv2.x * (pv3.y - pv1.y) + 
                pv3.x * (pv1.y - pv2.y);
            area = area / 2;
            // returns the ABS of this side
            return area < 0 ? -area : area;
        } else {
            return 0;
        }
    }

    constexpr auto center() const noexcept {
        ParentPolygonTypes::checkVertexComponent();
        if constexpr(ParentPolygonTypes::use3D)
        {
            return (m_vertices[0].getV3() + m_vertices[1].getV3() + m_vertices[2].getV3())
                / static_cast<Type>(3);
        }
        else if constexpr(ParentPolygonTypes::use2D)
        {
            return (m_vertices[0].getV2() + m_vertices[1].getV2() + m_vertices[2].getV2())
                / static_cast<Type>(3);
        }
    }

    vec3 normal() const noexcept {
        ParentPolygonTypes::checkVertexComponent();
        if constexpr(ParentPolygonTypes::use3D) {
            const vec3 ab = m_vertices[2].getV3() - m_vertices[0].getV3();
	        const vec3 ac = m_vertices[1].getV3() - m_vertices[0].getV3();
            return glm::normalize(glm::cross(ab, ac));
        } else if constexpr(ParentPolygonTypes::use2D) {
            return {0.0f, 1.0f, 0.0f};
        }
    }
    vec3 inverseNormal() const noexcept { 
        return -normal();
    }
};

template<typename Type, template<size_t> typename... Components>
class Plane : public PolygonTypes<Type, Components...> {
public:
    using ThisType = Plane<Type, Components...>;
    using PlaneType = ThisType;

    using ParentPolygonTypes = PolygonTypes<Type, Components...>;
    using VertexType = typename ParentPolygonTypes::VertexType;
    using VComponentType = typename ParentPolygonTypes::VComponentType;
protected:
    VertexType m_vertices[6]; 

public:
    constexpr Plane() noexcept : Plane(vec2(0.0f), vec2(1.0f)) { }

    constexpr Plane(float x, float y) noexcept : Plane({x, y}, vec2(1.0f)) { }
    constexpr Plane(float x, float y, float width, float height) noexcept :
        Plane({ x, y }, { width, height }) { }
    constexpr Plane(vec2 center, vec2 dimensions) noexcept {
        using ParentPolygonTypes::setVertex;
        // first triangle
        setVertex(m_vertices[0], center + vec2(-dimensions.x, -dimensions.y), ParentPolygonTypes::TEXTURE_V1);
        setVertex(m_vertices[1], center + vec2( dimensions.x, -dimensions.y), ParentPolygonTypes::TEXTURE_V2);
        setVertex(m_vertices[2], center + vec2( dimensions.x,  dimensions.y), ParentPolygonTypes::TEXTURE_V3);
        // second triangle
        m_vertices[3] = m_vertices[0];
        m_vertices[4] = m_vertices[2];
        setVertex(m_vertices[5], center + vec2(-dimensions.x,  dimensions.y), ParentPolygonTypes::TEXTURE_V4);
    }

    constexpr Plane(float x, float y, float z, float px, float py, float pz) noexcept :
        Plane({x, y, z}, {px, py, pz}) { }
    constexpr Plane(vec3 center, vec3 corner) noexcept {
        vec3 vOpposite = center - corner;
        // TODO
        //glm::normalize(glm::cross(center + corner));
    }

    void move(vec3 vec) noexcept {
        ParentPolygonTypes::checkVertexComponent();
        if constexpr(ParentPolygonTypes::use3D) {
            for (size_t i = 0; i < 6; i++)
                m_vertices[i].getV3() += vec;
        } else if constexpr(ParentPolygonTypes::use2D) {
            vec2 nvec = { vec.x, vec.y };
            for (size_t i = 0; i < 6; i++)
                m_vertices[i].getV2() += nvec;
        }
    }

    void move(vec2 vec) noexcept {
        move(vec3(vec, 0.0f));
    }
};


template<typename Type, template<size_t> typename... Components>
class Cube : public PolygonTypes<Type, Components...> {
public:
    using ThisType = Cube<Type, Components...>;
    using CubeType = ThisType;

    using PlaneType = Plane<Type, Components...>;
protected:
    PlaneType planes[6];

    constexpr Cube() noexcept : Cube(
        vec3(static_cast<Type>(0)), static_cast<Type>(1)) { }
    constexpr Cube(vec3 center, Type width) noexcept {

    }
};

template<typename Type>
class Circle2D  {
protected:
    float m_radius;
    vec2 m_pos;

public:
    constexpr Circle2D() noexcept : 
        m_radius(static_cast<Type>(1)),
        m_pos(static_cast<Type>(0)) { }
    constexpr Circle2D(Type radius) noexcept :
        m_radius(radius),
        m_pos(static_cast<Type>(0)) { }
    constexpr Circle2D(float radius, vec2 position) noexcept :
        m_radius(radius), m_pos(position) { }
    constexpr Circle2D(float radius, Type x, Type y) noexcept :
        m_radius(radius, {x, y}) { }

    template<typename VertexType> 
    void exportMesh(std::vector<VertexType> &meshObject) const noexcept {

    }

    template<typename VertexType> 
    void exportStripeMesh(std::vector<VertexType> &meshObject) const noexcept {

    }
};

template<typename Type>
class Circle3D {
protected:
    vec2 m_center;
    vec2 m_outer;

public:
    constexpr Circle3D() noexcept :
        m_center(0.0f), m_outer(1.0f, 0.0f) { }
    constexpr Circle3D(vec2 center, vec2 outer) noexcept :
        m_center(center), m_outer(outer) { }

    constexpr inline vec2 center() const noexcept { return m_center; }

    constexpr inline void setCenter(vec2 vec) noexcept {
        m_center = vec;
    }

    template<typename VertexType>
    void exportMesh(std::vector<VertexType> &meshObject) const noexcept {

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

struct {
    const char *exportName;
    size_t size;
} exportNames[] = {
    { "EXPORT_NONE", 0 },
    { "EXPORT_VERTEX", 2 },
    { "EXPORT_VERTEX_INDEXED", 2 },
    { "EXPORT_TEXTURE", 2 },
    { "EXPORT_TEXTURE_INDEXED", 2 },
    { "EXPORT_COLOR", 3 },
    { "EXPORT_COLOR_INDEXED", 3 },
    { "EXPORT_VERTEX_3D", 3 },
    { "EXPORT_VERTEX_INDEXED_3D", 3 },
    { "EXPORT_NORMAL_3D", 3 },
    { "EXPORT_NORMAL_INDEXED_3D", 3 },
    { "EXPORT_LAST", 0 }
};

enum ExportType {
    EXPORT_NONE             = 0,
    EXPORT_VERTEX           = 1,
    EXPORT_VERTEX_INDEXED   = 2,
    EXPORT_TEXTURE          = 3,
    EXPORT_TEXTURE_INDEXED  = 4,
    EXPORT_COLOR            = 5,
    EXPORT_COLOR_INDEXED    = 6,
    EXPORT_VERTEX_3D        = 7,
    EXPORT_VERTEX_INDEXED_3D= 8,
    EXPORT_NORMAL_3D        = 9,
    EXPORT_NORMAL_INDEXED_3D= 10,
    EXPORT_LAST             = 11
};

struct ExportMacro {
    ExportType type;
    size_t size;

    ExportMacro(ExportType type);
};

struct ExportFile {
    std::vector<float> data;
    std::vector<ExportMacro> exp;

    size_t strideSize() const noexcept;

    ExportFile() = default;
    ~ExportFile() = default;
    ExportFile(const ExportFile&) = delete;
    ExportFile(ExportFile&&) = default;
        
    std::string info() const;
    std::string detail(int max=1000, char seperator='\t', bool separateSegments=true) const;

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
    std::vector<vec2> vertices;
    std::vector<vec2> texCoords;
    std::vector<vec3> colors;

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

    MeshBuilder2D& invertWinding(bool indexed) noexcept;

    MeshBuilder2D& addMesh(const MeshBuilder2D &mesh) noexcept;
    MeshBuilder2D& addCircle(size_t pcount, float radius, bool strip=false);
    MeshBuilder2D& addRect(float x, float y, float width, float height);
    MeshBuilder2D& addRect(vec2 center, vec2 offset);
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

    const std::vector<vec2>& getVertices() const;
    const std::vector<vec2>& getTextureCoords() const;
    const std::vector<vec3>& getColors() const;

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
        

    template<typename Iterator>
    MeshBuilder& addVertices(Iterator begin, Iterator end) {
        vertices.reserve(vertices.size() + std::distance(begin, end));
        vertices.insert(vertices.end(), begin, end);
        return *this;
    }
    template<typename Iterator>
    MeshBuilder& addTextureCoords(Iterator begin, Iterator end) {
        texcoords.reserve(texcoords.size() + std::distance(begin, end));
        texcoords.insert(texcoords.end(), begin, end);
        return *this;
    }
    template<typename Iterator>
    MeshBuilder& addColors(Iterator begin, Iterator end) {
        colors.reserve(colors.size() + std::distance(begin, end));
        colors.insert(colors.end(), begin, end);
        return *this;
    }
    template<typename Iterator>
    MeshBuilder& addNormals(Iterator begin, Iterator end) {
        normals.reserve(normals.size() + std::distance(begin, end));
        normals.insert(normals.end(), begin, end);
        return *this;
    }

    template<typename Iterator>
    MeshBuilder& addIndices(std::vector<int> &indices, Iterator begin, Iterator end) {
        size_t initialSize = indices.size();
        indices.reserve(indices.size() + std::distance(begin, end));
        for (; begin != end; ++begin)
            indices.push_back(*begin + initialSize);
        return *this;
    }

    MeshBuilder& invertWinding(bool indexed) noexcept;

    template<typename Iterator>
    MeshBuilder& addVerticeIndices(Iterator begin, Iterator end) {
        return addIndices<Iterator>(v_indices, begin, end);
    }
    template<typename Iterator>
    MeshBuilder& addNormalIndices(Iterator begin, Iterator end) {
        return addIndices<Iterator>(vn_indices, begin, end);
    }
    template<typename Iterator>
    MeshBuilder& addTextureIndices(Iterator begin, Iterator end) {
        return addIndices<Iterator>(vt_indices, begin, end);
    }
    template<typename Iterator>
    MeshBuilder& addColorIndices(Iterator begin, Iterator end) {
        return addIndices<Iterator>(vc_indices, begin, end);
    }


    MeshBuilder& add(const MeshBuilder2D &mesh, float height = 0.0f, bool up=true,
        bool srcIndex=false, bool dstIndex=false) noexcept;
    MeshBuilder& add(const MeshBuilder &mesh) noexcept;

    std::string info() const noexcept;

    void scale(float scale);
    void unitize(float unitScale=1.0f);

    void setVertices(const std::vector<glm::vec3> &vertices);
    void setNormals(const std::vector<glm::vec3> &normals);
    void setTexCoords(const std::vector<glm::vec2> &texCoords);
    void setColors(const std::vector<glm::vec3> &colors);

    MeshBuilder& addCube(vec3 center, float dimension) noexcept;
    MeshBuilder& addCube(vec3 center, vec3 dimensions) noexcept;
    MeshBuilder& addPlane(vec3 p1, vec3 p2, vec3 p3, vec3 p4) noexcept;

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
