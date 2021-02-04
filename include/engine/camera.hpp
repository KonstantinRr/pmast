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
//export module nyrem:camera;

#include <engine/internal.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

NYREM_NAMESPACE_BEGIN

class ViewTransformer {
public:
	virtual ~ViewTransformer() = default;

	virtual mat4x4 matrix() const noexcept { return mat4x4(1.0f); }
	virtual mat4x4 inverse() const noexcept { return mat4x4(1.0f); }
	virtual void passthrough(mat4x4 &mat) const noexcept { }
	virtual void passthrough(vec4 &vec) const noexcept { }
	virtual void passthroughInverse(mat4x4 &mat) const noexcept { }
	virtual void passthroughInverse(vec4 &vec) const noexcept { }
};

class ViewPipeline : public ViewTransformer {
public:
	virtual ~ViewPipeline() = default;

	virtual mat4x4 projectionMatrix() const noexcept { return mat4x4(1.0f); }
	virtual mat4x4 viewMatrix() const noexcept { return mat4x4(1.0f); }
};

class CalculateMatrix {
public:
	static constexpr bool hasBuffer() { return false; }
};

class BufferMatrix {
protected:
	mutable mat4x4 buffer;

public:
	BufferMatrix() noexcept = default;

	static constexpr bool hasBuffer() { return true; }

	inline void store(const mat4x4 &mat) const noexcept { buffer = mat; }
	inline const mat4x4& get() const noexcept { return buffer; }
};

template<typename ForwardBuffer, typename BackwardBuffer>
class MatrixBuffer {
protected:
	ForwardBuffer m_bufferForward;
	BackwardBuffer m_bufferBackward;
	mutable bool m_dirtyForward;
	mutable bool m_dirtyBackward;

	template<typename Buffer, typename Exec>
	mat4x4 bufferOrMatrix(const Buffer &buffer, bool &dirty, Exec && exec) const noexcept {
		if constexpr (Buffer::hasBuffer()) {
			if (dirty) {
				auto storeMatrix = exec();
				buffer.store(storeMatrix);
				dirty = false;
				return storeMatrix;
			} else {
				return buffer.get();
			}
		} else {
			return exec();
		}
	}

public:
	MatrixBuffer() noexcept :
		m_dirtyForward(true), m_dirtyBackward(true) { }
	virtual ~MatrixBuffer() = default;

	inline bool isDirtyForward() const noexcept { return m_dirtyForward; }
	inline bool isDirtyBackward() const noexcept { return m_dirtyBackward; }

	inline void makeDirtyForward() const noexcept { m_dirtyForward = true; }
	inline void makeDirtyBackward() const noexcept { m_dirtyBackward = true; }
	inline void makeDirty() const noexcept {
		m_dirtyForward = true;
		m_dirtyBackward = true;
	}

	template<typename Exec>
	inline mat4x4 forwardBufferOrMatrix(Exec && exec) const noexcept {
		return bufferOrMatrix(m_bufferForward, m_dirtyForward, exec);
	}

	template<typename Exec>
	inline mat4x4 backwardBufferOrMatrix(Exec && exec) const noexcept {
		return bufferOrMatrix(m_bufferBackward, m_dirtyBackward, exec);
	}
};

template<typename ForwardBuffer, typename BackwardBuffer>
class DimensionScaler :
	protected MatrixBuffer<ForwardBuffer, BackwardBuffer> {
public:
	using TypeMatrixBuffer = MatrixBuffer<ForwardBuffer, BackwardBuffer>;

protected:
	size_t m_width, m_height;

public:
	DimensionScaler() noexcept : m_width(1), m_height(1) { }
	DimensionScaler(size_t width, size_t height) noexcept :
		m_width(width), m_height(height) { }
	virtual ~DimensionScaler() = default;

	float aspectRatio() const noexcept {
		return static_cast<float>(m_width) / static_cast<float>(m_height);
	}
	float inverseAspectRatio() const noexcept {
		return static_cast<float>(m_height) / static_cast<float>(m_width);
	}

	void setWidth(size_t width) noexcept {
		m_width = width;
    	TypeMatrixBuffer::makeDirty();
	}
	void setHeight(size_t height) noexcept {
		m_height = height;
    	TypeMatrixBuffer::makeDirty();
	}

	void updateMatrix(size_t width, size_t height) noexcept {
		m_width = width;
		m_height = height;
		TypeMatrixBuffer::makeDirty();
	}
};

template<
	typename ForwardBuffer = CalculateMatrix,
	typename BackwardBuffer = CalculateMatrix>
