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

NYREM_USE_NAMESPACE

//// ---- Camera ---- ////
mat4x4 Camera::viewMatrix()
const noexcept {
    return mat4x4(1.0f);
}

mat4x4 Camera::projectionMatrix()
const noexcept {
    return mat4x4(1.0f);
}

//// ---- TransformedCamera --- ////
TransformedCamera::TransformedCamera()
noexcept :
	k_mat_view(1.0f), k_mat_projection(1.0f) { }

TransformedCamera::TransformedCamera(
	const mat4x4& view, const mat4x4& proj)
noexcept :
	k_mat_view(view), k_mat_projection(proj) { }

void TransformedCamera::setViewMatrix(const mat4x4 &mat)
noexcept {
	k_mat_view = mat;
}

void TransformedCamera::setProjectionMatrix(const mat4x4 &mat)
noexcept {
	k_mat_projection = mat;
}

mat4x4 TransformedCamera::viewMatrix()
const noexcept {
	return k_mat_view;
}
mat4x4 TransformedCamera::projectionMatrix()
const noexcept {
	return k_mat_projection;
}

//// ---- Camera ---- ////

Camera3D::Camera3D()
noexcept :
	nearPlane(0.01f), farPlane(100.0f),
	fov(90.0f), aspectRatio(1.0f),
	position(0.0f), rotation(0.0f) { }

Camera3D::Camera3D(float nearPlane, float farPlane, float fov, float aspectRatio)
noexcept :
	nearPlane(nearPlane), farPlane(farPlane),
	fov(fov), aspectRatio(aspectRatio),
	rotation(0.0f), position(0.0f) { }

Camera3D::Camera3D(
	float nearPlane, float farPlane, float fov, float aspectRatio,
	const vec3& position, const vec3& rotation
) noexcept :
	nearPlane(nearPlane), farPlane(farPlane),
	fov(fov), aspectRatio(aspectRatio),
	rotation(rotation), position(position) { }

Camera3D::Camera3D(
	float nearPlane, float farPlane, float fov, float aspectRatio,
	const vec3& position, float roll, float pitch, float yaw
) noexcept :
	nearPlane(nearPlane), farPlane(farPlane),
	fov(fov), aspectRatio(aspectRatio),
	rotation(roll, pitch, yaw), position(position) { }

vec3 Camera3D::getViewDirection() const noexcept {
    return vec3(
        -std::cos(getYaw()),
        std::sin(getPitch()),
        -std::sin(getPitch()));
}

vec3 Camera3D::getViewCrossDirection() const noexcept {
    mat4x4 matrix = glm::rotate(mat4x4(1.0f), getRoll(), getViewDirection());
    vec4 vector(
        -std::cos(getYaw()),
         std::sin(getPitch()),
        -std::sin(getPitch()),
        0.0);
    return vec3(matrix * vector);
}

Camera3D& Camera3D::setNearPlane(float nearPlane) noexcept {
    this->nearPlane = nearPlane;
    return *this;
}
Camera3D& Camera3D::setFarPlane(float farPlane) noexcept {
    this->farPlane = farPlane;
    return *this;
}
Camera3D& Camera3D::setFOV(float fov) noexcept {
    this->fov = fov;
    return *this;
}
Camera3D& Camera3D::setAspectRatio(float aspectRatio) noexcept {
    this->aspectRatio = aspectRatio;
    return *this;
}
Camera3D& Camera3D::setAspectRatio(int width, int height) noexcept {
    aspectRatio = static_cast<float>(width)
            / static_cast<float>(height);
    return *this;
}
Camera3D& Camera3D::setRoll(float roll) noexcept { rotation[0] = roll; return *this; }
Camera3D& Camera3D::setPitch(float pitch) noexcept { rotation[1] = pitch; return *this; }
Camera3D& Camera3D::setYaw(float yaw) noexcept { rotation[2] = yaw; return *this; }
Camera3D& Camera3D::changeRoll(float roll) noexcept { rotation[0] += roll; return *this; }
Camera3D& Camera3D::changePitch(float pitch) noexcept { rotation[1] += pitch; return *this; }
Camera3D& Camera3D::changeYaw(float yaw) noexcept { rotation[2] += yaw; return *this; }

