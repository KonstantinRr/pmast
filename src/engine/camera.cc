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

#include <engine/camera.hpp>

#include <glm/gtc/matrix_transform.hpp>

using namespace nyrem;

//// ---- Camera ---- ////
mat4x4 Camera::viewMatrix() const noexcept { return mat4x4(1.0f); }
mat4x4 Camera::projectionMatrix() const noexcept { return mat4x4(1.0f); }


//// ---- TransformedCamera --- ////
TransformedCamera::TransformedCamera() noexcept :
	k_mat_view(1.0f), k_mat_projection(1.0f) { }
TransformedCamera::TransformedCamera(
		const glm::mat4x4 &view,
		const glm::mat4x4 &proj) noexcept :
	k_mat_view(view), k_mat_projection(proj) { }

void TransformedCamera::setViewMatrix(const glm::mat4x4 &mat) { k_mat_view = mat; }
void TransformedCamera::setProjectionMatrix(const glm::mat4x4 &mat) { k_mat_projection = mat; }

glm::mat4x4 TransformedCamera::viewMatrix() const { return k_mat_view; }
glm::mat4x4 TransformedCamera::projectionMatrix() const { return k_mat_projection; }

//// ---- Camera ---- ////

Camera3D::Camera3D() :
    nearPlane(0.01f), farPlane(100.0f),
    fov(90.0f), aspectRatio(1.0f),
    position(0.0f), rotation(0.0f) { }

Camera3D::Camera3D(float pNearPlane, float pFarPlane, float pFOV, float pAspectRatio) {
    this->nearPlane = pNearPlane;
    this->farPlane = pFarPlane;
    this->fov = pFOV;
    this->aspectRatio = pAspectRatio;
    this->position = glm::vec3(0.0f, 0.0f, 0.0f);
    this->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
}

Camera3D::Camera3D(
    float pNearPlane, float pFarPlane, float pFOV, float pAspectRatio,
    const glm::vec3 &pPosition,
    const glm::vec3 &pRotation
) {
    this->position = pPosition;
    this->rotation = pRotation;
    this->nearPlane = pNearPlane;
    this->farPlane = pFarPlane;
    this->fov = pFOV;
    this->aspectRatio = pAspectRatio;
}

Camera3D::Camera3D(
    float pNearPlane, float pFarPlane, float pFOV, float pAspectRatio,
    const glm::vec3 &pPosition,
    float pRoll, float pPitch, float pYaw) {
    this->position = pPosition;
    this->rotation = glm::vec3(pRoll, pPitch, pYaw);
    this->nearPlane = pNearPlane;
    this->farPlane = pFarPlane;
    this->fov = pFOV;
    this->aspectRatio = pAspectRatio;
}

glm::vec3 Camera3D::getViewDirection() const {
    return glm::vec3(
        -std::cos(getYaw()),
        std::sin(getPitch()),
        -std::sin(getPitch()));
}

glm::vec3 Camera3D::getViewCrossDirection() const {
    glm::mat4x4 matrix(1.0f);
    matrix = glm::rotate(matrix, getRoll(), getViewDirection());
    glm::vec4 vector(
        -std::cos(getYaw()),
         std::sin(getPitch()),
        -std::sin(getPitch()),
        0.0);
    return glm::vec3(matrix * vector);
}

Camera3D& Camera3D::setNearPlane(float pNearPlane) {
    this->nearPlane = pNearPlane;
    return *this;
}
Camera3D& Camera3D::setFarPlane(float pFarPlane) {
    this->farPlane = pFarPlane;
    return *this;
}
Camera3D& Camera3D::setFOV(float pFOV) {
    this->fov = pFOV;
    return *this;
}
Camera3D& Camera3D::setAspectRatio(float pAspect) {
    this->aspectRatio = pAspect;
    return *this;
}
Camera3D& Camera3D::setAspectRatio(int width, int height) {
    this->aspectRatio = static_cast<float>(width)
            / static_cast<float>(height);
    return *this;
}
Camera3D& Camera3D::setRoll(float roll) { rotation[0] = roll; return *this; }
Camera3D& Camera3D::setPitch(float pitch) { rotation[1] = pitch; return *this; }
Camera3D& Camera3D::setYaw(float yaw) { rotation[2] = yaw; return *this; }
Camera3D& Camera3D::changeRoll(float roll) { rotation[0] += roll; return *this; }
Camera3D& Camera3D::changePitch(float pitch) { rotation[1] += pitch; return *this; }
Camera3D& Camera3D::changeYaw(float yaw) { rotation[2] += yaw; return *this; }

float Camera3D::getNearPlane() const { return nearPlane; }
float Camera3D::getFarPlane() const { return farPlane; }
float Camera3D::getFOV() const { return fov; }
float Camera3D::getAspectRatio() const { return aspectRatio; }

float Camera3D::getRoll() const { return rotation[0]; }
float Camera3D::getPitch() const { return rotation[1]; }
float Camera3D::getYaw() const { return rotation[2]; }
glm::vec3 Camera3D::getRotation() const { return rotation; }