class HeightScaler :
	public ViewTransformer,
	public DimensionScaler<ForwardBuffer, BackwardBuffer> {
public:
	using TypeDimensionScaler = DimensionScaler<ForwardBuffer, BackwardBuffer>;

protected:
	mat4x4 heightScalerMatrixForward() const noexcept {
		return TypeDimensionScaler::TypeMatrixBuffer::forwardBufferOrMatrix(
        	[this]() { return glm::scale(mat4x4(1.0f),
        	    {this->widthScaleFactorForward(), 1.0f, 1.0f}); });
	}
	mat4x4 heightScalerMatrixBackward() const noexcept {
		return TypeDimensionScaler::TypeMatrixBuffer::forwardBufferOrMatrix(
        	[this]() { return glm::scale(mat4x4(1.0f),
        	    {this->widthScaleFactorBackward(), 1.0f, 1.0f}); });
	}

	inline float widthScaleFactorForward() const noexcept {
		return TypeDimensionScaler::inverseAspectRatio();
	}
	inline float widthScaleFactorBackward() const noexcept {
		return TypeDimensionScaler::aspectRatio();
	}

public:
	virtual ~HeightScaler() = default;
	
	virtual mat4x4 matrix() const noexcept override {
		return heightScalerMatrixForward();
	}
	virtual mat4x4 inverse() const noexcept override {
		return heightScalerMatrixBackward();
	}
	virtual void passthrough(mat4x4 &mat) const noexcept override {
		mat = glm::scale(mat, {widthScaleFactorForward(), 1.0f, 1.0f});
	}
	virtual void passthrough(vec4 &vec) const noexcept override {
		vec[0] *= widthScaleFactorForward();
	}
	virtual void passthroughInverse(mat4x4 &mat) const noexcept override {
		mat = glm::scale(mat, {widthScaleFactorBackward(), 1.0f, 1.0f});
	}
	virtual void passthroughInverse(vec4 &vec) const noexcept override {
		vec[0] *= widthScaleFactorBackward();
	}
};

template<
	typename ForwardBuffer = CalculateMatrix,
	typename BackwardBuffer = CalculateMatrix>
class WidthScaler :
	public ViewTransformer,
	public DimensionScaler<ForwardBuffer, BackwardBuffer> {
public:
	using TypeDimensionScaler = DimensionScaler<ForwardBuffer, BackwardBuffer>;

protected:
	mat4x4 widthScalerMatrixForward() const noexcept {
		return forwardBufferOrMatrix(
        	[this]() { return glm::scale(mat4x4(1.0f),
        	    {1.0f, this->heightScaleFactorForward(), 1.0f}); });
	}
	mat4x4 widthScalerMatrixBackward() const noexcept {
		return backwardBufferOrMatrix(
        	[this]() { return glm::scale(mat4x4(1.0f),
        	    {1.0f, this->heightScaleFactorBackward(), 1.0f}); });
	}
	inline float heightScaleFactorForward() const noexcept {
		return TypeDimensionScaler::aspectRatio();
	}
	inline float heightScaleFactorBackward() const noexcept {
		return TypeDimensionScaler::inverseAspectRatio();
	}

public:
	virtual ~WidthScaler() = default;

	virtual mat4x4 matrix() const noexcept override {
		return widthScalerMatrixForward();
	}
	virtual mat4x4 inverse() const noexcept override {
		return widthScalerMatrixBackward();
	}
	virtual void passthrough(mat4x4 &mat) const noexcept override {
		mat = glm::scale(mat, {1.0f, heightScaleFactorForward(), 1.0f});
	}
	virtual void passthrough(vec4 &vec) const noexcept override {
		vec[1] *= heightScaleFactorForward();
	}
	virtual void passthroughInverse(mat4x4 &mat) const noexcept override {
		mat = glm::scale(mat, {1.0f, heightScaleFactorBackward(), 1.0f});
	}
	virtual void passthroughInverse(vec4 &vec) const noexcept override {
		vec[1] *= heightScaleFactorBackward();
	}
};

/// <summary>
/// The transformed camera implements fixed view and projection transformations.
/// They can be changed by using the setViewMatrix and setProjectionMatrix functions.
/// </summrary>
template<typename BackwardBuffer = CalculateMatrix>
class TransformedCamera :
	public ViewTransformer,
	protected MatrixBuffer<CalculateMatrix, BackwardBuffer> {
public:
	using TypeMatrixBuffer = MatrixBuffer<CalculateMatrix, BackwardBuffer>;

protected:
	mat4x4 m_matrix;

	mat4x4 inverseCameraMatrix() const noexcept {
		return TypeMatrixBuffer::backwardBufferOrMatrix(
			[this]() { return glm::inverse(this->get()); });
	}

public:
	TransformedCamera() noexcept : m_matrix(1.0f) { }
	TransformedCamera(const mat4x4 &transform) noexcept
		: m_matrix(transform) { }

	virtual ~TransformedCamera() = default;

	inline void set(const mat4x4 &mat) noexcept {
		m_matrix = mat;
		TypeMatrixBuffer::makeDirty();
	}
	inline const mat4x4& get() const noexcept { return m_matrix; }

	virtual mat4x4 matrix() const noexcept override {return m_matrix; }
	virtual mat4x4 inverse() const noexcept override { return inverseCameraMatrix(); }
	virtual void passthrough(mat4x4 &mat) const noexcept override { mat = m_matrix * mat; }
	virtual void passthrough(vec4 &vec) const noexcept override { vec = m_matrix * vec; }
	virtual void passthroughInverse(mat4x4 &mat) const noexcept override { mat = inverseCameraMatrix() * mat; }
	virtual void passthroughInverse(vec4 &vec) const noexcept override { vec = inverseCameraMatrix() * vec; }
};

