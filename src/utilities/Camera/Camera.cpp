#include "Camera.h"

namespace util
{
	ICamera::ICamera() : mPos(0.f),
						 mUp(0.f, 1.f, 0.f),
						 mLook(0.f, 0.f, 1.f),
						 mOrientation(),
						 mPitchYawRoll(0.f)
	{
		mViewMat = glm::lookAt(mPos, mPos + mLook, mUp);
	}
	ICamera::~ICamera() {}

	void ICamera::SetPosition(const glm::vec3 &pos)
	{
		mPos = pos;
		mViewMat = glm::lookAt(mPos, mPos + mLook, mUp);
	}
	void ICamera::SetOrientation(const glm::vec3 &up, const glm::vec3 &look)
	{
		mUp = up;
		mLook = look;

		mViewMat = glm::lookAt(mPos, mPos + mLook, mUp);
	}
	void ICamera::SetOrientation(const glm::quat &orientation)
	{
		mOrientation = glm::normalize(orientation);

		// R^(-1) == transpose(R), if: R * R^(-1) == I
		const glm::mat4 inv_rot = glm::transpose(glm::toMat4(mOrientation));

		const glm::mat4 inv_tran = glm::translate(glm::mat4(1.f), -mPos);

		// VieMat = (T * R)^(-1) = R^(-1) * T^(-1)
		mViewMat = inv_rot * inv_tran;
	}

	void ICamera::SetOrientation(float pitch, float yaw, float roll)
	{
		// R^(-1) == transpose(R), if: R * R^(-1) == I
		const glm::mat4 inv_rot = glm::transpose(glm::eulerAngleYXZ(yaw, pitch, roll));
		const glm::mat4 test = glm::transpose(glm::yawPitchRoll(yaw, pitch, roll));

		assert(inv_rot == test);

		const glm::mat4 inv_tran = glm::translate(glm::mat4(1.f), -mPos);

		// VieMat = (T * R)^(-1) = R^(-1) * T^(-1)
		mViewMat = inv_rot * inv_tran;
	}

	Camera<ProjectionType::PERSPECTIVE>::Camera() : mNear(0.01f),
													mFar(100.f),
													mAspectRatio(16.f / 9.f),
													mFOV(60.f)
	{
		mProjectionMat = glm::perspective(glm::radians(mFOV), mAspectRatio, mNear, mFar);
	}

	void Camera<ProjectionType::PERSPECTIVE>::SetState(float near, float far, float aspectRatio, float fov)
	{
		mNear = near;
		mFar = far;
		mAspectRatio = aspectRatio;
		mFOV = fov;

		mProjectionMat = glm::perspective(glm::radians(mFOV), mAspectRatio, mNear, mFar);
	}

	void Camera<ProjectionType::PERSPECTIVE>::SetNear(float near)
	{
		mNear = near;
		mProjectionMat = glm::perspective(glm::radians(mFOV), mAspectRatio, mNear, mFar);
	}
	void Camera<ProjectionType::PERSPECTIVE>::SetFar(float far)
	{
		mFar = far;
		mProjectionMat = glm::perspective(glm::radians(mFOV), mAspectRatio, mNear, mFar);
	}
	void Camera<ProjectionType::PERSPECTIVE>::SetAspectRatio(float aspectRatio)
	{
		mAspectRatio = aspectRatio;
		mProjectionMat = glm::perspective(glm::radians(mFOV), mAspectRatio, mNear, mFar);
	}
	void Camera<ProjectionType::PERSPECTIVE>::SetFOV(float fov)
	{
		mFOV = fov;
		mProjectionMat = glm::perspective(glm::radians(mFOV), mAspectRatio, mNear, mFar);
	}

	Camera<ProjectionType::ORTHOGRAPHIC>::Camera() : mNear(0.01f),
													 mFar(100.f),
													 mTop(10.f),
													 mBottom(-10.f),
													 mLeft(-10.f),
													 mRight(10.f)
	{
		mProjectionMat = glm::ortho(mLeft, mRight, mBottom, mTop, mNear, mFar);
	}

	void Camera<ProjectionType::ORTHOGRAPHIC>::SetState(float near, float far, float top, float bottom, float left, float right)
	{
		mNear = near;
		mFar = far;
		mTop = top;
		mBottom = bottom;
		mLeft = left;
		mRight = right;

		mProjectionMat = glm::ortho(mLeft, mRight, mBottom, mTop, mNear, mFar);
	}

	void Camera<ProjectionType::ORTHOGRAPHIC>::SetNear(float near)
	{
		mNear = near;
		mProjectionMat = glm::ortho(mLeft, mRight, mBottom, mTop, mNear, mFar);
	}
	void Camera<ProjectionType::ORTHOGRAPHIC>::SetFar(float far)
	{
		mFar = far;
		mProjectionMat = glm::ortho(mLeft, mRight, mBottom, mTop, mNear, mFar);
	}
	void Camera<ProjectionType::ORTHOGRAPHIC>::SetTop(float top)
	{
		mTop = top;
		mProjectionMat = glm::ortho(mLeft, mRight, mBottom, mTop, mNear, mFar);
	}
	void Camera<ProjectionType::ORTHOGRAPHIC>::SetBottom(float bottom)
	{
		mBottom = bottom;
		mProjectionMat = glm::ortho(mLeft, mRight, mBottom, mTop, mNear, mFar);
	}
	void Camera<ProjectionType::ORTHOGRAPHIC>::SetLeft(float left)
	{
		mLeft = left;
		mProjectionMat = glm::ortho(mLeft, mRight, mBottom, mTop, mNear, mFar);
	}
	void Camera<ProjectionType::ORTHOGRAPHIC>::SetRight(float right)
	{
		mRight = right;
		mProjectionMat = glm::ortho(mLeft, mRight, mBottom, mTop, mNear, mFar);
	}
}