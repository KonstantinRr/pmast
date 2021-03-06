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

#include <engine/entity.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <limits>

NYREM_USE_NAMESPACE

Entity::Entity() noexcept : IDObject() { }
Entity::Entity(uint32_t id) noexcept : IDObject(id) { }

IDObject::IDObject() noexcept : id(std::numeric_limits<uint32_t>::max()) { }
IDObject::IDObject(uint32_t id) noexcept : id(id) { }

bool IDObject::hasID() const noexcept { return id != std::numeric_limits<uint32_t>::max(); }

void IDObject::setID(uint32_t id) noexcept { this->id = id; }
uint32_t IDObject::getID() const noexcept { return id; }

// ---- Entity ---- //

bool Entity::hasModel() const noexcept { return k_model.get() != nullptr; }
bool Entity::hasTexture() const noexcept { return k_texture.get() != nullptr; }
bool Entity::hasNormalTexture() const noexcept { return k_normal.get() != nullptr; }
bool Entity::hasMaterial() const noexcept { return k_material.get() != nullptr; }

void Entity::setModel(const std::shared_ptr<GLModel> &model) { k_model = model; }
void Entity::setTexture(const std::shared_ptr<GLTexture2D> &tex) { k_texture = tex; }
void Entity::setNormalTexture(const std::shared_ptr<GLTexture2D> &tex) { k_normal = tex; }
void Entity::setMaterial(const std::shared_ptr<GLMaterial> &material) { k_material = material; }

const std::shared_ptr<GLModel>& Entity::getModel() const { return k_model; }
const std::shared_ptr<GLTexture2D>& Entity::getTexture() const { return k_texture; }
const std::shared_ptr<GLTexture2D>& Entity::getNormalTexture() const { return k_normal; }
const std::shared_ptr<GLMaterial>& Entity::getMaterial() const { return k_material; }

Entity::ColorStorage& Entity::getColorStorage() noexcept { return m_colors; }
const Entity::ColorStorage& Entity::getColorStorage() const noexcept { return m_colors; }

//// ---- TransformableEntity ---- ////

TransformableEntity::TransformableEntity(int id,
    const Translation3DSettings &translation,
    const RotationSettings3DEuler &rotation,
    const ScaleSettings3D &scale) noexcept :
    Entity(id), m_translation(translation),
    m_rotation(rotation), m_scale(scale) { }


mat4x4 TransformableEntity::matrix() const noexcept {
    mat4x4 mat(1.0f);
    passthroughType(mat);
    return mat;
}
mat4x4 TransformableEntity::inverse() const noexcept {
    mat4x4 mat(1.0f);
    passthroughInverseType(mat);
    return mat;
}
void TransformableEntity::passthrough(mat4x4 &mat) const noexcept {
    passthroughType(mat);
}
void TransformableEntity::passthrough(vec4 &vec) const noexcept {
    passthroughType(vec);
}
void TransformableEntity::passthroughInverse(mat4x4 &mat) const noexcept {
    passthroughInverseType(mat);
}
void TransformableEntity::passthroughInverse(vec4 &vec) const noexcept {
    passthroughInverseType(vec);
}

//// ---- TransformedEntity ---- ////
TransformedEntity::TransformedEntity(int id,
    const glm::mat4 &transform, const glm::mat3 &normal)
    : Entity(id), k_mat_transform(transform),
    k_mat_normal(normal) { }

glm::mat4x4 TransformedEntity::getTransformationMatrix() const { return k_mat_transform; }
glm::mat3x3 TransformedEntity::getNormalMatrix() const { return k_mat_normal; }

//// ---- Entity2D ---- ////

Entity2D::Entity2D() : IDObject() { }

Entity2D::Entity2D(int id,
    const std::shared_ptr<GLModel>& model,
    const std::shared_ptr<GLTexture2D>& texture,
    const ColorStorage &colors)
    : IDObject(id), model(model), texture(texture), m_colors(colors) { }


const std::shared_ptr<GLTexture2D> Entity2D::getTexture() const { return texture; }
const std::shared_ptr<GLModel> Entity2D::getModel() const { return model; }

void Entity2D::setTexture(const std::shared_ptr<GLTexture2D> texture) { this->texture = texture; }
void Entity2D::setModel(const std::shared_ptr<GLModel> model) { this->model = model; }

Entity::ColorStorage& Entity2D::getColorStorage() noexcept { return m_colors; }
const Entity::ColorStorage& Entity2D::getColorStorage() const noexcept { return m_colors; }

// ---- TransformedEntity2D ---- //
TransformedEntity2D::TransformedEntity2D()
    : transform(1.0f) { }

TransformedEntity2D::TransformedEntity2D(int id,
    const std::shared_ptr<GLModel>& model,
    const std::shared_ptr<GLTexture2D>& texture,
    const glm::mat4x4 &transform)
    : Entity2D(id, model, texture), transform(transform) { }