float Camera3D::getNearPlane() const noexcept { return nearPlane; }
float Camera3D::getFarPlane() const noexcept { return farPlane; }
float Camera3D::getFOV() const noexcept { return fov; }
float Camera3D::getAspectRatio() const noexcept { return aspectRatio; }

float Camera3D::getRoll() const noexcept { return rotation[0]; }
float Camera3D::getPitch() const noexcept { return rotation[1]; }
float Camera3D::getYaw() const noexcept { return rotation[2]; }
vec3 Camera3D::getRotation() const noexcept { return rotation; }

float Camera3D::getX() const noexcept { return position[0]; }
float Camera3D::getY() const noexcept { return position[1]; }
float Camera3D::getZ() const noexcept { return position[2]; }
vec3 Camera3D::getPosition() const noexcept { return position; }

Camera3D& Camera3D::rotate(const vec3 & rotation)
noexcept { this->rotation += rotation; return *this;}
Camera3D& Camera3D::move(const vec3 & position)
noexcept { this->position += position; return *this; }

Camera3D& Camera3D::setRotation(const vec3 &rotation)
noexcept { this->rotation = rotation; return *this; }
Camera3D& Camera3D::setPosition(const vec3 &pposition)
noexcept { position = pposition; return *this; }

Camera3D& Camera3D::setX(float x) noexcept { position[0] = x; return *this; }
Camera3D& Camera3D::setY(float y) noexcept { position[1] = y; return *this; }
Camera3D& Camera3D::setZ(float z) noexcept { position[2] = z; return *this; }

mat4x4 Camera3D::viewMatrix() const noexcept { return calculateViewMatrix(); }
mat4x4 Camera3D::projectionMatrix() const noexcept { return calculateProjectionMatrix(); }

mat4x4 Camera3D::calculateViewMatrix() const noexcept {
    // roll can be removed from here. because is not actually used in FPS camera
    mat4x4 matRoll(1.0f), matPitch(1.0f), matYaw(1.0f);
    // roll, pitch and yaw are used to store our angles in our class
    matRoll = glm::rotate(matRoll, getRoll(), vec3(0.0f, 0.0f, 1.0f));
    matPitch = glm::rotate(matPitch, getPitch(), vec3(1.0f, 0.0f, 0.0f));
    matYaw = glm::rotate(matYaw, getYaw(), vec3(0.0f, 1.0f, 0.0f));

    // order matters
    mat4x4 rotate = matRoll * matPitch * matYaw;

    mat4x4 translate(1.0f);
    translate = glm::translate(translate, -position);
    return rotate * translate;
}

mat4x4 Camera3D::calculateProjectionMatrix() const noexcept {
    return glm::perspective(fov, aspectRatio, nearPlane, farPlane);
}

//// ---- MatrixBufferedCamera3D ---- ////

MatrixBufferedCamera3D::MatrixBufferedCamera3D(
    float pNearPlane, float pFarPlane, float pFOV, float pAspectRatio
) noexcept :
    Camera3D(pNearPlane, pFarPlane, pFOV, pAspectRatio) { }

MatrixBufferedCamera3D::MatrixBufferedCamera3D(
    float pNearPlane, float pFarPlane, float pFOV, float pAspectRatio,
    const vec3 &pPosition, const vec3 &pRotation
) noexcept :
    Camera3D(pNearPlane, pFarPlane, pFOV, pAspectRatio, pPosition, pRotation) { }

MatrixBufferedCamera3D::MatrixBufferedCamera3D(
    float pNearPlane, float pFarPlane, float pFOV, float pAspectRatio,
    const vec3 &pPosition, float pRoll, float pPitch, float pYaw
) noexcept :
    Camera3D(pNearPlane, pFarPlane, pFOV, pAspectRatio,
        pPosition, pRoll, pPitch, pYaw) { }

