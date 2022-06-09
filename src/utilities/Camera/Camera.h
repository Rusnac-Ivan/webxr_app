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


	
}

#endif