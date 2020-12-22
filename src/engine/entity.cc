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

// ---- Entity ---- //
using namespace nyrem;

Entity::Entity() : id(-1) { }
Entity::Entity(int id) : id(id) { }

Entity& Entity::setID(int pID) {
    this->id = pID;
    return *this;
}

int Entity::getID() const { return id; }

bool Entity::hasModel() const { return k_model.get() != nullptr; }
bool Entity::hasTexture() const { return k_texture.get() != nullptr; }
bool Entity::hasNormalTexture() const { return k_normal.get() != nullptr; }
bool Entity::hasMaterial() const { return k_material.get() != nullptr; }

void Entity::setModel(const std::shared_ptr<GLModel> &model) { k_model = model; }
void Entity::setTexture(const std::shared_ptr<GLTexture2D> &tex) { k_texture = tex; }
void Entity::setNormalTexture(const std::shared_ptr<GLTexture2D> &tex) { k_normal = tex; }
void Entity::setMaterial(const std::shared_ptr<GLMaterial> &material) { k_material = material; }

const std::shared_ptr<GLModel>& Entity::getModel() const { return k_model; }
const std::shared_ptr<GLTexture2D>& Entity::getTexture() const { return k_texture; }
const std::shared_ptr<GLTexture2D>& Entity::getNormalTexture() const { return k_normal; }
const std::shared_ptr<GLMaterial>& Entity::getMaterial() const { return k_material; }


//// ---- TransformableEntity ---- ////

TransformableEntity::TransformableEntity(int id,
    const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scale)
    : Entity(id), entityPosition(pos),
    entityRotation(rot), entityScale(scale) { }

Entity& TransformableEntity::move(const glm::vec3 &operation) {
    entityPosition += operation;
    return *this;
}
Entity& TransformableEntity::scale(const glm::vec3 &operation) {
    entityScale *= operation;
    return *this;
}
Entity& TransformableEntity::scale(float scale) {
    entityScale *= scale;
    return *this;
}
Entity& TransformableEntity::rotate(const glm::vec3 &operation) {
    entityRotation += operation;
    return *this;
}

Entity& TransformableEntity::rotateX(float angle) { entityRotation[0] += angle; return *this; }
Entity& TransformableEntity::rotateY(float angle) { entityRotation[1] += angle; return *this; }
Entity& TransformableEntity::rotateZ(float angle) { entityRotation[2] += angle; return *this; }

Entity& TransformableEntity::setPosition(const glm::vec3 &position) {
    entityPosition = position;
    return *this;
}
Entity& TransformableEntity::setRotation(const glm::vec3 &rotation) {
    entityRotation = rotation;
    return *this;
}
Entity& TransformableEntity::setScale(const glm::vec3 &scale) {
    entityScale = scale;
    return *this;
}
Entity& TransformableEntity::setScale(float scale) {
    entityScale = {scale, scale, scale};
    return *this;
}

const glm::vec3& TransformableEntity::getPosition() const { return entityPosition; }
const glm::vec3& TransformableEntity::getRotation() const { return entityRotation; }
const glm::vec3& TransformableEntity::getScale() const { return entityScale; }

glm::mat4x4 TransformableEntity::calculateTransformationMatrix() const {
    glm::mat4x4 meshTransform(1.0f);
    meshTransform = glm::translate(meshTransform, entityPosition);

    meshTransform = glm::rotate(meshTransform, entityRotation.x, {1.0F, 0.0F, 0.0F});
    meshTransform = glm::rotate(meshTransform, entityRotation.y, {0.0F, 1.0F, 0.0F});
    meshTransform = glm::rotate(meshTransform, entityRotation.z, {0.0F, 0.0F, 1.0F});

    meshTransform = glm::scale(meshTransform, entityScale);
    return meshTransform;
}

glm::mat3x3 TransformableEntity::calculateNormalMatrix() const {
    glm::mat3x3 mat(calculateTransformationMatrix());
    return glm::inverseTranspose(mat);
}

glm::mat4x4 TransformableEntity::getTransformationMatrix() const { return calculateTransformationMatrix(); }
glm::mat3x3 TransformableEntity::getNormalMatrix() const { return calculateNormalMatrix(); }

//// ---- MatrixBufferedEntity ---- ////
MatrixBufferedEntity::MatrixBufferedEntity(int id,
    const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale)
    : TransformableEntity(id, position, rotation, scale) { }

