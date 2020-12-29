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

#include "internal.hpp"
#include "glmodel.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <functional>

namespace nyrem {

class Entity {
public:
    Entity();
    Entity(int id);

    virtual ~Entity() = default;

    virtual Entity& setID(int id);
    int getID() const;

    bool hasModel() const;
    bool hasTexture() const;
    bool hasNormalTexture() const;
    bool hasMaterial() const;

    void setModel(const std::shared_ptr<GLModel> &model);
    void setTexture(const std::shared_ptr<GLTexture2D> &tex);
    void setNormalTexture(const std::shared_ptr<GLTexture2D> &tex);
    void setMaterial(const std::shared_ptr<GLMaterial> &material);

    const std::shared_ptr<GLModel>& getModel() const;
    const std::shared_ptr<GLTexture2D>& getTexture() const;
    const std::shared_ptr<GLTexture2D>& getNormalTexture() const;
    const std::shared_ptr<GLMaterial>& getMaterial() const;

    virtual glm::mat4x4 getTransformationMatrix() const = 0;
    virtual glm::mat3x3 getNormalMatrix() const = 0;

protected:
    int id;
    std::shared_ptr<GLModel> k_model;
    std::shared_ptr<GLTexture2D> k_texture;
    std::shared_ptr<GLTexture2D> k_normal;
    std::shared_ptr<GLMaterial> k_material;
};

//// ---- MatrixBufferedEntity ---- ////

class TransformableEntity : public Entity {
public:
    TransformableEntity() = default;
    TransformableEntity(int id,
        const glm::vec3 &pos = {0.0f, 0.0f, 0.0f},
        const glm::vec3 &rot = {0.0f, 0.0f, 0.0f},
        const glm::vec3 &scale = {1.0f, 1.0f, 1.0f});
    virtual ~TransformableEntity() = default;

    virtual Entity& move(const glm::vec3 &operation);

    virtual Entity& scale(const glm::vec3 &scale);
    virtual Entity& scale(float scale);

    virtual Entity& rotate(const glm::vec3 &rotation);
    virtual Entity& rotateX(float angle);
    virtual Entity& rotateY(float angle);
    virtual Entity& rotateZ(float angle);

    virtual Entity& setPosition(const glm::vec3 &position);
    virtual Entity& setRotation(const glm::vec3 &rotation);
    virtual Entity& setScale(const glm::vec3 &scale);
    virtual Entity& setScale(float scale);

    const glm::vec3& getPosition() const;
    const glm::vec3& getRotation() const;
    const glm::vec3& getScale() const;

    glm::mat4x4 calculateTransformationMatrix() const;
    glm::mat3x3 calculateNormalMatrix() const;

    virtual glm::mat4x4 getTransformationMatrix() const;
    virtual glm::mat3x3 getNormalMatrix() const;

protected:
    glm::vec3 entityPosition, entityRotation, entityScale;
};

class MatrixBufferedEntity : public TransformableEntity {
public:
    MatrixBufferedEntity() = default;
    MatrixBufferedEntity(int id,
        const glm::vec3 &position = {0.0f, 0.0f, 0.0f},
        const glm::vec3 &rotation = {0.0f, 0.0f, 0.0f},
        const glm::vec3 &scale = {1.0f, 1.0f, 1.0f});
    virtual ~MatrixBufferedEntity() = default;

    virtual Entity& move(const glm::vec3 &operation);

    virtual Entity& scale(const glm::vec3 &scale);
    virtual Entity& scale(float scale);

    virtual Entity& rotate(const glm::vec3 &rotation);
    virtual Entity& rotateX(float angle);
    virtual Entity& rotateY(float angle);
    virtual Entity& rotateZ(float angle);

    virtual Entity& setPosition(const glm::vec3 &position);
    virtual Entity& setRotation(const glm::vec3 &rotation);
    virtual Entity& setScale(const glm::vec3 &scale);
    virtual Entity& setScale(float scale);

    void rebuild() const;
    bool isDirty() const;
    void dirty(bool value=true) const;

    virtual glm::mat4x4 getTransformationMatrix() const;
    virtual glm::mat3x3 getNormalMatrix() const;

protected:
    struct BufferObject {
        glm::mat4x4 transformationMatrix = glm::mat4x4(1.0f);
        glm::mat3x3 normalMatrix = glm::mat3x3(1.0f);
        bool hasTransformChange = true;
    };
    std::unique_ptr<BufferObject> k_buffer
        = std::make_unique<BufferObject>();
};

class TransformedEntity : public Entity {
public:
    TransformedEntity() = default;
    TransformedEntity(int id,
        const glm::mat4 &transform = glm::mat4(1.0f),
        const glm::mat3 &normal = glm::mat3(1.0f));
    virtual ~TransformedEntity() = default;

