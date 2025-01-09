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
#include <map>

#include "ge_visual_effect_impl.h"
#include "ge_log.h"
#include "ge_external_dynamic_loader.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

std::map<const std::string, std::function<void(GEVisualEffectImpl*)>> GEVisualEffectImpl::g_initialMap = {
    { GE_FILTER_KAWASE_BLUR,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::KAWASE_BLUR);
            impl->MakeKawaseParams();
        }
    },
    { GE_FILTER_MESA_BLUR,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::MESA_BLUR);
            impl->MakeMESAParams();
        }
    },
    { GE_FILTER_GREY,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::GREY);
            impl->MakeGreyParams();
        }
    },
    { GE_FILTER_AI_BAR,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::AIBAR);
            impl->MakeAIBarParams();
        }
    },
    { GE_FILTER_LINEAR_GRADIENT_BLUR,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::LINEAR_GRADIENT_BLUR);
            impl->MakeLinearGradientBlurParams();
        }
    },
    { GE_FILTER_MAGNIFIER,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::MAGNIFIER);
            impl->MakeMagnifierParams();
        }
    },
    { GE_FILTER_WATER_RIPPLE,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::WATER_RIPPLE);
            impl->MakeWaterRippleParams();
        }
    }
};

GEVisualEffectImpl::GEVisualEffectImpl(const std::string& name)
{
    auto iter = g_initialMap.find(name);
    if (iter != g_initialMap.end()) {
        iter->second(this);
    }
}

GEVisualEffectImpl::~GEVisualEffectImpl() {}