Entity& MatrixBufferedEntity::move(const glm::vec3 &operation) { dirty(); return TransformableEntity::move(operation);}

Entity& MatrixBufferedEntity::scale(const glm::vec3 &scale) { dirty(); return TransformableEntity::scale(scale); }
Entity& MatrixBufferedEntity::scale(float scale) { dirty(); return TransformableEntity::scale(scale); }

Entity& MatrixBufferedEntity::rotate(const glm::vec3 &rotation) { dirty(); return TransformableEntity::rotate(rotation); }
Entity& MatrixBufferedEntity::rotateX(float angle) { dirty(); return TransformableEntity::rotateX(angle); }
Entity& MatrixBufferedEntity::rotateY(float angle) { dirty(); return TransformableEntity::rotateY(angle); }
Entity& MatrixBufferedEntity::rotateZ(float angle) { dirty(); return TransformableEntity::rotateZ(angle); }

Entity& MatrixBufferedEntity::setPosition(const glm::vec3 &position) { dirty(); return TransformableEntity::setPosition(position); }
Entity& MatrixBufferedEntity::setRotation(const glm::vec3 &rotation) { dirty(); return TransformableEntity::setRotation(rotation); }
Entity& MatrixBufferedEntity::setScale(const glm::vec3 &scale) { dirty(); return TransformableEntity::setScale(scale); }
Entity& MatrixBufferedEntity::setScale(float scale) { dirty(); return TransformableEntity::setScale(scale); }

glm::mat4x4 MatrixBufferedEntity::getTransformationMatrix() const { rebuild(); return k_buffer->transformationMatrix; }
glm::mat3x3 MatrixBufferedEntity::getNormalMatrix() const { return k_buffer->normalMatrix; }

void MatrixBufferedEntity::rebuild() const {
    if (isDirty()) {
        k_buffer->transformationMatrix = calculateTransformationMatrix();
        k_buffer->normalMatrix = calculateNormalMatrix();
        dirty(false);
    }
}
bool MatrixBufferedEntity::isDirty() const { return k_buffer->hasTransformChange; }
void MatrixBufferedEntity::dirty(bool value) const {
    k_buffer->hasTransformChange = value;
}

//// ---- TransformedEntity ---- ////
TransformedEntity::TransformedEntity(int id,
    const glm::mat4 &transform, const glm::mat3 &normal)
    : Entity(id), k_mat_transform(transform),
    k_mat_normal(normal) { }

glm::mat4x4 TransformedEntity::getTransformationMatrix() const { return k_mat_transform; }
glm::mat3x3 TransformedEntity::getNormalMatrix() const { return k_mat_normal; }

//// ---- Entity2D ---- ////

Entity2D::Entity2D() : id(-1) { }

Entity2D::Entity2D(int id,
    const std::shared_ptr<GLModel>& model,
    const std::shared_ptr<GLTexture2D>& texture)
    : id(id), model(model), texture(texture) { }


const std::shared_ptr<GLTexture2D> Entity2D::getTexture() const { return texture; }
const std::shared_ptr<GLModel> Entity2D::getModel() const { return model; }

void Entity2D::setTexture(const std::shared_ptr<GLTexture2D> texture) { this->texture = texture; }
void Entity2D::setModel(const std::shared_ptr<GLModel> model) { this->model = model; }

int Entity2D::getID() const { return id; }
void Entity2D::setID(int pID) { this->id = pID; }

// ---- TransformedEntity2D ---- //
TransformedEntity2D::TransformedEntity2D()
    : transform(1.0f) { }

TransformedEntity2D::TransformedEntity2D(int id,
    const std::shared_ptr<GLModel>& model,
    const std::shared_ptr<GLTexture2D>& texture,
    const glm::mat3x3 &transform)
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
    const glm::vec2 &position, const glm::vec2 &scale,
    float rotation
) : Entity2D(id, model, texture), entityPosition(position),
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

glm::mat3x3 TransformableEntity2D::calculateTransformationMatrix3D() const {
    glm::mat3x3 transform(1.0f);
    transform = glm::translate(transform, entityPosition);
    transform = glm::rotate(transform, entityRotation);
    transform = glm::scale(transform, entityScale);
    return transform;
}

glm::mat4x4 TransformableEntity2D::calculateTransformationMatrix() const
{
    glm::mat4x4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(entityPosition, 1.0f));
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
    const glm::vec2 &position,
    const glm::vec2 &scale,
    float rotation) : TransformableEntity2D(
        id, model, texture, position, scale, rotation) { }

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