    virtual glm::mat4x4 getTransformationMatrix() const;
    virtual glm::mat3x3 getNormalMatrix() const;

protected:
    glm::mat4 k_mat_transform;
    glm::mat3 k_mat_normal;
};

/////////////////////////////////////////////
//////////// ---- ENTITY 2D ---- ////////////
/////////////////////////////////////////////

//// ---- TransformableEntity2D ---- ////

class Entity2D {
public:
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
        const std::shared_ptr<GLTexture2D>& texture = nullptr
    );

    int getID() const;
    void setID(int id);

    const std::shared_ptr<GLTexture2D> getTexture() const;
    const std::shared_ptr<GLModel> getModel() const;

    void setTexture(const std::shared_ptr<GLTexture2D> texture);
    void setModel(const std::shared_ptr<GLModel> model);

    virtual glm::mat4x4 getTransformationMatrix() const = 0;

protected:
    int id;
    std::shared_ptr<GLModel> model;
    std::shared_ptr<GLTexture2D> texture;
};

class TransformedEntity2D : public Entity2D {
public:
    TransformedEntity2D();
    TransformedEntity2D(int id,
        const std::shared_ptr<GLModel>& model = nullptr,
        const std::shared_ptr<GLTexture2D>& texture = nullptr,
        const glm::mat4x4 &transformation = glm::mat4(1.0f));

    void setTransformationMatrix(const glm::mat4x4 &mat);

    virtual glm::mat4x4 getTransformationMatrix() const;

protected:
    glm::mat4x4 transform;
};

class TransformableEntity2D : public Entity2D {
public:
    TransformableEntity2D();
    TransformableEntity2D(int id,
        const std::shared_ptr<GLModel> &model = nullptr,
        const std::shared_ptr<GLTexture2D> &texture = nullptr,
        const glm::vec2 &position=glm::vec2(0.0f, 0.0f),
        const glm::vec2 &scale=glm::vec2(1.0f, 1.0f),
        float rotation=0.0f);
    virtual ~TransformableEntity2D() = default;

    virtual TransformableEntity2D& setPosition(const glm::vec2 &position);
    virtual TransformableEntity2D& setScale(const glm::vec2 &scale);
    virtual TransformableEntity2D& setRotation(float rotation);

    virtual TransformableEntity2D& move(const glm::vec2 &argument);
    virtual TransformableEntity2D& scale(const glm::vec2 &scale);
    virtual TransformableEntity2D& scale(float scale);
    virtual TransformableEntity2D& rotate(float rotation);

    const glm::vec2& getPosition() const;
    const glm::vec2& getScale() const;
    float getRotation() const;

    glm::mat3x3 calculateTransformationMatrix3D() const;
    glm::mat4x4 calculateTransformationMatrix() const;

    virtual glm::mat4x4 getTransformationMatrix() const;

protected:
    glm::vec2 entityPosition;
    glm::vec2 entityScale;
    float entityRotation;
};

//// ---- MatrixBufferedEntity2D ---- ////
class MatrixBufferedEntity2D : public TransformableEntity2D {
public:
    MatrixBufferedEntity2D() = default;
    MatrixBufferedEntity2D(int id,
        const std::shared_ptr<GLModel> &model = nullptr,
        const std::shared_ptr<GLTexture2D> &texture = nullptr,
        const glm::vec2 &position = glm::vec2(0.0f, 0.0f),
        const glm::vec2 &scale = glm::vec2(1.0f, 1.0f),
        float rotation = 0.0f);

    virtual TransformableEntity2D& setPosition(const glm::vec2 &position);
    virtual TransformableEntity2D& setScale(const glm::vec2 &scale);
    virtual TransformableEntity2D& setRotation(float rotation);

    virtual TransformableEntity2D& move(const glm::vec2 &argument);
    virtual TransformableEntity2D& scale(const glm::vec2 &scale);
    virtual TransformableEntity2D& scale(float scale);
    virtual TransformableEntity2D& rotate(float rotation);

    bool isDirty() const;
    void rebuild();
    void dirty(bool value=true);

    virtual glm::mat4x4 getTransformationMatrix() const;
protected:
    struct MatrixBuffer2D {
        bool hasTransformChange = true;
        glm::mat4x4 transform = glm::mat4x4(1.0f);
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
            const glm::vec3 &position = glm::vec3(0.0, 0.0, 0.0),
            const glm::vec3 &rotation = glm::vec3(0.0, 0.0, 0.0),
            const glm::vec3 &scale = glm::vec3(1.0, 1.0, 1.0))
        : Entity(id, model, position, rotation, scale) {
        // wraps the lambda call in a std::function
        this->updateFunction = std::function<
                void(float, float, Entity&)>(updateFunction);
    }

    virtual void update(float t, float dt);
};
*/

} // !nyrem
#endif // NYREM_ENTITY_H
