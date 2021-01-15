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

#ifndef NYREM_CAMERA_H
#define NYREM_CAMERA_H

#include "internal.hpp"
#include <glm/glm.hpp>

#ifdef NYREM_NAMESPACE
namespace nyrem {
#endif

/// <summary>
/// General interface for all camera objects. Cameras can be used to perform arbitrary
/// transformations in three dimensional space. Each camera needs to implement the viewMatrix
/// and projectionMatrix functions that return the given given transformation. The default
/// implementation returns the identity matrix for both.
/// </summary>
class Camera {
public:
	/// <summary>
	/// Allows deleting objects using the base class.
	/// </summary>
	virtual ~Camera() = default;

	/// <summary>
	/// Creates the view matrix associated with this Camera.
	/// The default implementation returns the identity matrix.
	/// </summary>
	/// <returns>The 4x4 view matrix</returns>
	virtual mat4x4 viewMatrix() const noexcept;

	/// <summary>
	/// Creates the projection matrix associated with this Camera.
	/// The default implementation returns the identity matrx.
	/// </summary>
	/// <returns>The 4x4 projection matrix</returns>
	virtual mat4x4 projectionMatrix() const noexcept;
};

/// <summary>
/// The transformed camera implements fixed view and projection transformations.
/// They can be changed by using the setViewMatrix and setProjectionMatrix functions.
/// </summrary>
class TransformedCamera : public Camera {
public:
	/// <summary>
	/// Creates a Camera with the identity matrix as view and projection matrix.
	/// </summary>
	TransformedCamera() noexcept;

	/// <summary>
	/// Creates a Camera with the given view and projection matrix.
	/// </summary>
	/// <param name="view">The fixed view matrix</param>
	/// <param name="proj">The fixed projection matrix</param>
	TransformedCamera(const mat4x4 &view, const mat4x4 &proj) noexcept;

	/// <summary>
	/// Allows deleting objects using the base class.
	/// </summary>
	virtual ~TransformedCamera() = default;

	/// <summary>Sets the view matrix</summary>
	/// <param name="mat">The new 4x4 view matrix</param>
	void setViewMatrix(const glm::mat4x4& mat) noexcept;

	/// <summary>Sets the projection matrix</summary>
	/// <param name="mat">The new 4x4 projection matrix</param>
	void setProjectionMatrix(const glm::mat4x4& mat) noexcept;

	/// <summary>Overriden method from Camera::viewMatrix</summary>
	/// <returns>The view matrix that was previously set</returns>
	virtual mat4x4 viewMatrix() const noexcept override;
	
	/// <summary>Overriden method from Camera::projectionMatrix</summary>
	/// <returns>The projection matrix that was previously set</returns>
	virtual mat4x4 projectionMatrix() const noexcept override;

protected:
	/// <summary>Stores the view  matrix</summary>
	mat4x4 k_mat_view;
	/// <summary>Stores the projection matrix</summary>
	mat4x4 k_mat_projection;
};

/// <summary>
/// A three dimensional camera that can be used to emulate a view position, angle,
/// fov, aspect ratio as well as the near and far plane. The camera settings can
/// be exported as a 4x4 matrix applying the defined transformation. The view and
/// projection matrices are not buffered and recalculated for every call of
/// projectionMatrix() or viewMatrix(). See MatrixBufferedCamera3D for an
/// implementation that caches the matrices.
/// </summary>
class Camera3D : public Camera {
public:
	/// <summary>
	/// Creates a camera with the following default settings: 
	/// nearPlane   = 0.01f
	/// farPlane    = 100.0f
	/// fov         = 90.0f
	/// aspectRatio = 1.0f
	/// position    = (0.0f, 0.0f, 0.0f)
	/// rotation    = (0.0f, 0.0f, 0.0f)
	/// </summary>
	Camera3D() noexcept;

	/// <summary>
	/// Creates a camera using the given render settings. Position and rotation are
	/// initialized with a the default value of (0.0f, 0.0f, 0.0f).
	/// </summary>
	/// <param name="nearPlane">The camera's near plane</param>
	/// <param name="farPlane">The camera's far plane</param>
	/// <param name="fov">The field of view angle in radians</param>
	/// <param name="aspectRatio">The viewports's aspect ratio</param>
	Camera3D(
		float nearPlane, float farPlane,
		float fov, float aspectRatio) noexcept;

	/// <summary>
	/// Creates a camera using the given render settings.
	/// </summary>
	/// <param name="nearPlane">The camera's near plane</param>
	/// <param name="farPlane">The camera's far plane</param>
	/// <param name="fov">The field of view angle in radians</param>
	/// <param name="aspectRatio">The viewport's aspect ratio</param>
	/// <param name="position">The camera's position</param>
	/// <param name="rotation">The camera's rotation</param>
	Camera3D(
		float nearPlane, float farPlane,
		float fov, float aspectRatio,
		const vec3& position,
		const vec3& rotation) noexcept;

