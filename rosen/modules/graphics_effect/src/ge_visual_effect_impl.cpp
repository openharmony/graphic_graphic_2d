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

#include "ge_visual_effect_impl.h"

#include <unordered_map>

#include "ge_log.h"

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
    { GE_FILTER_HPS_BLUR,
        [](GEVisualEffectImpl* impl) {
            impl->SetFilterType(GEVisualEffectImpl::FilterType::HPS_BLUR);
            impl->MakeHpsBlurParams();
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
        case FilterType::LINEAR_GRADIENT_BLUR: {
            if (linearGradientBlurParams_ == nullptr) {
                return;
            }

            if (tag == GE_FILTER_LINEAR_GRADIENT_BLUR_DIRECTION) {
                linearGradientBlurParams_->direction = param;
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
        case FilterType::HPS_BLUR: {
            SetHpsBlurParams(tag, param);
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

void GEVisualEffectImpl::SetHpsBlurParams(const std::string& tag, float param)
{
    if (hpsBlurParams_ == nullptr) {
        return;
    }

    static std::unordered_map<std::string, std::function<void(GEVisualEffectImpl*, float)>> actions = {
        { GE_FILTER_HPS_BLUR_RADIUS, [](GEVisualEffectImpl* obj, float p) { obj->hpsBlurParams_->radius = p; } },
        { GE_FILTER_HPS_BLUR_SATURATION,
            [](GEVisualEffectImpl* obj, float p) { obj->hpsBlurParams_->saturation = p; } },
        { GE_FILTER_HPS_BLUR_BRIGHTNESS, [](GEVisualEffectImpl* obj, float p) { obj->hpsBlurParams_->brightness = p; } }
    };

    auto it = actions.find(tag);
    if (it != actions.end()) {
        it->second(this, param);
    }
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
