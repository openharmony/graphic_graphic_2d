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
#include "render/rs_effect_luminance_manager.h"

#include <algorithm>
#include <mutex>
#include <vector>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

namespace {
const std::vector<Vector2f> g_hdrPathPoint = {
    {0.0f, 0.0f},
    {0.75f, 0.75f},
    {1.0f, 0.95f},
    {1.25f, 0.97f},
    {1.5f, 0.98f},
    {1.75f, 0.9875f},
    {1.90f, 0.995f},
    {2.0f, 1.0f},
};

const std::vector<Vector2f> g_hdrControlPoint = {
    {0.375f, 0.375f},
    {0.875f, 0.875f},
    {1.125f, 0.9625f},
    {1.375f, 0.9725f},
    {1.625f, 0.98f},
    {1.825f, 0.99f},
    {1.95f, 0.998f},  // 1 less than path point
};

std::mutex g_dataMutex;
}

RSEffectLuminanceManager& RSEffectLuminanceManager::GetInstance()
{
    static RSEffectLuminanceManager instance;
    return instance;
}

void RSEffectLuminanceManager::SetDisplayHeadroom(NodeId id, float headroom)
{
    std::lock_guard<std::mutex> lock(g_dataMutex);
    displayHeadroomMap_[static_cast<uint64_t>(id)] = headroom;
}

float RSEffectLuminanceManager::GetDisplayHeadroom(NodeId id) const
{
    std::lock_guard<std::mutex> lock(g_dataMutex);
    auto iter = displayHeadroomMap_.find(static_cast<uint64_t>(id));
    if (iter != displayHeadroomMap_.end()) {
        ROSEN_LOGW("RSEffectLuminanceManager::GetDisplayHeadroom headroom is %{public}f", iter->second);
        return iter->second;
    }
    return 1.0f;
}

Vector4f RSEffectLuminanceManager::GetBrightnessMapping(float brightness, Vector4f& input)
{
    float highValue = std::max({input.x_, input.y_, input.z_, 1.0f});
    float compressRatio = GetBrightnessMapping(brightness, highValue) / highValue;
    Vector4f output;
    output.x_ = input.x_ * compressRatio;
    output.y_ = input.y_ * compressRatio;
    output.z_ = input.z_ * compressRatio;
    output.w_ = input.w_;
    return output;
}

Drawing::Color4f RSEffectLuminanceManager::GetBrightnessMapping(float brightness, Drawing::Color4f& input)
{
    float highValue = std::max({input.redF_, input.greenF_, input.blueF_, 1.0f});
    float compressRatio = GetBrightnessMapping(brightness, highValue) / highValue;
    Drawing::Color4f output;
    output.redF_ = input.redF_ * compressRatio;
    output.greenF_ = input.greenF_ * compressRatio;
    output.blueF_ = input.blueF_ * compressRatio;
    output.alphaF_ = input.alphaF_;
    return output;
}

float RSEffectLuminanceManager::GetBrightnessMapping(float brightness, float input)
{
    if (ROSEN_GE(brightness, EFFECT_MAX_LUMINANCE)) {
        return input;
    }
    int rangeIndex = -1;
    for (auto point : g_hdrPathPoint) {
        if (input <= point.x_) {
            break;
        }
        rangeIndex++;
    }
    if (rangeIndex == -1) {
        return 0.0f;
    }
    if ((rangeIndex >= static_cast<int>(g_hdrPathPoint.size()) - 1) ||
        (rangeIndex >= static_cast<int>(g_hdrControlPoint.size()))) {
        return brightness;
    }

    // calculate new hdr bightness via bezier curve
    Vector2f start = g_hdrPathPoint[rangeIndex];
    Vector2f end = g_hdrPathPoint[rangeIndex + 1];
    Vector2f control = g_hdrControlPoint[rangeIndex];

    float y = 0.0f;
    if (CalcBezierResultY(start, end, control, input, y)) {
        y = ((EFFECT_MAX_LUMINANCE - brightness) * y + (brightness - 1.0) * input) /
            (EFFECT_MAX_LUMINANCE - 1.0f);  // linear interpolation
        ROSEN_LOGD("RSEffectLuminanceManager::GetBrightnessMapping y is %{public}f", y);
        return std::clamp(y, 0.0f, brightness);
    }
    return std::clamp(input, 0.0f, brightness);
}

bool RSEffectLuminanceManager::CalcBezierResultY(
    const Vector2f& start, const Vector2f& end, const Vector2f& control, float x, float& y)
{
    const float a = start[0] - 2 * control[0] + end[0];
    const float b = 2 * (control[0] - start[0]);
    const float c = start[0] - x;
    const float discriminant = b * b - 4 * a * c;

    float t = 0.0f;
    if (ROSEN_LNE(discriminant, 0.0f)) {
        return false;
    } else if (ROSEN_EQ(a, 0.0f)) {
        t = -c / b;
    } else {
        const float sqrtD = std::sqrt(discriminant);
        const float t1 = (-b + sqrtD) / (2.0 * a);
        const float t2 = (-b - sqrtD) / (2.0 * a);
        if (ROSEN_GE(t1, 0.0f) && ROSEN_LE(t1, 1.0f)) {
            t = t1;
        } else if (ROSEN_GE(t2, 0.0f) && ROSEN_LE(t2, 1.0f)) {
            t = t2;
        }
    }

    y = start[1] + t * (2.0f * (control[1] - start[1]) + t * (start[1] - 2.0f * control[1] + end[1]));
    return true;
}

}  // Rosen
} // OHOS