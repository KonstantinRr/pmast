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

#ifndef NYREM_ENTITY_H
#define NYREM_ENTITY_H

#include <engine/internal.hpp>
#include <engine/glmodel.hpp>
#include <engine/camera.hpp>

#include <glm/glm.hpp>

#include <memory>
#include <array>
#include <functional>

NYREM_NAMESPACE_BEGIN

class TextureStorage {

};

template<size_t n>
class ColorStorageGeneric {
public:
    ColorStorageGeneric() : m_count(0), m_colors{nyrem::vec3{0.0f}} { }
    explicit ColorStorageGeneric(nyrem::vec3 color) : m_count(1) {
        m_colors[0] = color;
    }
    ColorStorageGeneric(std::initializer_list<nyrem::vec3> v) :
        m_count(v.size()) {
        auto vEnd = v.end();
        auto out = m_colors.begin();
        for (auto it = v.begin(); it != vEnd; ++it, ++out) {
            *out = *it;
        }
    }

    bool hasColor() const noexcept { return m_count != 0; }
    size_t colorCount() const noexcept { return m_count; }
    size_t maxCount() const noexcept { return n; }
    
    size_t addColor(const nyrem::vec3 &color) noexcept {
        m_colors[m_count] = color;
        return m_count++;
    }

    nyrem::vec3& operator[](size_t idx) noexcept { return m_colors[idx]; }
    const nyrem::vec3& operator[](size_t idx) const noexcept { return m_colors[idx]; }

protected:
    size_t m_count;
    std::array<nyrem::vec3, n> m_colors;
};

/// <summary>
/// Entities are the base objects in the render and physics engine.
/// Each entity owns a specific set of attributes that are used by the
/// render engine (the specific usage depends on the rendering system).
/// Each default entity owns
///   - 4 texture slots
///   - 4 model slots
///   - 4 material slots
/// </summary>
class Entity : public IDObject, public ViewTransformer {
public:
    using ColorStorage = ColorStorageGeneric<4>;

    Entity() noexcept;
    Entity(uint32_t id) noexcept;

    /// <summary>
    /// Allows classes to be deleted by their base class.
    /// </summary>
    virtual ~Entity() = default;

    /// <summary>Checks whether the entity has a model associated</summary>
    /// <returns>True iff the entity has a model</returns>
    bool hasModel() const noexcept;
    /// <summary>Checks whether the entity has a default texture</summary>
    /// <returns>True iff the entity has a default texture</returns>
    bool hasTexture() const noexcept;
    /// <summary>Checks whether the entity has a normal texture</summary>
    /// <returns>True iff the entity has a normal texture</returns>
    bool hasNormalTexture() const noexcept;
    /// <summary>Checks whether the entity has a material</summary>
    /// <returns>True iff the entity has a material</returns>
    bool hasMaterial() const noexcept;

    void setModel(const std::shared_ptr<GLModel> &model);
    void setTexture(const std::shared_ptr<GLTexture2D> &tex);
    void setNormalTexture(const std::shared_ptr<GLTexture2D> &tex);
    void setMaterial(const std::shared_ptr<GLMaterial> &material);

    const std::shared_ptr<GLModel>& getModel() const;
    const std::shared_ptr<GLTexture2D>& getTexture() const;
    const std::shared_ptr<GLTexture2D>& getNormalTexture() const;
    const std::shared_ptr<GLMaterial>& getMaterial() const;

    ColorStorage& getColorStorage() noexcept;
    const ColorStorage& getColorStorage() const noexcept;
protected:
    std::shared_ptr<GLModel> k_model;
    std::shared_ptr<GLTexture2D> k_texture;
    std::shared_ptr<GLTexture2D> k_normal;
    std::shared_ptr<GLMaterial> k_material;

    ColorStorage m_colors;
};

//// ---- TransformableEntity ---- ////