template<typename BackwardBuffer = CalculateMatrix>
class TransformedInverseCamera :
	public ViewTransformer,
	protected MatrixBuffer<CalculateMatrix, BackwardBuffer> {
public:
	using TypeMatrixBuffer = MatrixBuffer<CalculateMatrix, BackwardBuffer>;

protected:
	mat4x4 m_inverse_matrix;

	mat4x4 cameraMatrix() const noexcept {
		return forwardBufferOrMatrix(
			[this]() { return glm::inverse(this->m_inverse_matrix); });
	} 

public:
	TransformedInverseCamera() noexcept : m_inverse_matrix(1.0f) { }
	TransformedInverseCamera(const mat4x4 &transform) noexcept
		: m_inverse_matrix(transform) { }

	virtual ~TransformedInverseCamera() = default;

	void set(const mat4x4 &mat) noexcept {
		m_inverse_matrix = mat;
		TypeMatrixBuffer::makeDirty();
	}
	inline const mat4x4 get() const noexcept { return m_inverse_matrix; }

	virtual mat4x4 matrix() const noexcept override { return cameraMatrix(); }
	virtual mat4x4 inverse() const noexcept override { return m_inverse_matrix; }
	virtual void passthrough(mat4x4 &mat) const noexcept override { mat = cameraMatrix() * mat; }
	virtual void passthrough(vec4 &vec) const noexcept override { vec = cameraMatrix() * vec; }
	virtual void passthroughInverse(mat4x4 &mat) const noexcept override { mat = m_inverse_matrix * mat; }
	virtual void passthroughInverse(vec4 &vec) const noexcept override { vec = m_inverse_matrix * vec; }
};

struct Projection3DSettings {
	float m_nearPlane;
	float m_farPlane;
	float m_fov;
	float m_aspectRatio;

	constexpr Projection3DSettings() noexcept :
		m_nearPlane(0.01f), m_farPlane(100.0f),
		m_fov(90.0f), m_aspectRatio(1.0f) { }

	constexpr Projection3DSettings(float nearPlane, float farPlane,
		float fov, float aspectRatio) noexcept :
    	m_nearPlane(nearPlane), m_farPlane(farPlane),
    	m_fov(fov), m_aspectRatio(aspectRatio) { }
};

template<
	typename ForwardBuffer = BufferMatrix,
	typename BackwardBuffer = CalculateMatrix>
class Projection3D :
	public ViewTransformer,
	protected MatrixBuffer<ForwardBuffer, BackwardBuffer> {
public:
	using ThisType = Projection3D<ForwardBuffer, BackwardBuffer>;
	using Projection3DType = ThisType;

	using TypeMatrixBuffer = MatrixBuffer<ForwardBuffer, BackwardBuffer>;

protected:
	Projection3DSettings m_settings;

	mat4x4 projectionMatrix() const noexcept {
		return TypeMatrixBuffer::forwardBufferOrMatrix([this]() {
			return glm::perspective(
				m_settings.m_fov, m_settings.m_aspectRatio,
				m_settings.m_nearPlane, m_settings.m_farPlane); });
	}

	mat4x4 inverseProjectionMatrix() const noexcept {
		return TypeMatrixBuffer::backwardBufferOrMatrix([this]() {
			return glm::inverse(glm::perspective(
				m_settings.m_fov, m_settings.m_aspectRatio,
				m_settings.m_nearPlane, m_settings.m_farPlane)); });
	}

public:
	Projection3D() noexcept { }

	Projection3D(Projection3DSettings settings) noexcept :
    	m_settings(settings) { }

	// allows creating different buffered projections from each other
	template<typename PForwardBuffer, typename PBackwardBuffer>
	Projection3D(const Projection3D<PForwardBuffer, PBackwardBuffer> &proj) noexcept :
		m_settings(proj.settings()) { }


	virtual ~Projection3D() = default;

	void setNearPlane(float nearPlane) {
		m_settings.m_nearPlane = nearPlane;
		TypeMatrixBuffer::makeDirty();
	}
	void setFarPlane(float farPlane) {
		m_settings.m_farPlane = farPlane;
		TypeMatrixBuffer::makeDirty();
	}
	void setFOV(float fov) {
		m_settings.m_fov = fov;
		TypeMatrixBuffer::makeDirty();
	}
	void setAspectRatio(float aspectRatio) {
		m_settings.m_aspectRatio = aspectRatio;
		TypeMatrixBuffer::makeDirty();
	}

	inline const Projection3DSettings& settings() const noexcept { return m_settings; }

	virtual mat4x4 matrix() const noexcept override { return projectionMatrix(); }
	virtual mat4x4 inverse() const noexcept override { return inverseProjectionMatrix(); }
	virtual void passthrough(mat4x4 &mat) const noexcept override { mat = projectionMatrix() * mat; }
	virtual void passthrough(vec4 &vec) const noexcept override { vec = projectionMatrix() * vec; }
	virtual void passthroughInverse(mat4x4 &mat) const noexcept override { mat = inverseProjectionMatrix() * mat; }
	virtual void passthroughInverse(vec4 &vec) const noexcept override { vec = inverseProjectionMatrix() * vec; }
};

