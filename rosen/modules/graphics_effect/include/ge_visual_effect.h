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
#ifndef GRAPHICS_EFFECT_GE_VISUAL_EFFECT_H
#define GRAPHICS_EFFECT_GE_VISUAL_EFFECT_H

#include <memory>

#include "effect/color_filter.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

enum class DrawingPaintType { NONE, BRUSH, PEN, PAINT, BRUSH_PEN };

struct GEAIBarShaderFilterParams {
    float aiBarLow;
    float aiBarHigh;
    float aiBarThreshold;
    float aiBarOpacity;
    float aiBarSaturation;
};

struct GEGreyShaderFilterParams {
    float greyCoef1;
    float greyCoef2;
};

struct GEKawaseBlurShaderFilterParams {
    int radius;
};

struct GELinearGradientBlurShaderFilterParams {
    float blurRadius;
    std::vector<std::pair<float, float>> fractionStops;
    int direction;
    float geoWidth;
    float geoHeight;
    Drawing::Matrix mat;
    float tranX;
    float tranY;
    bool isOffscreenCanvas;
};

class GEVisualEffectImpl;

class GEVisualEffect {
public:
    GEVisualEffect(const std::string& name, DrawingPaintType type = DrawingPaintType::BRUSH);
    ~GEVisualEffect();

    void SetParam(const std::string& tag, int32_t param);
    void SetParam(const std::string& tag, int64_t param);
    void SetParam(const std::string& tag, float param);
    void SetParam(const std::string& tag, double param);
    void SetParam(const std::string& tag, const char* const param);

    void SetParam(const std::string& tag, const std::shared_ptr<Drawing::Image> param) {}
    void SetParam(const std::string& tag, const std::shared_ptr<Drawing::ColorFilter> param) {}
    void SetParam(const std::string& tag, const Drawing::Matrix param);
    void SetParam(const std::string& tag, const std::vector<std::pair<float, float>>);
    void SetParam(const std::string& tag, bool param);

    const std::string& GetName() const
    {
        return visualEffectName_;
    }

    const std::shared_ptr<GEVisualEffectImpl> GetImpl() const
    {
        return visualEffectImpl_;
    }

private:
    std::string visualEffectName_;
    DrawingPaintType type_;
    std::shared_ptr<GEVisualEffectImpl> visualEffectImpl_;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_VISUAL_EFFECT_H