/// <summary>
/// TransformableEntities are a subclass of entities that can be moved/scaled
/// and rotated to achieve the entity transformation functions. The transformation
/// matrix is recalculated every time transformationMatrix() or normalMatrix() is
/// called. See MatrixBufferedEntity for a solution that buffers the matrix between calls.
/// </summary>
class TransformableEntity : public Entity,
    protected MatrixBuffer<CalculateMatrix, CalculateMatrix> {
public:
    using ThisType = TransformableEntity;
    using TransformableEntityType = ThisType;
    using TypeMatrixBuffer = MatrixBuffer<CalculateMatrix, CalculateMatrix>;

    using Scale3DType = Scale3D<>;
    using Translation3DType = Translation3D<>;
    using Rotation3DType = Rotation3D<>;

protected:
    Scale3DType m_scale;
    Translation3DType m_translation;
    Rotation3DType m_rotation;

    template<typename Type>
    void passthroughType(Type& tp) const noexcept {
        m_translation.passthrough(tp);
        m_rotation.passthrough(tp);
        m_scale.passthrough(tp);
    }

    template<typename Type>
    void passthroughInverseType(Type &tp) const noexcept {
        m_scale.passthroughInverse(tp);
        m_rotation.passthroughInverse(tp);
        m_translation.passthroughInverse(tp);
    }

public:
    TransformableEntity() = default;
    TransformableEntity(int id,
        const Translation3DSettings &translation,
        const RotationSettings3DEuler &rotation,
        const ScaleSettings3D &scale) noexcept;
    virtual ~TransformableEntity() = default;

    Scale3DType& scale() noexcept { return m_scale; }
    Translation3DType& translation() noexcept { return m_translation; }
    Rotation3DType& rotation() noexcept { return m_rotation; }
    
    const Scale3DType& scale() const noexcept { return m_scale; }
    const Translation3DType& translation() const noexcept { return m_translation; }
    const Rotation3DType& rotation() const noexcept { return m_rotation; }
    
    virtual mat4x4 matrix() const noexcept override;
	virtual mat4x4 inverse() const noexcept override;
	virtual void passthrough(mat4x4 &mat) const noexcept override;
	virtual void passthrough(vec4 &vec) const noexcept override;
	virtual void passthroughInverse(mat4x4 &mat) const noexcept override;
	virtual void passthroughInverse(vec4 &vec) const noexcept override;

protected:
    vec3 entityPosition, entityRotation, entityScale;
};

class TransformedEntity : public Entity {
public:
    TransformedEntity() = default;
    TransformedEntity(int id,
        const mat4x4 &transform = mat4x4(1.0f),
        const mat3x3 &normal = mat3x3(1.0f));
    virtual ~TransformedEntity() = default;

    virtual mat4x4 getTransformationMatrix() const;
    virtual mat3x3 getNormalMatrix() const;

protected:
    mat4x4 k_mat_transform;
    mat3x3 k_mat_normal;
};

/////////////////////////////////////////////
//////////// ---- ENTITY 2D ---- ////////////
/////////////////////////////////////////////

//// ---- TransformableEntity2D ---- ////

class Entity2D : public IDObject {
public:
    using ColorStorage = ColorStorageGeneric<4>;

    /// <summary>
    /// Creates an Entity2D with an invalid ID and without any model or texture.
    /// This entity will be skipped in rendering.
    /// </summary>
    Entity2D();

    /// <summary>
    /// Creates an Entity2D with an ID as well as an optional model and/or texture.
    /// </summary>
    /// <param name="id">The entitie's unqiue ID</param>
    /// <param name="model">The entitie's model</param>
    /// <param name="texture">The entitie's texture</param>
    Entity2D(int id,
        const std::shared_ptr<GLModel>& model = nullptr,
        const std::shared_ptr<GLTexture2D>& texture = nullptr,
        const ColorStorage &colors = { }
    );

    const std::shared_ptr<GLTexture2D> getTexture() const;
    const std::shared_ptr<GLModel> getModel() const;

    void setTexture(const std::shared_ptr<GLTexture2D> texture);
    void setModel(const std::shared_ptr<GLModel> model);

    virtual mat4x4 getTransformationMatrix() const = 0;

    ColorStorage& getColorStorage() noexcept;
    const ColorStorage& getColorStorage() const noexcept;
protected:
    std::shared_ptr<GLModel> model;
    std::shared_ptr<GLTexture2D> texture;

    ColorStorage m_colors;
};