struct Translation3DSettings {
	using ThisType = Translation3DSettings;
	using Translation3DSettingsType = Translation3DSettings;

	vec3 translation;

	Translation3DSettings() noexcept : translation(0.0f) { }
	Translation3DSettings(float v) noexcept : translation(v) { }
	Translation3DSettings(float x, float y, float z) noexcept : translation(x, y, z) { }
	Translation3DSettings(const vec3& pos) noexcept : translation(pos) { }
};

template<
	typename ForwardBuffer = CalculateMatrix,
	typename BackwardBuffer = CalculateMatrix>
class Translation3D :
	public ViewTransformer, 
	protected MatrixBuffer<ForwardBuffer, BackwardBuffer> {
public:
	using ThisType = Translation3D<ForwardBuffer, BackwardBuffer>;
	using Translation3DType = ThisType;

	using TypeMatrixBuffer = MatrixBuffer<CalculateMatrix, BackwardBuffer>;

protected:
	Translation3DSettings m_settings;

	mat4x4 translationMatrix() const noexcept {
		return TypeMatrixBuffer::forwardBufferOrMatrix(
			[this]() { return glm::translate(mat4x4(1.0f), m_settings.translation); });
	}
	mat4x4 inverseTranslationMatrix() const noexcept {
		return TypeMatrixBuffer::backwardBufferOrMatrix(
			[this]() { return glm::translate(mat4x4(1.0f), -m_settings.translation); });
	}

public:
	Translation3D() noexcept { }
	Translation3D(const Translation3DSettings &settings) noexcept :
		m_settings(settings) { }

	template<typename PForwardBuffer, typename PBackwardBuffer>
	Translation3D(const Translation3D<PForwardBuffer, PBackwardBuffer> &trans) noexcept :
		m_settings(trans.settings()) { }

	virtual ~Translation3D() = default;

	const Translation3DSettings& settings() const noexcept { return m_settings; }

	void set(vec3 position) {
		m_settings.translation = position;
		TypeMatrixBuffer::makeDirty();
	}

	virtual mat4x4 matrix() const noexcept override { return translationMatrix(); }
	virtual mat4x4 inverse() const noexcept override { return inverseTranslationMatrix(); }
	
	virtual void passthrough(mat4x4 &mat) const noexcept override {
		mat = glm::translate(mat, m_settings.translation);
	}
	virtual void passthrough(vec4 &vec) const noexcept override {
		vec.x += m_settings.translation.x;
		vec.y += m_settings.translation.y;
		vec.z += m_settings.translation.z;
	}

	virtual void passthroughInverse(mat4x4 &mat) const noexcept override {
		mat = glm::translate(mat, -m_settings.translation);
	}
	virtual void passthroughInverse(vec4 &vec) const noexcept override {
		vec.x -= m_settings.translation.x;
		vec.y -= m_settings.translation.y;
		vec.z -= m_settings.translation.z;
	}
};

struct RotationSettings3DEuler {
	using ThisType = RotationSettings3DEuler;
	using RotationSettings3DEulerType = ThisType;

	vec3 m_rollPitchYaw;

	RotationSettings3DEuler() noexcept : m_rollPitchYaw(0.0f) { }
	RotationSettings3DEuler(float v) : m_rollPitchYaw(v) { }
	RotationSettings3DEuler(float roll, float pitch, float yaw) noexcept :
		m_rollPitchYaw(roll, pitch, yaw) { }	
	RotationSettings3DEuler(vec3 rollPitchYaw) noexcept :
		m_rollPitchYaw(rollPitchYaw) { }
};