Camera3D& MatrixBufferedCamera3D::setNearPlane(float pNearPlane) noexcept { dirtyProjection(); return Camera3D::setNearPlane(pNearPlane);}
Camera3D& MatrixBufferedCamera3D::setFarPlane(float pFarPlane) noexcept { dirtyProjection(); return Camera3D::setFarPlane(pFarPlane); }
Camera3D& MatrixBufferedCamera3D::setFOV(float pFOV) noexcept { dirtyProjection(); return Camera3D::setFOV(pFOV); }
Camera3D& MatrixBufferedCamera3D::setAspectRatio(float pAspect) noexcept { dirtyProjection(); return Camera3D::setAspectRatio(pAspect); }
Camera3D& MatrixBufferedCamera3D::setAspectRatio(int pWidth, int pHeight) noexcept { dirtyProjection(); return Camera3D::setAspectRatio(pWidth, pHeight); }

Camera3D& MatrixBufferedCamera3D::setRoll(float pRoll) noexcept { dirtyView(); return Camera3D::setRoll(pRoll); }
Camera3D& MatrixBufferedCamera3D::setPitch(float pPitch) noexcept { dirtyView(); return Camera3D::setPitch(pPitch); }
Camera3D& MatrixBufferedCamera3D::setYaw(float pYaw) noexcept { dirtyView(); return Camera3D::setYaw(pYaw); }

Camera3D& MatrixBufferedCamera3D::changeRoll(float pRoll) noexcept { dirtyView(); return Camera3D::changeRoll(pRoll); }
Camera3D& MatrixBufferedCamera3D::changePitch(float pPitch) noexcept { dirtyView(); return Camera3D::changePitch(pPitch); }
Camera3D& MatrixBufferedCamera3D::changeYaw(float pYaw) noexcept { dirtyView(); return Camera3D::changeYaw(pYaw); }

Camera3D& MatrixBufferedCamera3D::rotate(const vec3 &pRotation) noexcept { dirtyView(); return Camera3D::rotate(pRotation); }
Camera3D& MatrixBufferedCamera3D::move(const vec3 &pPosition) noexcept { dirtyView(); return Camera3D::move(pPosition); }
Camera3D& MatrixBufferedCamera3D::setRotation(const vec3 &pRotation) noexcept { dirtyView(); return Camera3D::setRotation(pRotation); }
Camera3D& MatrixBufferedCamera3D::setX(float pX) noexcept { dirtyView(); return Camera3D::setX(pX); }
Camera3D& MatrixBufferedCamera3D::setY(float pY) noexcept { dirtyView(); return Camera3D::setY(pY); }
Camera3D& MatrixBufferedCamera3D::setZ(float pZ) noexcept { dirtyView(); return Camera3D::setZ(pZ); }
Camera3D& MatrixBufferedCamera3D::setPosition(const vec3 &pPosition) noexcept { dirtyView(); return Camera3D::setPosition(pPosition); }

bool MatrixBufferedCamera3D::isDirtyView() const noexcept { return m_hasViewChange;}
bool MatrixBufferedCamera3D::isDirtyProjection() const noexcept { return m_hasProjectionChange; }

void MatrixBufferedCamera3D::rebuildProjection() const noexcept {
    if (isDirtyProjection()) {
        m_projectionMatrix = calculateProjectionMatrix();
        dirtyProjection(false);
    }
}
void MatrixBufferedCamera3D::rebuildView() const noexcept {
    if (isDirtyView()) {
        m_viewMatrix = calculateViewMatrix();
        dirtyView(false);
    }
}

void MatrixBufferedCamera3D::dirtyProjection(bool value) const noexcept { m_hasProjectionChange = value; }
void MatrixBufferedCamera3D::dirtyView(bool value) const noexcept { m_hasViewChange = value; }