	/// <summary
	/// Creates a camera using the given render settings.
	/// </summary>
	/// <param name="nearPlane">The camera's near plane</param>
	/// <param name="farPlane">The camera's far plane</param>
	/// <param name="fov">The field of view angle in radians</param>
	/// <param name="aspectRatio">The viewport's aspect ratio</param>
	/// <param name="position">The camera's position</param>
	/// <param name="roll">The camera's roll angle, same as rotation[0]</param>
	/// <param name="pitch">The camera's pitch angle, same as rotation[1]</param>
	/// <param name="yaw">The camera's yaw angle, same as rotation[2]</param>
	Camera3D(
		float nearPlane, float farPlane,
		float fov, float aspectRatio,
		const vec3& position,
		float roll, float pitch, float yaw) noexcept;

	/// <summary>
	/// Allows deleting child classes using the base class
	/// </summary>
	virtual ~Camera3D() = default;

	// ---- Render Parameters ---- //
	// Manipulating the basic render parameters
	Camera3D& setNearPlane(float nearPlane) noexcept;
	Camera3D& setFarPlane(float farPlane) noexcept;
	Camera3D& setFOV(float fov) noexcept;
	Camera3D& setAspectRatio(float aspect) noexcept;
	Camera3D& setAspectRatio(int width, int height) noexcept;

	// Finds the basic basic render parameters
	float getNearPlane() const noexcept;
	float getFarPlane() const noexcept;
	float getFOV() const noexcept;
	float getAspectRatio() const noexcept;

	// ---- Angle Parameters ---- //
	// Manipulating the rotation parameters
	Camera3D& setRoll(float roll) noexcept;
	Camera3D& setPitch(float pitch) noexcept;
	Camera3D& setYaw(float yaw) noexcept;
	Camera3D& changeRoll(float roll) noexcept;
	Camera3D& changePitch(float pitch) noexcept;
	Camera3D& changeYaw(float yaw) noexcept;
	Camera3D& setRotation(const vec3& rotation) noexcept;
	Camera3D& rotate(const vec3& model) noexcept;

	// Finds the camera angles
	float getRoll() const noexcept;
	float getPitch() const noexcept;
	float getYaw() const noexcept;
	vec3 getRotation() const noexcept;

	// ---- Position ---- //
	float getX() const noexcept;
	float getY() const noexcept;
	float getZ() const noexcept;
	glm::vec3 getPosition() const noexcept;

	// Allows manipulating the position parameters
	Camera3D& setX(float x) noexcept;
	Camera3D& setY(float y) noexcept;
	Camera3D& setZ(float z) noexcept;
	Camera3D& setPosition(const vec3& model) noexcept;
	Camera3D& move(const vec3& model) noexcept;

	// ---- Complex Functions ---- //
	vec3 getViewDirection() const noexcept;
	vec3 getViewCrossDirection() const noexcept;

	/// <summary>Overriden method from Camera::viewMatrix</summary>
	/// <returns>The newly calculated view matrix from the view settings</returns>
	virtual mat4x4 viewMatrix() const noexcept override;

	/// <summary>Overriden method from Camera::projectionMatrix</summary>
	/// <returns>The newly calculated projection matrix from the view settings</returns>
	virtual mat4x4 projectionMatrix() const noexcept override;

	/// <summary>
	/// Calculates the view matrix by applying the transformations in the
	/// following order: 1. translation, 2. rotation.
	/// </summary>
	mat4x4 calculateViewMatrix() const noexcept;
	/// <summary>
	/// Calculates the projection matrix given by the render parameters
	/// </summary>
	mat4x4 calculateProjectionMatrix() const noexcept;

protected:
	float nearPlane, farPlane, fov, aspectRatio;
	vec3 position, rotation;
};

/// <summary>
/// Subclass inheriting from Camera3D.
/// The camera stores its own transformation matrix
/// that is updated whenever a parameter is changed.
class MatrixBufferedCamera3D : public Camera3D {
public:
	/// Creates a buffered camera using the neccessary render settings.
	/// Position and rotation is set to the default value 0.
	/// nearPlane:      The camera's near plane
	/// farPlane:       The camera's far plane
	/// fov:            The field of view angle in radians
	/// aspectRation:   The viewports's aspect ratio
	MatrixBufferedCamera3D(
		float nearPlane, float farPlane,
		float fov, float aspectRatio);

	/// Creates a buffered camera using the neccessary render settings
	/// with a custom position and rotation.
	/// nearPlane:      The camera's near plane
	/// farPlane:       The camera's far plane
	/// fov:            The field of view angle in radians
	/// aspectRation:   The viewport's aspect ratio
	/// position:       The camera's position
	/// rotation:       The camera's rotation
	MatrixBufferedCamera3D(
		float nearPlane, float farPlane,
		float fov, float aspectRatio,
		const glm::vec3& position,
		const glm::vec3& rotation);