struct RotationSettings3DQuat {
	using ThisType = RotationSettings3DQuat;
	using RotationSettings3DQuatType = ThisType;

	quat m_quaternion;

	RotationSettings3DQuat() noexcept { }
	RotationSettings3DQuat(const RotationSettings3DEuler& euler) noexcept :
		m_quaternion(euler.m_rollPitchYaw) { }
	RotationSettings3DQuat(float w, float x, float y, float z) noexcept :
		m_quaternion(w, x, y, z) { }
	RotationSettings3DQuat(const vec4 &quat) noexcept :
		m_quaternion(quat) { }
	RotationSettings3DQuat(const quat &quaternion) noexcept :
		m_quaternion(quaternion) { }
};

template<
	typename ForwardBuffer = BufferMatrix,
	typename BackwardBuffer = CalculateMatrix>
class Rotation3D :
	public ViewTransformer,
	protected MatrixBuffer<ForwardBuffer, BackwardBuffer> {
public:
	using ThisType = Rotation3D<ForwardBuffer, BackwardBuffer>;
	using Rotation3DType = ThisType;

	using TypeMatrixBuffer = MatrixBuffer<CalculateMatrix, BackwardBuffer>;

protected:
	quat m_quaternion;

	mat4x4 rotationMatrix() const noexcept {
		return TypeMatrixBuffer::forwardBufferOrMatrix(
        	[this]() { return glm::toMat4(m_quaternion); });
	}
	mat4x4 inverseRotationMatrix() const noexcept {
		return TypeMatrixBuffer::backwardBufferOrMatrix(
        	[this]() { return glm::inverse(glm::toMat4(m_quaternion)); });
	}

public:
	Rotation3D() = default;
	Rotation3D(const RotationSettings3DEuler& euler) noexcept :
		m_quaternion(euler.m_rollPitchYaw) { }
	Rotation3D(const RotationSettings3DQuat& quaternion) noexcept :
		m_quaternion(quaternion.m_quaternion) { }

	template<typename PForwardBuffer, typename PBackwardBuffer>
	Rotation3D(const Rotation3D<PForwardBuffer, PBackwardBuffer> &rot) noexcept :
		m_quaternion(rot.m_quaternion) { }
	~Rotation3D() = default;

	RotationSettings3DQuat quaternionSettings() const noexcept {
		return RotationSettings3DQuat(m_quaternion);
	}
	RotationSettings3DEuler eulerSettings() const noexcept {
		return RotationSettings3DEuler(); // TODO
	}

	void set(float roll, float pitch, float yaw) {
		m_quaternion = quat({roll, pitch, yaw});
		TypeMatrixBuffer::makeDirty();
	}
	void set(vec3 vec) {
		m_quaternion = quat(vec);
		TypeMatrixBuffer::makeDirty();
	}

	void set(float w, float x, float y, float z) {
		m_quaternion = quat(w, x, y, z);
		TypeMatrixBuffer::makeDirty();
	}
	void set(vec4 vec) {
		m_quaternion = quat(vec);
		TypeMatrixBuffer::makeDirty();
	}
	void set(quat quaternion) {
		m_quaternion = quaternion;
		TypeMatrixBuffer::makeDirty();
	}

	virtual mat4x4 matrix() const noexcept override { return rotationMatrix(); }
	virtual mat4x4 inverse() const noexcept override { return inverseRotationMatrix(); }
	virtual void passthrough(mat4x4 &mat) const noexcept override { mat = rotationMatrix() * mat; }
	virtual void passthrough(vec4 &vec) const noexcept override { vec = rotationMatrix() * vec; }
	virtual void passthroughInverse(mat4x4 &mat) const noexcept override { mat = inverseRotationMatrix() * mat; }
	virtual void passthroughInverse(vec4 &vec) const noexcept override { vec = inverseRotationMatrix() * vec; }
};

template<
	typename ForwardBuffer = CalculateMatrix,
	typename BackwardBuffer = CalculateMatrix>
