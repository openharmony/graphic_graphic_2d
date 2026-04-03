/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RENDER_SERVICE_BASE_PROPERTY_RS_SPATIAL_EFFECT_DEF_H
#define RENDER_SERVICE_BASE_PROPERTY_RS_SPATIAL_EFFECT_DEF_H

#include "common/rs_vector3.h"
#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {

enum class DepthSpaceType : int16_t {
    INSTANCE = 0;
    GLOBAL = 1
};

struct DepthCameraPara {
    Vector3f position;
    Vector4f quaternion;
    float yFov = 0.f;
    float zNear = 0.1f;
    float zFar = 100.f;

    bool operator==(const DepthCameraPara& other) const noexcept
    {
        return position == other.position && quaternion == other.quaternion && ROSEN_EQ(yFov, other.yFov)
            && ROSEN_EQ(zNear, other.zNear) && ROSEN_EQ(zFar, other.zFar);
    }
};

struct DepthLightPara {
    Vector3f direction;
    Vector3f color;
    float intensity = 1.f;

    bool operator==(const DepthLightPara& other) const noexcept
    {
        return direction == other.direction && color == other.color && ROSEN_EQ(intensity, other.intensity);
    }
};

struct SpatialEffectPara {
    Vector3f leftTop;
    Vector3f rightTop;
    Vector3f leftBottom;
    Vector3f rightBottom;
    float occlusionWeight = 0.f;

    bool operator==(const SpatialEffectPara& other) const noexcept
    {
        return leftTop == other.leftTop && rightTop == other.rightTop &&
            leftBottom == other.leftBottom && rightBottom == other.rightBottom &&
            ROSEN_EQ(occlusionWeight, other.occlusionWeight);
    }
}; 
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PROPERTY_RS_SPATIAL_EFFECT_DEF_H