float Camera3D::getX() const { return position[0]; }
float Camera3D::getY() const { return position[1]; }
float Camera3D::getZ() const { return position[2]; }
glm::vec3 Camera3D::getPosition() const { return position; }

Camera3D& Camera3D::rotate(const glm::vec3 &protation) { rotation += protation; return *this;}
Camera3D& Camera3D::move(const glm::vec3 &pposition) { position += pposition; return *this; }

Camera3D& Camera3D::setRotation(const glm::vec3 &protation) { rotation = protation; return *this; }
Camera3D& Camera3D::setPosition(const glm::vec3 &pposition) { position = pposition; return *this; }

Camera3D& Camera3D::setX(float x) { position[0] = x; return *this; }
Camera3D& Camera3D::setY(float y) { position[1] = y; return *this; }
Camera3D& Camera3D::setZ(float z) { position[2] = z; return *this; }

glm::mat4x4 Camera3D::viewMatrix() const { return calculateViewMatrix(); }
glm::mat4x4 Camera3D::projectionMatrix() const { return calculateProjectionMatrix(); }

glm::mat4x4 Camera3D::calculateViewMatrix() const{
    // roll can be removed from here. because is not actually used in FPS camera
    glm::mat4x4 matRoll(1.0f), matPitch(1.0f), matYaw(1.0f);
    // roll, pitch and yaw are used to store our angles in our class
    matRoll = glm::rotate(matRoll, getRoll(), glm::vec3(0.0f, 0.0f, 1.0f));
    matPitch = glm::rotate(matPitch, getPitch(), glm::vec3(1.0f, 0.0f, 0.0f));
    matYaw = glm::rotate(matYaw, getYaw(), glm::vec3(0.0f, 1.0f, 0.0f));

    // order matters
    glm::mat4x4 rotate = matRoll * matPitch * matYaw;

    glm::mat4x4 translate(1.0f);
    translate = glm::translate(translate, -position);
    return rotate * translate;
}

glm::mat4x4 Camera3D::calculateProjectionMatrix() const{
    return glm::perspective(fov, aspectRatio, nearPlane, farPlane);
}

//// ---- MatrixBufferedCamera3D ---- ////

MatrixBufferedCamera3D::MatrixBufferedCamera3D(float pNearPlane, float pFarPlane, float pFOV, float pAspectRatio)
    : Camera3D(pNearPlane, pFarPlane, pFOV, pAspectRatio) { }
MatrixBufferedCamera3D::MatrixBufferedCamera3D(float pNearPlane, float pFarPlane, float pFOV, float pAspectRatio,
    const glm::vec3 &pPosition, const glm::vec3 &pRotation)
    : Camera3D(pNearPlane, pFarPlane, pFOV, pAspectRatio, pPosition, pRotation) { }
MatrixBufferedCamera3D::MatrixBufferedCamera3D(float pNearPlane, float pFarPlane, float pFOV, float pAspectRatio,
    const glm::vec3 &pPosition, float pRoll, float pPitch, float pYaw)
    : Camera3D(pNearPlane, pFarPlane, pFOV, pAspectRatio, pPosition, pRoll, pPitch, pYaw) { }

Camera3D& MatrixBufferedCamera3D::setNearPlane(float pNearPlane) { dirtyProjection(); return Camera3D::setNearPlane(pNearPlane);}
Camera3D& MatrixBufferedCamera3D::setFarPlane(float pFarPlane) { dirtyProjection(); return Camera3D::setFarPlane(pFarPlane); }
Camera3D& MatrixBufferedCamera3D::setFOV(float pFOV) { dirtyProjection(); return Camera3D::setFOV(pFOV); }
Camera3D& MatrixBufferedCamera3D::setAspectRatio(float pAspect) { dirtyProjection(); return Camera3D::setAspectRatio(pAspect); }
Camera3D& MatrixBufferedCamera3D::setAspectRatio(int pWidth, int pHeight) { dirtyProjection(); return Camera3D::setAspectRatio(pWidth, pHeight); }

Camera3D& MatrixBufferedCamera3D::setRoll(float pRoll) { dirtyView(); return Camera3D::setRoll(pRoll); }
Camera3D& MatrixBufferedCamera3D::setPitch(float pPitch) { dirtyView(); return Camera3D::setPitch(pPitch); }
Camera3D& MatrixBufferedCamera3D::setYaw(float pYaw) { dirtyView(); return Camera3D::setYaw(pYaw); }

Camera3D& MatrixBufferedCamera3D::changeRoll(float pRoll) { dirtyView(); return Camera3D::changeRoll(pRoll); }
Camera3D& MatrixBufferedCamera3D::changePitch(float pPitch) { dirtyView(); return Camera3D::changePitch(pPitch); }
Camera3D& MatrixBufferedCamera3D::changeYaw(float pYaw) { dirtyView(); return Camera3D::changeYaw(pYaw); }

