/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_BASE_RENDER_RS_EFFECT_LUMINANCE_MANAGER_H
#define RENDER_SERVICE_BASE_RENDER_RS_EFFECT_LUMINANCE_MANAGER_H

#include <map>

#include "common/rs_common_def.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "draw/color.h"

namespace OHOS {
namespace Rosen {

constexpr float EFFECT_MAX_LUMINANCE = 2.0f;

class RSB_EXPORT RSEffectLuminanceManager {
public:
    virtual ~RSEffectLuminanceManager() = default;

    static RSEffectLuminanceManager& GetInstance();

    void SetDisplayHeadroom(NodeId id, float brightness);

    float GetDisplayHeadroom(NodeId id) const;

    static Vector4f GetBrightnessMapping(float brightness, Vector4f& input);

    static Drawing::Color4f GetBrightnessMapping(float brightness, Drawing::Color4f& input);

    static float GetBrightnessMapping(float brightness, float input);

private:
    static bool CalcBezierResultY(
        const Vector2f& start, const Vector2f& end, const Vector2f& control, float x, float& y);

    std::map<uint64_t, float> displayHeadroomMap_;
};
} // Rosen
} // OHOS
#endif // RENDER_SERVICE_BASE_RENDER_RS_EFFECT_LUMINANCE_MANAGER_H
