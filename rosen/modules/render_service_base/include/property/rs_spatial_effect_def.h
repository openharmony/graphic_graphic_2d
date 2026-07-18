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

#include <variant>

#include "common/rs_vector2.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {

enum class DepthSpaceType : int16_t {
    INSTANCE = 0,
    GLOBAL = 1
};

enum class SpatialEffectMode : int16_t {
    WORLD_XYZ_MODE = 0,
    NDC_XY_WORLD_Z_MODE = 1
};

struct DepthCameraPara {
    Vector3f position;
    Vector4f quaternion;
    float yFov = 0.f;
    float zNear = 0.1f;
    float zFar = 100.0f;
    Vector2f offset;

    bool operator==(const DepthCameraPara& other) const
    {
        return position == other.position && quaternion == other.quaternion && ROSEN_EQ(yFov, other.yFov) &&
            ROSEN_EQ(zNear, other.zNear) && ROSEN_EQ(zFar, other.zFar) && offset == other.offset;
    }
};

struct DepthLightPara {
    Vector3f direction;
    Vector3f color;
    float intensity = 0.f;

    bool operator==(const DepthLightPara& other) const
    {
        return direction == other.direction && color == other.color && ROSEN_EQ(intensity, other.intensity);
    }
};

struct DepthEffectPara {
    float depth = 0.0f;
    float occlusionWeight = 0.0f;

    bool operator==(const DepthEffectPara& other) const
    {
        return ROSEN_EQ(depth, other.depth) && ROSEN_EQ(occlusionWeight, other.occlusionWeight);
    }
};

struct SpatialEffectPara {
    static constexpr uint8_t LEFT_TOP_INDEX = 0;
    static constexpr uint8_t RIGHT_TOP_INDEX = 1;
    static constexpr uint8_t LEFT_BOTTOM_INDEX = 2;
    static constexpr uint8_t RIGHT_BOTTOM_INDEX = 3;
    static constexpr uint8_t CORNER_NUMBER = 4;

    using CornerPositions = std::array<Vector3f, CORNER_NUMBER>;

    union {
        struct {
            Vector3f leftTop;
            Vector3f rightTop;
            Vector3f leftBottom;
            Vector3f rightBottom;
        };
        CornerPositions corners;
    };

    float occlusionWeight = 0.0f;
    CornerPositions xyzCorners;
    SpatialEffectMode spatialEffectMode = SpatialEffectMode::WORLD_XYZ_MODE;

    SpatialEffectPara() {};
    ~SpatialEffectPara() {};

    SpatialEffectPara& operator=(const SpatialEffectPara& other)
    {
        leftTop = other.leftTop;
        rightTop = other.rightTop;
        leftBottom = other.leftBottom;
        rightBottom = other.rightBottom;
        occlusionWeight = other.occlusionWeight;
        spatialEffectMode = other.spatialEffectMode;
        xyzCorners = other.xyzCorners;
        return *this;
    }

    bool operator==(const SpatialEffectPara& other) const
    {
        return leftTop == other.leftTop && rightTop == other.rightTop &&
            leftBottom == other.leftBottom && rightBottom == other.rightBottom &&
            ROSEN_EQ(occlusionWeight, other.occlusionWeight) &&
            spatialEffectMode == other.spatialEffectMode;
    }
};

struct SpatialEffectVariantPara {
    std::variant<float, SpatialEffectPara::CornerPositions> position;
    float occlusionWeight = 0.0f;
    SpatialEffectMode spatialEffectMode = SpatialEffectMode::WORLD_XYZ_MODE;
    SpatialEffectPara::CornerPositions xyzCornerPoints = {};

    SpatialEffectVariantPara() = default;

    SpatialEffectVariantPara(const DepthEffectPara& depthEffectPara)
    {
        position = depthEffectPara.depth;
        occlusionWeight = depthEffectPara.occlusionWeight;
    }

    SpatialEffectVariantPara(const SpatialEffectPara& spatialEffectPara)
    {
        position = spatialEffectPara.corners;
        occlusionWeight = spatialEffectPara.occlusionWeight;
    }

    bool operator==(const SpatialEffectVariantPara& other) const
    {
        if (std::holds_alternative<float>(position) && std::holds_alternative<float>(other.position)) {
            return ROSEN_EQ(position, other.position) && ROSEN_EQ(occlusionWeight, other.occlusionWeight)
                && ROSEN_EQ(spatialEffectMode, other.spatialEffectMode);
        }

        if (std::holds_alternative<SpatialEffectPara::CornerPositions>(position) &&
            std::holds_alternative<SpatialEffectPara::CornerPositions>(other.position)) {
            const auto& corners = std::get<SpatialEffectPara::CornerPositions>(position);
            const auto& otherCorners = std::get<SpatialEffectPara::CornerPositions>(other.position);
            for (uint32_t i = 0; i < SpatialEffectPara::CORNER_NUMBER; i++) {
                if (!ROSEN_EQ(corners[i], otherCorners[i])) {
                    return false;
                }
            }
            return ROSEN_EQ(occlusionWeight, other.occlusionWeight);
        }

        return false;
    }

    bool PerspectiveEnabled() const
    {
        return std::holds_alternative<SpatialEffectPara::CornerPositions>(position);
    }
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PROPERTY_RS_SPATIAL_EFFECT_DEF_H