Camera3D& MatrixBufferedCamera3D::rotate(const glm::vec3 &pRotation) { dirtyView(); return Camera3D::rotate(pRotation); }
Camera3D& MatrixBufferedCamera3D::move(const glm::vec3 &pPosition) { dirtyView(); return Camera3D::move(pPosition); }
Camera3D& MatrixBufferedCamera3D::setRotation(const glm::vec3 &pRotation) { dirtyView(); return Camera3D::setRotation(pRotation); }
Camera3D& MatrixBufferedCamera3D::setX(float pX) { dirtyView(); return Camera3D::setX(pX); }
Camera3D& MatrixBufferedCamera3D::setY(float pY) { dirtyView(); return Camera3D::setY(pY); }
Camera3D& MatrixBufferedCamera3D::setZ(float pZ) { dirtyView(); return Camera3D::setZ(pZ); }
Camera3D& MatrixBufferedCamera3D::setPosition(const glm::vec3 &pPosition) { dirtyView(); return Camera3D::setPosition(pPosition); }

bool MatrixBufferedCamera3D::isDirtyView() const { return k_buffer->hasViewChange;}
bool MatrixBufferedCamera3D::isDirtyProjection() const { return k_buffer->hasProjectionChange; }

void MatrixBufferedCamera3D::rebuildProjection() const {
    if (isDirtyProjection()) {
        k_buffer->projectionMatrix = calculateProjectionMatrix();
        dirtyProjection(false);
    }
}
void MatrixBufferedCamera3D::rebuildView() const {
    if (isDirtyView()) {
        k_buffer->viewMatrix = calculateViewMatrix();
        dirtyView(false);
    }
}

void MatrixBufferedCamera3D::dirtyProjection(bool value) const { k_buffer->hasProjectionChange = value; }
void MatrixBufferedCamera3D::dirtyView(bool value) const { k_buffer->hasViewChange = value; }

glm::mat4x4 MatrixBufferedCamera3D::viewMatrix() const { rebuildView(); return k_buffer->viewMatrix; }
glm::mat4x4 MatrixBufferedCamera3D::projectionMatrix() const { rebuildProjection(); return k_buffer->projectionMatrix; }

// ---- Camera2D ---- //
Camera2D::Camera2D(const glm::vec2 &position, float rotation) :
    k_rotation(rotation), k_position(position) { }

float Camera2D::getX() const { return k_position.x; }
float Camera2D::getY() const { return k_position.y; }
const glm::vec2& Camera2D::getPosition() const { return k_position; }
float Camera2D::getRotation() const { return k_rotation; }

void Camera2D::setX(float x) { k_position.x = x; }
void Camera2D::setY(float y) { k_position.y = y; }
void Camera2D::setPosition(float x, float y) { k_position = glm::vec2(x, y); }
void Camera2D::setPosition(const glm::vec2 &pos) { k_position = pos; }

void Camera2D::setRotation(float rotation) { k_rotation = rotation; }

void Camera2D::move(const glm::vec2& pos) { k_position += pos; }
void Camera2D::rotate(float rotation) { k_rotation += rotation; }

glm::mat4x4 Camera2D::calculateViewMatrix() const
{
    glm::mat4x4 mat(1.0f);
    mat = glm::translate(mat, -glm::vec3(k_position.x, k_position.y, 0.0f));
    mat = glm::rotate(mat, k_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    return mat;
}

glm::mat4x4 Camera2D::calculateProjectionMatrix() const
{
    glm::mat4x4 mat(1.0f);
    return mat;
}

glm::mat4x4 Camera2D::viewMatrix() const { return calculateViewMatrix(); }
glm::mat4x4 Camera2D::projectionMatrix() const { return calculateProjectionMatrix(); }

// ---- MatrixBufferedCamera2D ---- //
void MatrixBufferedCamera2D::setX(float x) { k_position.x = x; }
void MatrixBufferedCamera2D::setY(float y) { k_position.y = y; }
void MatrixBufferedCamera2D::setPosition(float x, float y) { k_position = {x, y}; }
void MatrixBufferedCamera2D::setPosition(const glm::vec2 &pos) { k_position = pos; }

void MatrixBufferedCamera2D::setRotation(float rotation) { k_rotation = rotation; }

void MatrixBufferedCamera2D::move(const glm::vec2& pos) { k_position += pos; }
void MatrixBufferedCamera2D::rotate(float rotation) { k_rotation += rotation; }

bool MatrixBufferedCamera2D::isDirty() const { return k_buffer->k_dirty; }
void MatrixBufferedCamera2D::dirty(bool value) const { k_buffer->k_dirty = value; }
void MatrixBufferedCamera2D::rebuild() const {
    if (isDirty()) {
        k_buffer->k_mat_proj = calculateProjectionMatrix();
        k_buffer->k_mat_view = calculateViewMatrix();
        dirty(false);
    }
}

glm::mat4x4 MatrixBufferedCamera2D::viewMatrix() const { rebuild(); return k_buffer->k_mat_view; }
glm::mat4x4 MatrixBufferedCamera2D::projectionMatrix() const { rebuild(); return k_buffer->k_mat_proj; }

//// ---- FreeCamera ---- ////
FreeCamera::FreeCamera()
{

}