mat4x4 MatrixBufferedCamera3D::viewMatrix() const noexcept { rebuildView(); return m_viewMatrix; }
mat4x4 MatrixBufferedCamera3D::projectionMatrix() const noexcept{ rebuildProjection(); return m_projectionMatrix; }

// ---- Camera2D ---- //
Camera2D::Camera2D(const vec2 &position, float rotation, float zoom)
noexcept :
    k_rotation(rotation),
    k_position(position),
    k_zoom(zoom) { }

float Camera2D::getX() const noexcept { return k_position.x; }
float Camera2D::getY() const noexcept { return k_position.y; }
float Camera2D::zoom() const noexcept { return k_zoom; }
const vec2& Camera2D::getPosition() const noexcept { return k_position; }
float Camera2D::getRotation() const noexcept { return k_rotation; }

void Camera2D::setX(float x) noexcept { k_position.x = x; }
void Camera2D::setY(float y) noexcept { k_position.y = y; }
void Camera2D::setPosition(float x, float y) noexcept { k_position = vec2(x, y); }
void Camera2D::setPosition(const vec2 &pos) noexcept { k_position = pos; }
void Camera2D::setZoom(float zoom) noexcept { k_zoom = zoom; }
void Camera2D::setRotation(float rotation) noexcept { k_rotation = rotation; }

void Camera2D::move(const vec2& pos) noexcept { k_position += pos; }
void Camera2D::rotate(float rotation) noexcept { k_rotation += rotation; }
void Camera2D::applyZoom(float zoom) noexcept { k_zoom *= zoom; }

mat4x4 Camera2D::calculateViewMatrix() const noexcept
{
    mat4x4 mat(1.0f);
    mat = glm::scale(mat, glm::vec3(k_zoom, k_zoom, 1.0f));
    mat = glm::rotate(mat, k_rotation, vec3(0.0f, 0.0f, 1.0f));
    mat = glm::translate(mat, vec3(-k_position.x, -k_position.y, 0.0f));
    return mat;
}

mat4x4 Camera2D::calculateProjectionMatrix() const noexcept
{
    mat4x4 mat(1.0f);
    return mat;
}

mat4x4 Camera2D::viewMatrix() const noexcept { return calculateViewMatrix(); }
mat4x4 Camera2D::projectionMatrix() const noexcept { return calculateProjectionMatrix(); }

// ---- MatrixBufferedCamera2D ---- //
MatrixBufferedCamera2D::MatrixBufferedCamera2D(
	const glm::vec2& position, float rotation, float zoom)
noexcept : Camera2D(position, rotation, zoom) { }
    

void MatrixBufferedCamera2D::setX(float x) noexcept { k_position.x = x; }
void MatrixBufferedCamera2D::setY(float y) noexcept { k_position.y = y; }
void MatrixBufferedCamera2D::setPosition(float x, float y) noexcept { k_position = {x, y}; }
void MatrixBufferedCamera2D::setPosition(const vec2 &pos) noexcept { k_position = pos; }

void MatrixBufferedCamera2D::setRotation(float rotation) noexcept { k_rotation = rotation; }

void MatrixBufferedCamera2D::move(const vec2& pos) noexcept { k_position += pos; }
void MatrixBufferedCamera2D::rotate(float rotation) noexcept { k_rotation += rotation; }

bool MatrixBufferedCamera2D::isDirty() const noexcept { return m_dirty; }
void MatrixBufferedCamera2D::dirty(bool value) const noexcept { m_dirty = value; }
void MatrixBufferedCamera2D::rebuild() const noexcept {
    if (isDirty()) {
        m_projMatrix = calculateProjectionMatrix();
        m_viewMatrix = calculateViewMatrix();
        dirty(false);
    }
}

mat4x4 MatrixBufferedCamera2D::viewMatrix() const noexcept { rebuild(); return m_viewMatrix; }
mat4x4 MatrixBufferedCamera2D::projectionMatrix() const noexcept { rebuild(); return m_projMatrix; }

//// ---- FreeCamera ---- ////
FreeCamera::FreeCamera()
{

}