class Translation2D :
	public ViewTransformer,
	protected MatrixBuffer<ForwardBuffer, BackwardBuffer> {
public:
	using ThisType = Translation2D<ForwardBuffer, BackwardBuffer>;
	using Translation2DType = ThisType;

	using TypeMatrixBuffer = MatrixBuffer<ForwardBuffer, BackwardBuffer>;

protected:
	vec2 m_translation;

	mat4x4 translationMatrix() const noexcept {
		return TypeMatrixBuffer::forwardBufferOrMatrix(
			[this]() { return glm::translate(mat4x4(1.0f), vec3(m_translation, 0.0f)); });
	}
	mat4x4 inverseTranslationMatrix() const noexcept {
		return TypeMatrixBuffer::backwardBufferOrMatrix(
			[this]() { return glm::translate(mat4x4(1.0f), vec3(-m_translation, 0.0f)); });
	}
public:
	Translation2D() noexcept : m_translation(0.0f) { }
	Translation2D(float v) noexcept : m_translation(v) { }
	Translation2D(float x, float y) noexcept : m_translation(x, y) { }
	Translation2D(const vec2& pos) noexcept : m_translation(pos) { }

	void set(vec2 translation) {
		m_translation = translation;
		TypeMatrixBuffer::makeDirty();
	}

	virtual ~Translation2D() = default;

	virtual mat4x4 matrix() const noexcept override { return translationMatrix(); }
	virtual mat4x4 inverse() const noexcept override { return inverseTranslationMatrix(); }
	virtual void passthrough(mat4x4 &mat) const noexcept override {
		mat = glm::translate(mat, vec3(m_translation, 0.0f));
	}
	virtual void passthrough(vec4 &vec) const noexcept override {
		vec.x += m_translation.x;
		vec.y += m_translation.y;
	}
	virtual void passthroughInverse(mat4x4 &mat) const noexcept override {
		mat = glm::translate(mat, vec3(-m_translation, 0.0f));
	}
	virtual void passthroughInverse(vec4 &vec) const noexcept override {
		vec.x -= m_translation.x;
		vec.y -= m_translation.y;
	}
};

template<
	typename ForwardBuffer = CalculateMatrix,
	typename BackwardBuffer = CalculateMatrix>
class Rotation2D :
	public ViewTransformer,
	protected MatrixBuffer<ForwardBuffer, BackwardBuffer> {
public:
	using TypeMatrixBuffer = MatrixBuffer<CalculateMatrix, BackwardBuffer>;

protected:
	float m_rotation;

	mat4x4 rotationMatrix() const noexcept {
		return TypeMatrixBuffer::forwardBufferOrMatrix(
			[this]() { return glm::rotate(m_rotation, vec3{0.0f, 1.0f, 0.0f}); });
	}

	mat4x4 inverseRotationMatrix() const noexcept {
		return TypeMatrixBuffer::forwardBufferOrMatrix(
			[this]() { return glm::rotate(-m_rotation, vec3{0.0f, 1.0f, 0.0f}); });
	}

public:
	Rotation2D() noexcept : m_rotation(0.0f) { }
	Rotation2D(float vec) noexcept : m_rotation(vec) { }
	virtual ~Rotation2D() = default;

	void set(float rotation) {
		m_rotation = rotation;
		TypeMatrixBuffer::makeDirty();
	}
	float get() const noexcept { return m_rotation; }

	virtual mat4x4 matrix() const noexcept override { return rotationMatrix(); }
	virtual mat4x4 inverse() const noexcept override { return inverseRotationMatrix(); }
	virtual void passthrough(mat4x4 &mat) const noexcept override { mat = rotationMatrix() * mat; }
	virtual void passthrough(vec4 &vec) const noexcept override { vec = rotationMatrix() * vec; }
	virtual void passthroughInverse(mat4x4 &mat) const noexcept override { mat = rotationMatrix() * mat; }
	virtual void passthroughInverse(vec4 &vec) const noexcept override { vec = rotationMatrix() * vec; }
};

template<
	typename ForwardBuffer = CalculateMatrix,
	typename BackwardBuffer = CalculateMatrix>
class Scale3D :
	public ViewTransformer,
	protected MatrixBuffer<ForwardBuffer, BackwardBuffer> {
public:
	using TypeMatrixBuffer = MatrixBuffer<CalculateMatrix, BackwardBuffer>;

protected:
	vec3 m_scale;

	vec3 inverseScaleVector() const noexcept {
		return {1.0 / m_scale.x, 1.0f / m_scale.y, 1.0f / m_scale.z};
	}

	mat4x4 scaleMatrix() const noexcept {
		return TypeMatrixBuffer::forwardBufferOrMatrix(
        	[this]() { return glm::scale(mat4x4(1.0f), m_scale); });
	}
	mat4x4 inverseScaleMatrix() const noexcept {
		return TypeMatrixBuffer::backwardBufferOrMatrix(
        	[this]() { return glm::scale(mat4x4(1.0f), inverseScaleVector()); });
	}

public:
	Scale3D() noexcept : m_scale(1.0f) { }
	Scale3D(float x, float y, float z) noexcept : m_scale(x, y, z) { }
	Scale3D(vec3 scale) noexcept : m_scale(scale) { }
	virtual ~Scale3D() = default;

	virtual mat4x4 matrix() const noexcept override { return scaleMatrix(); }
	virtual mat4x4 inverse() const noexcept override { return inverseScaleMatrix(); }
	
	virtual void passthrough(mat4x4 &mat) const noexcept override {
		mat = glm::scale(mat, m_scale);
	}
	virtual void passthrough(vec4 &vec) const noexcept override {
		vec.x *= m_scale.x;
		vec.y *= m_scale.y;
		vec.z *= m_scale.z;
	}

	virtual void passthroughInverse(mat4x4 &mat) const noexcept override {
		mat = glm::scale(mat, inverseScaleVector());
	}
	virtual void passthroughInverse(vec4 &vec) const noexcept override {
		vec.x /= m_scale.x;
		vec.y /= m_scale.y;
		vec.z /= m_scale.z;
	}
};

