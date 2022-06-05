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
            AABB() : mIsValid(false), mMin(0.f), mMax(0.f) {}
            ~AABB() {}

            glm::vec3 GetMin() { return mMin; }
            glm::vec3 GetMax() { return mMax; }

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
    };
}


#endif // !_AABB_H_ //"axis-aligned bounding box"