class TransformedEntity2D : public Entity2D {
public:
    TransformedEntity2D();
    TransformedEntity2D(int id,
        const std::shared_ptr<GLModel>& model = nullptr,
        const std::shared_ptr<GLTexture2D>& texture = nullptr,
        const mat4x4 &transformation = mat4x4(1.0f));

    void setTransformationMatrix(const mat4x4 &mat);

    virtual mat4x4 getTransformationMatrix() const;

protected:
    mat4x4 transform;
};

class TransformableEntity2D : public Entity2D {
public:
    TransformableEntity2D();
    TransformableEntity2D(int id,
        const std::shared_ptr<GLModel> &model = nullptr,
        const std::shared_ptr<GLTexture2D> &texture = nullptr,
        const ColorStorage &colorStorage = { },
        const vec2 &position=vec2(0.0f, 0.0f),
        const vec2 &scale=vec2(1.0f, 1.0f),
        float rotation=0.0f);
    virtual ~TransformableEntity2D() = default;

    virtual TransformableEntity2D& setPosition(const vec2 &position);
    virtual TransformableEntity2D& setScale(const vec2 &scale);
    virtual TransformableEntity2D& setRotation(float rotation);

    virtual TransformableEntity2D& move(const vec2 &argument);
    virtual TransformableEntity2D& scale(const vec2 &scale);
    virtual TransformableEntity2D& scale(float scale);
    virtual TransformableEntity2D& rotate(float rotation);

    const vec2& getPosition() const;
    const vec2& getScale() const;
    float getRotation() const;

    mat3x3 calculateTransformationMatrix3D() const;
    mat4x4 calculateTransformationMatrix() const;

    virtual mat4x4 getTransformationMatrix() const;

protected:
    vec2 entityPosition;
    vec2 entityScale;
    float entityRotation;
};

//// ---- MatrixBufferedEntity2D ---- ////
class MatrixBufferedEntity2D : public TransformableEntity2D {
public:
    MatrixBufferedEntity2D() = default;
    MatrixBufferedEntity2D(int id,
        const std::shared_ptr<GLModel> &model = nullptr,
        const std::shared_ptr<GLTexture2D> &texture = nullptr,
        const ColorStorage& colorStorage = { },
        const vec2 &position = vec2(0.0f, 0.0f),
        const vec2 &scale = vec2(1.0f, 1.0f),
        float rotation = 0.0f);

    virtual TransformableEntity2D& setPosition(const vec2 &position);
    virtual TransformableEntity2D& setScale(const vec2 &scale);
    virtual TransformableEntity2D& setRotation(float rotation);

    virtual TransformableEntity2D& move(const vec2 &argument);
    virtual TransformableEntity2D& scale(const vec2 &scale);
    virtual TransformableEntity2D& scale(float scale);
    virtual TransformableEntity2D& rotate(float rotation);

    bool isDirty() const;
    void rebuild();
    void dirty(bool value=true);

    virtual mat4x4 getTransformationMatrix() const;
protected:
    struct MatrixBuffer2D {
        bool hasTransformChange = true;
        mat4x4 transform = mat4x4(1.0f);
    };
    std::unique_ptr<MatrixBuffer2D> k_buffer
        = std::make_unique<MatrixBuffer2D>();
};

class Tickable {
public:
    virtual ~Tickable() = default;
    virtual void update(float t, float dt) = 0;
};

/*
class TickableLambdaEntity : public Entity, public Tickable {
protected:
    std::function<void(float, float, Entity&)> updateFunction;
public:
    TickableLambdaEntity() = default;

    template<typename ftype>
    TickableLambdaEntity(
            int id,
            const std::shared_ptr<GLTexturedModel> &model,
            const ftype& updateFunction,
            const vec3 &position = vec3(0.0, 0.0, 0.0),
            const vec3 &rotation = vec3(0.0, 0.0, 0.0),
            const vec3 &scale = vec3(1.0, 1.0, 1.0))
        : Entity(id, model, position, rotation, scale) {
        // wraps the lambda call in a std::function
        this->updateFunction = std::function<
                void(float, float, Entity&)>(updateFunction);
    }

    virtual void update(float t, float dt);
};
*/
NYREM_NAMESPACE_END

#endif // NYREM_ENTITY_H