template<
	typename ForwardBuffer = CalculateMatrix,
	typename BackwardBuffer = CalculateMatrix>
class Scale2D :
	public ViewTransformer,
	protected MatrixBuffer<ForwardBuffer, BackwardBuffer> {
public:
	using TypeMatrixBuffer = MatrixBuffer<CalculateMatrix, BackwardBuffer>;

protected:
	vec2 m_scale;

	vec2 inverseScaleVector() const noexcept {
		return { 1.0f / m_scale.x, 1.0f / m_scale.y };
	}

	mat4x4 scaleMatrix() const noexcept {
		return TypeMatrixBuffer::forwardBufferOrMatrix(
        	[this]() { return glm::scale(mat4x4(1.0f), vec3(m_scale, 1.0f)); });
	}
	mat4x4 inverseScaleMatrix() const noexcept {
		return TypeMatrixBuffer::backwardBufferOrMatrix(
        	[this]() { return glm::scale(mat4x4(1.0f), vec3(inverseScaleVector(), 1.0f)); });
	}

public:
	Scale2D() noexcept : m_scale(1.0f) { }
	Scale2D(float x, float y) noexcept : m_scale(x, y) { }
	Scale2D(vec2 scale) noexcept : m_scale(scale) { }
	virtual ~Scale2D() = default;

	virtual mat4x4 matrix() const noexcept override { return scaleMatrix(); }
	virtual mat4x4 inverse() const noexcept override { return inverseScaleMatrix(); }
	
	virtual void passthrough(mat4x4 &mat) const noexcept override {
		mat = glm::scale(mat, vec3(m_scale, 1.0f));
	}
	virtual void passthrough(vec4 &vec) const noexcept override {
		vec.x *= m_scale.x;
		vec.y *= m_scale.y;
	}
	
	virtual void passthroughInverse(mat4x4 &mat) const noexcept override {
		mat = glm::scale(mat, vec3(inverseScaleVector(), 1.0f));
	}
	virtual void passthroughInverse(vec4 &vec) const noexcept override {
		vec.x /= m_scale.x;
		vec.y /= m_scale.y;
	}
};


/// <summary>
/// A three dimensional camera that can be used to emulate a view position, angle,
/// fov, aspect ratio as well as the near and far plane. The camera settings can
/// be exported as a 4x4 matrix applying the defined transformation. The view and
/// projection matrices are not buffered and recalculated for every call of
/// projectionMatrix() or viewMatrix(). See MatrixBufferedCamera3D for an
/// implementation that caches the matrices.
/// </summary>
template<
	typename ForwardBuffer = BufferMatrix,
	typename BackwardBuffer = BufferMatrix>
