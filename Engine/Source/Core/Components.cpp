#include "Core/Components.h"

namespace FS::Component
{
    glm::vec3 Transform::GetWorldRotation()
    {
        glm::vec3 worldRotation{
            glm::degrees(atan2f(mWorldMatrix[1][2], mWorldMatrix[2][2])),
            glm::degrees(glm::atan(-mWorldMatrix[0][2], 
            sqrtf(mWorldMatrix[1][2] * mWorldMatrix[1][2] + mWorldMatrix[2][2] * mWorldMatrix[2][2]))),
            glm::degrees(atan2f(mWorldMatrix[0][1], mWorldMatrix[0][0]))
        };
        return worldRotation;
    };

    glm::vec3 Transform::GetWorldScale()
    {
        glm::vec3 scale
        {
            glm::length(glm::vec3(mWorldMatrix[0])),
            glm::length(glm::vec3(mWorldMatrix[1])),
            glm::length(glm::vec3(mWorldMatrix[2]))
        };
        return scale;
    };
}