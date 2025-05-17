/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef UIEFFECT_UTILS_H
#define UIEFFECT_UTILS_H

#include <math.h>
#include <utility>
#include "common/rs_common_def.h"
#include "common/rs_vector2.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {
namespace UIEffect {
inline bool IsParaSameSign(Vector4f para)
{
    return (ROSEN_GE(para.x_, 0.f) && ROSEN_GE(para.y_, 0.f) && ROSEN_GE(para.z_, 0.f) && ROSEN_GE(para.w_, 0.f)) ||
        (ROSEN_LE(para.x_, 0.f) && ROSEN_LE(para.y_, 0.f) && ROSEN_LE(para.z_, 0.f) && ROSEN_LE(para.w_, 0.f));
}

inline float GetLimitedPara(float para, std::pair<float, float> limits = {-INFINITY, INFINITY})
{
    return std::clamp(para, limits.first, limits.second);
}

inline Vector2f GetLimitedPara(const Vector2f& para, std::pair<float, float> limits = {-INFINITY, INFINITY})
{
    return Vector2f(std::clamp(para.x_, limits.first, limits.second), std::clamp(para.y_, limits.first, limits.second));
}

inline Vector3f GetLimitedPara(const Vector3f& para, std::pair<float, float> limits = {-INFINITY, INFINITY})
{
    return Vector3f(std::clamp(para.x_, limits.first, limits.second), std::clamp(para.y_, limits.first, limits.second),
        std::clamp(para.z_, limits.first, limits.second));
}

inline Vector4f GetLimitedPara(const Vector4f& para, std::pair<float, float> limits = {-INFINITY, INFINITY})
{
    return Vector4f(std::clamp(para.x_, limits.first, limits.second), std::clamp(para.y_, limits.first, limits.second),
        std::clamp(para.z_, limits.first, limits.second), std::clamp(para.w_, limits.first, limits.second));
}
} // namespace UIEffect
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_UTILS_H
