#ifndef _UTIL_AABB_H_ //"axis-aligned bounding box"
#define _UTIL_AABB_H_

#include <glm/glm.hpp>

namespace util
{
    class AABB
    {
        private:
            glm::vec3 mMin;
            glm::vec3 mMax;

            bool mIsValid;
        public:
            AABB() : mIsValid(false), mMin(FLT_MAX), mMax(-FLT_MAX) {}
            AABB(glm::vec3 min, glm::vec3 max) : mIsValid(true), mMin(min), mMax(max) {}
            ~AABB() {}

            bool IsValid() { return mIsValid; }

            glm::vec3 GetMin() const { return mMin; }
            glm::vec3 GetMax() const { return mMax; }

            void Align(glm::vec3 min, glm::vec3 max)
            {
                if (!mIsValid)
                {
                    mMin = min;
                    mMax = max;
                    mIsValid = true;
                }
                else
                {
                    mMin = glm::min(mMin, min);
                    mMax = glm::max(mMax, max);
                }

            }

            void Reset()
            {
                mMin = mMax = glm::vec3(0.f);
                mIsValid = false;
            }

            constexpr glm::vec3 Length() const
            {
                return mMax - mMin;
            }

            constexpr glm::vec3 GetCenter() const
            {
                return (mMax + mMin) * 0.5f;
            }

            void SetState(glm::mat4 m)
            {
                glm::vec3 min = glm::vec3(m[3]);
                glm::vec3 max = min;
                glm::vec3 v0, v1;

                glm::vec3 right = glm::vec3(m[0]);
                v0 = right * this->mMin.x;
                v1 = right * this->mMax.x;
                min += glm::min(v0, v1);
                max += glm::max(v0, v1);

                glm::vec3 up = glm::vec3(m[1]);
                v0 = up * this->mMin.y;
                v1 = up * this->mMax.y;
                min += glm::min(v0, v1);
                max += glm::max(v0, v1);

                glm::vec3 back = glm::vec3(m[2]);
                v0 = back * this->mMin.z;
                v1 = back * this->mMax.z;
                min += glm::min(v0, v1);
                max += glm::max(v0, v1);

                mMin = min;
                mMax = max;
            }
    };
}


#endif // !_AABB_H_ //"axis-aligned bounding box"