void TransformedEntity2D::setTransformationMatrix(const glm::mat4x4& mat) { transform = mat; }
glm::mat4x4 TransformedEntity2D::getTransformationMatrix() const { return transform; }

// ---- TransformableEntity2D ---- //
TransformableEntity2D::TransformableEntity2D()
    : entityPosition({0.0f, 0.0f}),
    entityScale({0.0f, 0.0f}), entityRotation(1.0f) {}
TransformableEntity2D::TransformableEntity2D(int id,
    const std::shared_ptr<GLModel> &model,
    const std::shared_ptr<GLTexture2D> &texture,
    const ColorStorage &colorStorage,
    const glm::vec2 &position, const glm::vec2 &scale,
    float rotation
) : Entity2D(id, model, texture, colorStorage),
    entityPosition(position),
    entityScale(scale), entityRotation(rotation) { }

TransformableEntity2D& TransformableEntity2D::setPosition(const glm::vec2 &position) { entityPosition = position; return *this; }
TransformableEntity2D& TransformableEntity2D::setScale(const glm::vec2 &scale) { entityScale = scale; return *this; }
TransformableEntity2D& TransformableEntity2D::setRotation(float rotation) { entityRotation = rotation; return *this; }

TransformableEntity2D& TransformableEntity2D::move(const glm::vec2 &argument) { entityPosition += argument; return *this; }
TransformableEntity2D& TransformableEntity2D::scale(const glm::vec2 &scale) { entityScale *= scale; return *this; }
TransformableEntity2D& TransformableEntity2D::scale(float scale) { entityScale *= scale; return *this; }
TransformableEntity2D& TransformableEntity2D::rotate(float rotation) { entityRotation += rotation; return *this; }

const glm::vec2& TransformableEntity2D::getPosition() const { return entityPosition; }
const glm::vec2& TransformableEntity2D::getScale() const { return entityScale; }
float TransformableEntity2D::getRotation() const { return entityRotation; }

glm::mat3x3 TransformableEntity2D::calculateTransformationMatrix3D() const
{
    glm::mat3x3 transform(1.0f);
    transform = glm::translate(transform, entityPosition);
    transform = glm::rotate(transform, entityRotation);
    transform = glm::scale(transform, entityScale);
    return transform;
}

glm::mat4x4 TransformableEntity2D::calculateTransformationMatrix() const
{
    glm::mat4x4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(entityPosition, 0.0f));
    transform = glm::rotate(transform, entityRotation, { 0.0F, 0.0F, 1.0F });
    transform = glm::scale(transform, glm::vec3(entityScale, 1.0f));
    return transform;
}

glm::mat4x4 TransformableEntity2D::getTransformationMatrix() const {
    return calculateTransformationMatrix();
}

// ---- MatrixBufferedEntity2D ---- //
MatrixBufferedEntity2D::MatrixBufferedEntity2D(int id,
    const std::shared_ptr<GLModel> &model,
    const std::shared_ptr<GLTexture2D> &texture,
    const ColorStorage& colorStorage,
    const glm::vec2 &position,
    const glm::vec2 &scale,
    float rotation) : TransformableEntity2D(
        id, model, texture, colorStorage, position, scale, rotation) { }

TransformableEntity2D& MatrixBufferedEntity2D::setPosition(const glm::vec2 &position) { dirty(); return TransformableEntity2D::setPosition(position); }
TransformableEntity2D& MatrixBufferedEntity2D::setScale(const glm::vec2 &scale) { dirty(); return TransformableEntity2D::setScale(scale); }
TransformableEntity2D& MatrixBufferedEntity2D::setRotation(float rotation) { dirty(); return TransformableEntity2D::setRotation(rotation); }

TransformableEntity2D& MatrixBufferedEntity2D::move(const glm::vec2 &argument) { dirty(); return TransformableEntity2D::move(argument); }
TransformableEntity2D& MatrixBufferedEntity2D::scale(const glm::vec2 &scale) { dirty(); return TransformableEntity2D::scale(scale); }
TransformableEntity2D& MatrixBufferedEntity2D::scale(float scale) { dirty(); return TransformableEntity2D::scale(scale); }
TransformableEntity2D& MatrixBufferedEntity2D::rotate(float rotation) { dirty(); return TransformableEntity2D::rotate(rotation); };

bool MatrixBufferedEntity2D::isDirty() const { return k_buffer->hasTransformChange; }
void MatrixBufferedEntity2D::rebuild() {
    if (isDirty()) {
        k_buffer->transform = calculateTransformationMatrix();
        dirty(false);
    }
}
void MatrixBufferedEntity2D::dirty(bool value) {
    k_buffer->hasTransformChange = value;
}
glm::mat4x4 MatrixBufferedEntity2D::getTransformationMatrix() const {
    return k_buffer->transform;
}
// ---- TickableLambdaEntity ---- //

/*
void TickableLambdaEntity::update(float t, float dt) {
    updateFunction(t, dt, *this);
}
*/
