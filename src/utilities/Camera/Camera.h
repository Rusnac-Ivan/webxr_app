#ifndef _UTIL_CAMERA_H_
#define _UTIL_CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace util
{
	class ICamera
	{
			glm::vec3 mPos;

			//Orientation by vectors
			glm::vec3 mUp;
			glm::vec3 mLook;

			//Orientation by quaternion or Euler angles
			glm::quat mOrientation;
			glm::vec3 mPitchYawRoll; // pitch -> OX, Yaw -> OY, Roll -> OZ

			glm::mat4 mViewMat;

		public:
			ICamera();
			virtual ~ICamera();

			void SetPosition(const glm::vec3& pos);
			void SetOrientation(const glm::vec3& up, const glm::vec3& look);
			void SetOrientation(const glm::quat& orientation);
			void SetOrientation(float pitch, float yaw, float roll);


			const glm::vec3& GetPosition() const { return mPos; }
			const glm::vec3& GetUp() const { return mUp; }
			const glm::vec3& GetLook() const { return mLook; }
			const glm::quat& GetOrientation() const { return mOrientation; }
			const glm::vec3& GetPitchYawRoll() const { return mPitchYawRoll; }

			virtual const glm::mat4& GetProjectionMat() = 0;
	};

	ICamera::ICamera() :
		mPos(0.f),
		mUp(0.f, 1.f, 0.f),
		mLook(0.f, 0.f, 1.f),
		mOrientation({}),
		mPitchYawRoll(0.f)
	{
		mViewMat = glm::lookAt(mPos, mPos + mLook, mUp);
	}
	ICamera::~ICamera(){}

	void ICamera::SetPosition(const glm::vec3& pos)
	{
		mPos = pos;
		mViewMat = glm::lookAt(mPos, mPos + mLook, mUp);
	}
	void ICamera::SetOrientation(const glm::vec3& up, const glm::vec3& look)
	{
		mUp = up;
		mLook = look;

		mViewMat = glm::lookAt(mPos, mPos + mLook, mUp);
	}
	void ICamera::SetOrientation(const glm::quat& orientation)
	{
		mOrientation = glm::normalize(orientation);

		//R^(-1) == transpose(R), if: R * R^(-1) == I
		const glm::mat4& inv_rot = glm::transpose(glm::toMat4(mOrientation));

		const glm::mat4& inv_tran = glm::translate(glm::mat4(1.f), -mPos);

		// VieMat = (T * R)^(-1) = R^(-1) * T^(-1)
		mViewMat = inv_rot * inv_tran;
	}

	void ICamera::SetOrientation(float pitch, float yaw, float roll)
	{
		//R^(-1) == transpose(R), if: R * R^(-1) == I
		const glm::mat4& inv_rot = glm::transpose(glm::eulerAngleYXZ(yaw, pitch, roll));
		const glm::mat4& test = glm::transpose(glm::yawPitchRoll(yaw, pitch, roll));

		assert(inv_rot == test);

		const glm::mat4& inv_tran = glm::translate(glm::mat4(1.f), -mPos);

		// VieMat = (T * R)^(-1) = R^(-1) * T^(-1)
		mViewMat = inv_rot * inv_tran;
	}


	enum struct ProjectionType
	{
		ORTHOGRAPHIC,
		PERSPECTIVE
	};

	template<ProjectionType Type>
	class Camera : public ICamera {};


	template<>
	class Camera<ProjectionType::PERSPECTIVE>
	{
		float mNear;
		float mFar;
		float mAspectRatio;
		float mFOV;

		glm::mat4 mProjectionMat;
	public:
		Camera();
		~Camera(){}

		void SetState(float near, float far, float aspectRatio, float fov);

		void SetNear(float near);
		void SetFar(float far);
		void SetAspectRatio(float aspectRatio);
		void SetFOV(float fov);

		float GetNear() const { return mNear; }
		float GetFar() const { return mFar; }
		float GetAspectRatio() const { return mAspectRatio; }
		float GetFOV() const { return mFOV; }

		virtual const glm::mat4& GetProjectionMat() { return mProjectionMat; }

	};


	Camera<ProjectionType::PERSPECTIVE>::Camera() :
		mNear(0.01f),
		mFar(100.f),
		mAspectRatio(16.f/9.f),
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


	template<>
	class Camera<ProjectionType::ORTHOGRAPHIC>
	{
		float mNear;
		float mFar;
		float mTop;
		float mBottom;
		float mLeft;
		float mRight;

		glm::mat4 mProjectionMat;
	public:
		Camera();
		~Camera(){}

		void SetState(float near, float far, float top, float bottom, float left, float right);

		void SetNear(float near);
		void SetFar(float far);
		void SetTop(float top);
		void SetBottom(float bottom);
		void SetLeft(float left);
		void SetRight(float right);

		float GetNear() const { return mNear; }
		float GetFar() const { return mFar; }
		float GetTop() const { return mTop; }
		float GetBottom() const { return mBottom; }
		float GetLeft() const { return mLeft; }
		float GetRight() const { return mRight; }

		virtual const glm::mat4& GetProjectionMat() { return mProjectionMat; }

	};


	Camera<ProjectionType::ORTHOGRAPHIC>::Camera() :
		mNear(0.01f),
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

#endif