void GEVisualEffectImpl::SetParam(const std::string& tag, int32_t param)
{
    switch (filterType_) {
        case FilterType::KAWASE_BLUR: {
            if (kawaseParams_ == nullptr) {
                return;
            }

            if (tag == GE_FILTER_KAWASE_BLUR_RADIUS) {
                kawaseParams_->radius = param;
            }
            break;
        }
        case FilterType::MESA_BLUR: {
            if (mesaParams_ == nullptr) {
                return;
            }

            if (tag == GE_FILTER_MESA_BLUR_RADIUS) {
                mesaParams_->radius = param;
            }
            if (tag == GE_FILTER_MESA_BLUR_STRETCH_TILE_MODE) {
                mesaParams_->tileMode = param;
            }
            break;
        }
        case FilterType::LINEAR_GRADIENT_BLUR: {
            if (linearGradientBlurParams_ == nullptr) {
                return;
            }

            if (tag == GE_FILTER_LINEAR_GRADIENT_BLUR_DIRECTION) {
                linearGradientBlurParams_->direction = param;
            }
            break;
        }
        case FilterType::MAGNIFIER: {
            if (magnifierParams_ == nullptr) {
                return;
            }

            if (tag == GE_FILTER_MAGNIFIER_ROTATE_DEGREE) {
                magnifierParams_->rotateDegree = param;
            }
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, bool param)
{
    switch (filterType_) {
        case FilterType::LINEAR_GRADIENT_BLUR: {
            if (linearGradientBlurParams_ == nullptr) {
                return;
            }

            if (tag == GE_FILTER_LINEAR_GRADIENT_BLUR_IS_OFF_SCREEN) {
                linearGradientBlurParams_->isOffscreenCanvas = param;
            }
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, int64_t param) {}

void GEVisualEffectImpl::SetParam(const std::string& tag, float param)
{
    switch (filterType_) {
        case FilterType::MESA_BLUR: {
            SetMESABlurParams(tag, param);
            break;
        }
        case FilterType::AIBAR: {
            SetAIBarParams(tag, param);
            break;
        }
        case FilterType::GREY: {
            SetGreyParams(tag, param);
            break;
        }

        case FilterType::LINEAR_GRADIENT_BLUR: {
            SetLinearGradientBlurParams(tag, param);
            break;
        }
        case FilterType::MAGNIFIER: {
            SetMagnifierParamsFloat(tag, param);
            break;
        }
        case FilterType::WATER_RIPPLE: {
            SetWaterRippleParams(tag, param);
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, double param) {}

void GEVisualEffectImpl::SetParam(const std::string& tag, const char* const param) {}

void GEVisualEffectImpl::SetParam(const std::string& tag, const std::shared_ptr<Drawing::Image> param) {}

void GEVisualEffectImpl::SetParam(const std::string& tag, const std::shared_ptr<Drawing::ColorFilter> param) {}

void GEVisualEffectImpl::SetParam(const std::string& tag, const Drawing::Matrix param)
{
    switch (filterType_) {
        case FilterType::LINEAR_GRADIENT_BLUR: {
            if (linearGradientBlurParams_ == nullptr) {
                return;
            }

            if (tag == GE_FILTER_LINEAR_GRADIENT_BLUR_CANVAS_MAT) {
                linearGradientBlurParams_->mat = param;
            }
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, const std::vector<std::pair<float, float>> param)
{
    switch (filterType_) {
        case FilterType::LINEAR_GRADIENT_BLUR: {
            if (linearGradientBlurParams_ == nullptr) {
                return;
            }
            if (tag == GE_FILTER_LINEAR_GRADIENT_BLUR_FRACTION_STOPS) {
                linearGradientBlurParams_->fractionStops = param;
            }
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetParam(const std::string& tag, const uint32_t param)
{
    switch (filterType_) {
        case FilterType::MAGNIFIER: {
            SetMagnifierParamsUint32(tag, param);
            break;
        }
        case FilterType::WATER_RIPPLE: {
            if (waterRippleParams_ == nullptr) {
                return;
            }
            if (tag == GE_FILTER_WATER_RIPPLE_RIPPLE_MODE) {
                waterRippleParams_->rippleMode = param;
            } else if (tag == GE_FILTER_WATER_RIPPLE_WAVE_NUM) {
                waterRippleParams_->waveCount = param;
            }
            break;
        }
        default:
            break;
    }
}

void GEVisualEffectImpl::SetMESABlurParams(const std::string& tag, float param)
{
    if (mesaParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_FILTER_MESA_BLUR_GREY_COEF_1,
            [](GEVisualEffectImpl* obj, float p) { obj->mesaParams_->greyCoef1 = p; } },
        { GE_FILTER_MESA_BLUR_GREY_COEF_2,
            [](GEVisualEffectImpl* obj, float p) { obj->mesaParams_->greyCoef2 = p; } },
        { GE_FILTER_MESA_BLUR_STRETCH_OFFSET_X,
            [](GEVisualEffectImpl* obj, float p) { obj->mesaParams_->offsetX = p; } },
        { GE_FILTER_MESA_BLUR_STRETCH_OFFSET_Y,
            [](GEVisualEffectImpl* obj, float p) { obj->mesaParams_->offsetY = p; } },
        { GE_FILTER_MESA_BLUR_STRETCH_OFFSET_Z,
            [](GEVisualEffectImpl* obj, float p) { obj->mesaParams_->offsetZ = p; } },
        { GE_FILTER_MESA_BLUR_STRETCH_OFFSET_W,
            [](GEVisualEffectImpl* obj, float p) { obj->mesaParams_->offsetW = p; } },
        { GE_FILTER_MESA_BLUR_STRETCH_WIDTH,
            [](GEVisualEffectImpl* obj, float p) { obj->mesaParams_->width = p; } },
        { GE_FILTER_MESA_BLUR_STRETCH_HEIGHT,
            [](GEVisualEffectImpl* obj, float p) { obj->mesaParams_->height = p; } }
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetAIBarParams(const std::string& tag, float param)
{
    if (aiBarParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_FILTER_AI_BAR_LOW,
            [](GEVisualEffectImpl* obj, float p) { obj->aiBarParams_->aiBarLow        = p; } },
        { GE_FILTER_AI_BAR_HIGH,
            [](GEVisualEffectImpl* obj, float p) { obj->aiBarParams_->aiBarHigh       = p; } },
        { GE_FILTER_AI_BAR_THRESHOLD,
            [](GEVisualEffectImpl* obj, float p) { obj->aiBarParams_->aiBarThreshold  = p; } },
        { GE_FILTER_AI_BAR_OPACITY,
            [](GEVisualEffectImpl* obj, float p) { obj->aiBarParams_->aiBarOpacity    = p; } },
        { GE_FILTER_AI_BAR_SATURATION,
            [](GEVisualEffectImpl* obj, float p) { obj->aiBarParams_->aiBarSaturation = p; } }
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetGreyParams(const std::string& tag, float param)
{
    if (greyParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_FILTER_GREY_COEF_1, [](GEVisualEffectImpl* obj, float p) { obj->greyParams_->greyCoef1 = p; } },
        { GE_FILTER_GREY_COEF_2, [](GEVisualEffectImpl* obj, float p) { obj->greyParams_->greyCoef2 = p; } }
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetLinearGradientBlurParams(const std::string& tag, float param)
{
    if (linearGradientBlurParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_FILTER_LINEAR_GRADIENT_BLUR_RADIUS,
            [](GEVisualEffectImpl* obj, float p) { obj->linearGradientBlurParams_->blurRadius = p; } },
        { GE_FILTER_LINEAR_GRADIENT_BLUR_GEO_WIDTH,
            [](GEVisualEffectImpl* obj, float p) { obj->linearGradientBlurParams_->geoWidth   = p; } },
        { GE_FILTER_LINEAR_GRADIENT_BLUR_GEO_HEIGHT,
            [](GEVisualEffectImpl* obj, float p) { obj->linearGradientBlurParams_->geoHeight  = p; } },
        { GE_FILTER_LINEAR_GRADIENT_BLUR_TRAN_X,
            [](GEVisualEffectImpl* obj, float p) { obj->linearGradientBlurParams_->tranX      = p; } },
        { GE_FILTER_LINEAR_GRADIENT_BLUR_TRAN_Y,
            [](GEVisualEffectImpl* obj, float p) { obj->linearGradientBlurParams_->tranY      = p; } }
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetMagnifierParamsFloat(const std::string& tag, float param)
{
    if (magnifierParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_FILTER_MAGNIFIER_FACTOR,
            [](GEVisualEffectImpl* obj, float p) { obj->magnifierParams_->factor = p; } },
        { GE_FILTER_MAGNIFIER_WIDTH,
            [](GEVisualEffectImpl* obj, float p) { obj->magnifierParams_->width = p; } },
        { GE_FILTER_MAGNIFIER_HEIGHT,
            [](GEVisualEffectImpl* obj, float p) { obj->magnifierParams_->height = p; } },
        { GE_FILTER_MAGNIFIER_CORNER_RADIUS,
            [](GEVisualEffectImpl* obj, float p) { obj->magnifierParams_->cornerRadius = p; } },
        { GE_FILTER_MAGNIFIER_BORDER_WIDTH,
            [](GEVisualEffectImpl* obj, float p) { obj->magnifierParams_->borderWidth = p; } },
        { GE_FILTER_MAGNIFIER_SHADOW_OFFSET_X,
            [](GEVisualEffectImpl* obj, float p) { obj->magnifierParams_->shadowOffsetX = p; } },
        { GE_FILTER_MAGNIFIER_SHADOW_OFFSET_Y,
            [](GEVisualEffectImpl* obj, float p) { obj->magnifierParams_->shadowOffsetY = p; } },
        { GE_FILTER_MAGNIFIER_SHADOW_SIZE,
            [](GEVisualEffectImpl* obj, float p) { obj->magnifierParams_->shadowSize = p; } },
        { GE_FILTER_MAGNIFIER_SHADOW_STRENGTH,
            [](GEVisualEffectImpl* obj, float p) { obj->magnifierParams_->shadowStrength = p; } }
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetMagnifierParamsUint32(const std::string& tag, uint32_t param)
{
    if (magnifierParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, uint32_t)>> actions = {
        { GE_FILTER_MAGNIFIER_GRADIENT_MASK_COLOR_1,
            [](GEVisualEffectImpl* obj, uint32_t p) { obj->magnifierParams_->gradientMaskColor1 = p; } },
        { GE_FILTER_MAGNIFIER_GRADIENT_MASK_COLOR_2,
            [](GEVisualEffectImpl* obj, uint32_t p) { obj->magnifierParams_->gradientMaskColor2 = p; } },
        { GE_FILTER_MAGNIFIER_OUTER_CONTOUR_COLOR_1,
            [](GEVisualEffectImpl* obj, uint32_t p) { obj->magnifierParams_->outerContourColor1 = p; } },
        { GE_FILTER_MAGNIFIER_OUTER_CONTOUR_COLOR_2,
            [](GEVisualEffectImpl* obj, uint32_t p) { obj->magnifierParams_->outerContourColor2 = p; } }
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

void GEVisualEffectImpl::SetWaterRippleParams(const std::string& tag, float param)
{
    if (waterRippleParams_ == nullptr) {
        return;
    }
 
    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        
        { GE_FILTER_WATER_RIPPLE_PROGRESS,
            [](GEVisualEffectImpl* obj, float p) { obj->waterRippleParams_->progress = p; } },
        { GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_X,
            [](GEVisualEffectImpl* obj, float p) { obj->waterRippleParams_->rippleCenterX = p; } },
        { GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_Y,
            [](GEVisualEffectImpl* obj, float p) { obj->waterRippleParams_->rippleCenterY = p; } },
    };
 
    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