class Camera3D :
	public ViewPipeline,
	protected MatrixBuffer<ForwardBuffer, BackwardBuffer> {
public:
	using TypeTranslation3D = Translation3D<CalculateMatrix, CalculateMatrix>;
	using TypeProjection3D = Projection3D<BufferMatrix, BufferMatrix>;
	using TypeRotation3D = Rotation3D<CalculateMatrix, CalculateMatrix>;

protected:
	TypeProjection3D m_projection;
	TypeTranslation3D m_translation;
	TypeRotation3D m_rotation;

	template<typename Type>
	void passthroughCamera3D(Type &val) const noexcept {
    	m_translation.passthrough(val);
    	m_rotation.passthrough(val);
		m_projection.passthrough(val); // TODO check
	}
	template<typename Type>
	void passthroughInverseCamera3D(Type &val) const noexcept {
    	m_projection.passthroughInverse(val);
    	m_translation.passthroughInverse(val);
		m_rotation.passthroughInverse(val); // TODO check
	}

public:
	Camera3D() noexcept = default;
	Camera3D(
		const Projection3DSettings &projectionSettings,
		const Translation3DSettings &translationSettings,
		const RotationSettings3DEuler &rotationSettings) noexcept :
		m_projection(projectionSettings),
		m_translation(translationSettings),
		m_rotation(rotationSettings) { }
	Camera3D(
		const Projection3DSettings &projectionSettings,
		const Translation3DSettings &translationSettings,
		const RotationSettings3DQuat &rotationSettings) noexcept :
		m_projection(projectionSettings),
		m_translation(translationSettings),
		m_rotation(rotationSettings) { }

	virtual ~Camera3D() = default;

	// ---- Complex Functions ---- //
	void lookAt(const vec3 &pos, const vec3 &dest, const vec3 &up) noexcept;

	// ---- Const access modifiers ---- //
	inline const TypeProjection3D& projection() const noexcept { return m_projection; }
	inline const TypeTranslation3D& translation() const noexcept { return m_translation; }
	inline const TypeRotation3D& rotation() const noexcept { return m_rotation; }
	// ---- access modifiers ---- //
	inline TypeProjection3D& projection() noexcept { return m_projection; }
	inline TypeTranslation3D& translation() noexcept { return m_translation; }
	inline TypeRotation3D& rotation() noexcept { return m_rotation; }

	virtual mat4x4 projectionMatrix() const noexcept override {
		return m_projection.matrix();
	}
	virtual mat4x4 viewMatrix() const noexcept override {
		mat4x4 mat(1.0f);
		m_translation.passthrough(mat); // TODO check
		m_rotation.passthrough(mat);
		return mat;
	}

	virtual mat4x4 matrix() const noexcept override {
		mat4x4 mat(1.0f);
		passthroughCamera3D(mat);
		return mat;
	}
	virtual mat4x4 inverse() const noexcept override {
		mat4x4 mat(1.0f);
		passthroughInverseCamera3D(mat);
		return mat;
	}
	virtual void passthrough(mat4x4 &mat) const noexcept override {
		passthrough(mat);
	}
	virtual void passthrough(vec4 &vec) const noexcept override {
		passthrough(vec);
	}
	virtual void passthroughInverse(mat4x4 &mat) const noexcept override {
		passthrough(mat);
	}
	virtual void passthroughInverse(vec4 &vec) const noexcept override {
		passthrough(vec);
	}
};

//// ---- Camera2D ---- ////
template<
	typename ForwardBuffer = BufferMatrix,
	typename BackwardBuffer = BufferMatrix>
class Camera2D :
	public ViewPipeline,
	protected MatrixBuffer<ForwardBuffer, BackwardBuffer> {
public:
	using TypeMatrixBuffer = MatrixBuffer<ForwardBuffer, BackwardBuffer>;

	using TypeRotation2D = Rotation2D<CalculateMatrix, CalculateMatrix>;
	using TypeTranslation2D = Translation2D<CalculateMatrix, CalculateMatrix>;
	using TypeScale2D = Scale2D<CalculateMatrix, CalculateMatrix>;

protected:
	TypeRotation2D m_rotation;
	TypeTranslation2D m_translation;
	TypeScale2D m_scale;
	
	template<typename Type>
	void passthroughCamera2D(Type &val) const noexcept {
		m_scale.passthrough(val); // TODO check
    	m_rotation.passthrough(val);
    	m_translation.passthrough(val);
	}
	template<typename Type>
	void passthroughInverseCamera2D(Type &val) const noexcept {
		m_translation.passthroughInverse(val); // TODO check
    	m_rotation.passthroughInverse(val);
    	m_scale.passthroughInverse(val);
	}

public:
	Camera2D() noexcept { }
	Camera2D(
		const TypeTranslation2D translation,
		const TypeRotation2D rotation,
		const TypeScale2D scale
	) noexcept :
		m_translation(translation),
		m_rotation(rotation), m_scale(scale) { }
	
	virtual ~Camera2D() = default;

	// ---- ViewPipeline ---- //
	virtual mat4x4 projectionMatrix() const noexcept override { return mat4x4(1.0f); }
	virtual mat4x4 viewMatrix() const noexcept override { return matrix(); }

	virtual mat4x4 matrix() const noexcept override {
		mat4x4 mat(1.0f);
		passthroughCamera2D(mat);
		return mat;
	}

	virtual mat4x4 inverse() const noexcept override {
		mat4x4 mat(1.0f);
		passthroughInverseCamera2D(mat);
		return mat;
	}

	virtual void passthrough(mat4x4 &mat) const noexcept {
		passthroughCamera2D(mat);
	}
	virtual void passthrough(vec4 &vec) const noexcept {
		passthroughCamera2D(vec);
	}
	virtual void passthroughInverse(mat4x4 &mat) const noexcept {
		passthroughInverseCamera2D(mat);
	}
	virtual void passthroughInverse(vec4 &vec) const noexcept {
		passthroughInverseCamera2D(vec);
	}
};

NYREM_NAMESPACE_END