	/// Creates a camera using the neccessary render settings
	/// with a custom position and rotation.
	/// nearPlane:      The camera's near plane
	/// farPlane:       The camera's far plane
	/// fov:            The field of view angle in radians
	/// aspectRation:   The viewport's aspect ratio
	/// roll:           The camera's roll angle. Same as rotation[0]
	/// pitch:          The camera's pitch angle. Same as rotation[1]
	/// yaw:            The camera's yaw angle. Same as rotation[2]
	MatrixBufferedCamera3D(
		float nearPlane, float farPlane,
		float fov, float aspectRatio,
		const glm::vec3& position,
		float roll, float pitch, float yaw);

	virtual Camera3D& setNearPlane(float nearPlane);
	virtual Camera3D& setFarPlane(float farPlane);
	virtual Camera3D& setFOV(float fov);
	virtual Camera3D& setAspectRatio(float aspect);
	virtual Camera3D& setAspectRatio(int width, int height);

	// Camera3D angles
	virtual Camera3D& setRoll(float roll);
	virtual Camera3D& setPitch(float pitch);
	virtual Camera3D& setYaw(float yaw);
	virtual Camera3D& changeRoll(float roll);
	virtual Camera3D& changePitch(float pitch);
	virtual Camera3D& changeYaw(float yaw);
	virtual Camera3D& rotate(const glm::vec3& model);
	virtual Camera3D& setRotation(const glm::vec3& rotation);

	virtual Camera3D& setX(float x);
	virtual Camera3D& setY(float y);
	virtual Camera3D& setZ(float z);
	virtual Camera3D& move(const glm::vec3& model);
	virtual Camera3D& setPosition(const glm::vec3& position);

	virtual glm::mat4x4 viewMatrix() const override;
	virtual glm::mat4x4 projectionMatrix() const override;

	void rebuildProjection() const;
	void rebuildView() const;

	void dirtyProjection(bool value = true) const;
	void dirtyView(bool value = true) const;

	bool isDirtyView() const;
	bool isDirtyProjection() const;
protected:
	struct MatrixBuffer {
		glm::mat4x4 viewMatrix = glm::mat4x4(1.0f);
		glm::mat4x4 projectionMatrix = glm::mat4x4(1.0f);
		bool hasViewChange = true;
		bool hasProjectionChange = true;
	};

	std::unique_ptr<MatrixBuffer> k_buffer
		= std::make_unique<MatrixBuffer>();
};

//// ---- Camera2D ---- ////

class Camera2D : public Camera {
public:
	Camera2D(
		const glm::vec2& position = { 0.0f, 0.0f },
		float rotation = 0.0f);
	virtual ~Camera2D() = default;

	float getX() const;
	float getY() const;
	const glm::vec2& getPosition() const;
	float getRotation() const;;

	virtual void setX(float x);
	virtual void setY(float y);
	virtual void setPosition(float x, float y);
	virtual void setPosition(const glm::vec2& pos);

	virtual void setRotation(float rotation);

	virtual void move(const glm::vec2& pos);
	virtual void rotate(float rotation);

	glm::mat4x4 calculateViewMatrix() const;
	glm::mat4x4 calculateProjectionMatrix() const;

	virtual glm::mat4x4 viewMatrix() const;
	virtual glm::mat4x4 projectionMatrix() const;

protected:
	float k_rotation;
	glm::vec2 k_position;
};


class MatrixBufferedCamera2D : public Camera2D {
public:
	MatrixBufferedCamera2D(
		const glm::vec2& position = { 0.0f, 0.0f },
		float rotation = 0.0f);
	virtual ~MatrixBufferedCamera2D() = default;

	virtual void setX(float x);
	virtual void setY(float y);
	virtual void setPosition(float x, float y);
	virtual void setPosition(const glm::vec2& pos);

	virtual void setRotation(float rotation);

	virtual void move(const glm::vec2& pos);
	virtual void rotate(float rotation);

	bool isDirty() const;
	void dirty(bool value = true) const;
	void rebuild() const;

	virtual glm::mat4x4 viewMatrix() const;
	virtual glm::mat4x4 projectionMatrix() const;
protected:
	struct MatrixBuffer {
		glm::mat4x4 k_mat_view = glm::mat4x4(1.0f);
		glm::mat4x4 k_mat_proj = glm::mat4x4(1.0f);
		bool k_dirty = true;
	};
	std::unique_ptr<MatrixBuffer> k_buffer
		= std::make_unique<MatrixBuffer>();
};

class FreeCamera : public Camera3D {
public:
	FreeCamera();
	virtual ~FreeCamera() = default;
};

#ifdef NYREM_NAMESPACE
} // namespace nyrem
#endif

#endif // CAMERA_H
