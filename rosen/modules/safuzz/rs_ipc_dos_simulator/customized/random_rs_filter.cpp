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

#include "customized/random_rs_filter.h"

#include "customized/random_rs_gradient_blur_para.h"
#include "customized/random_rs_path.h"
#include "effect/shader_effect.h"
#include "random/random_data.h"
#include "random/random_engine.h"
#include "render/rs_aibar_shader_filter.h"
#include "render/rs_attraction_effect_filter.h"
#include "render/rs_blur_filter.h"
#include "render/rs_colorful_shadow_filter.h"
#include "render/rs_distortion_shader_filter.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_fly_out_shader_filter.h"
#include "render/rs_foreground_effect_filter.h"
#include "render/rs_gradient_blur_para.h"
#include "render/rs_hdr_ui_brightness_filter.h"
#include "render/rs_kawase_blur_shader_filter.h"
#include "render/rs_linear_gradient_blur_shader_filter.h"
#include "render/rs_magnifier_shader_filter.h"
#include "render/rs_material_filter.h"
#include "render/rs_motion_blur_filter.h"
#include "render/rs_skia_filter.h"
#include "render/rs_spherize_effect_filter.h"
#include "render/rs_water_ripple_shader_filter.h"

namespace OHOS {
namespace Rosen {
namespace {
BLUR_COLOR_MODE GetRandomBlurColorMode()
{
    static constexpr int BLUR_COLOR_MODE_INDEX_MAX = 2;
    return static_cast<BLUR_COLOR_MODE>(RandomEngine::GetRandomIndex(BLUR_COLOR_MODE_INDEX_MAX));
}

Drawing::TileMode GetRandomDrawingTileMode()
{
    static constexpr int DRAWING_TILE_MODE_INDEX_MAX = 3;
    return static_cast<Drawing::TileMode>(RandomEngine::GetRandomIndex(DRAWING_TILE_MODE_INDEX_MAX));
}
}

std::shared_ptr<RSFilter> RandomRSFilter::GetRandomRSFilter()
{
    static const std::vector<std::function<std::shared_ptr<RSFilter>()>> filterRandomGenerator = {
        RandomRSFilter::GetRandomFilter,
        RandomRSFilter::GetRandomBlurFilter,
        RandomRSFilter::GetRandomMaterialFilter,
        RandomRSFilter::GetRandomLightUpEffectFilter,
        RandomRSFilter::GetRandomAIBarFilter,
        RandomRSFilter::GetRandomLinearGradientBlurShaderFilter,
        RandomRSFilter::GetRandomForegroundEffectFilter,
        RandomRSFilter::GetRandomMotionBlurFilter,
        RandomRSFilter::GetRandomSpherizeEffectFilter,
        RandomRSFilter::GetRandomHDRUIBrightnessFilter,
        RandomRSFilter::GetRandomColorfulShadowFilter,
        RandomRSFilter::GetRandomAttractionEffectFilter,
        RandomRSFilter::GetRandomMagnifierShaderFilter,
        RandomRSFilter::GetRandomWaterRippleShaderFilter,
        RandomRSFilter::GetRandomFlyOutShaderFilter,
        RandomRSFilter::GetRandomDistortionFilter,
    };
    int index = RandomEngine::GetRandomIndex(filterRandomGenerator.size() - 1);
    auto rsFilter = filterRandomGenerator[index]();
    rsFilter->SetSnapshotOutset(RandomData::GetRandomBool());
    return rsFilter;
}

std::shared_ptr<RSFilter> RandomRSFilter::GetRandomFilter()
{
    std::shared_ptr<RSFilter> filter = std::make_shared<RSFilter>();
    return filter;
}

std::shared_ptr<RSFilter> RandomRSFilter::GetRandomBlurFilter()
{
    Vector2f blurRadius = RandomData::GetRandomVector2f();
    auto blurFilter = std::make_shared<RSBlurFilter>(blurRadius.x_, blurRadius.y_);
    if (RandomData::GetRandomBool()) {
        blurFilter->SetGreyCoef(RandomData::GetRandomVector2f());
    }
    return blurFilter;
}

std::shared_ptr<RSFilter> RandomRSFilter::GetRandomMaterialFilter()
{
    MaterialParam materialParam = {
        RandomData::GetRandomFloat(), RandomData::GetRandomFloat(),
        RandomData::GetRandomFloat(), RandomData::GetRandomColor()
    };
    auto filter = std::make_shared<RSMaterialFilter>(materialParam, GetRandomBlurColorMode());
    if (RandomData::GetRandomBool()) {
        filter->SetGreyCoef(RandomData::GetRandomVector2f());
    }
    return filter;
}

std::shared_ptr<RSFilter> RandomRSFilter::GetRandomLightUpEffectFilter()
{
    return RSFilter::CreateLightUpEffectFilter(RandomData::GetRandomFloat());
}

std::shared_ptr<RSFilter> RandomRSFilter::GetRandomAIBarFilter()
{
    float aiBarRadius = RandomData::GetRandomFloat();
    Drawing::TileMode tileMode = GetRandomDrawingTileMode();
    auto aiBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    auto filter = std::make_shared<RSDrawingFilter>(aiBarShaderFilter);
    if (RandomData::GetRandomBool()) {
        std::shared_ptr<RSKawaseBlurShaderFilter> kawaseBlurFilter =
            std::make_shared<RSKawaseBlurShaderFilter>(aiBarRadius);
        filter = filter->Compose(std::static_pointer_cast<RSShaderFilter>(kawaseBlurFilter));
    } else {
        auto blurFilter = Drawing::ImageFilter::CreateBlurImageFilter(aiBarRadius, aiBarRadius, tileMode, nullptr);
        uint32_t hash = SkOpts::hash(&aiBarRadius, sizeof(aiBarRadius), 0);
        filter = filter->Compose(blurFilter, hash);
    }
    filter->SetFilterType(RSFilter::AIBAR);
    return filter;
}

std::shared_ptr<RSFilter> RandomRSFilter::GetRandomLinearGradientBlurShaderFilter()
{
    auto para = RandomRSLinearGradientBlurPara::GetRandomRSLinearGradientBlurPara();
    float geoWidth = RandomData::GetRandomFloat();
    float geoHeight = RandomData::GetRandomFloat();
    auto shaderFilter = std::make_shared<RSLinearGradientBlurShaderFilter>(para, geoWidth, geoHeight);
    auto filter = std::make_shared<RSDrawingFilter>(shaderFilter);
    filter->SetFilterType(RSFilter::LINEAR_GRADIENT_BLUR);
    return filter;
}

std::shared_ptr<RSFilter> RandomRSFilter::GetRandomForegroundEffectFilter()
{
    float radius = RandomData::GetRandomFloat();
    return std::make_shared<RSForegroundEffectFilter>(radius);
}

std::shared_ptr<RSFilter> RandomRSFilter::GetRandomMotionBlurFilter()
{
    auto motionBlurParam = RandomData::GetRandomMotionBlurParamSharedPtr();
    auto filter = std::make_shared<RSMotionBlurFilter>(motionBlurParam);
    filter->DisableMotionBlur(RandomData::GetRandomBool());
    return filter;
}

std::shared_ptr<RSFilter> RandomRSFilter::GetRandomSpherizeEffectFilter()
{
    float spherizeDegree = RandomData::GetRandomFloat();
    return std::make_shared<RSSpherizeEffectFilter>(spherizeDegree);
}

std::shared_ptr<RSFilter> RandomRSFilter::GetRandomHDRUIBrightnessFilter()
{
    float hdrUIBrightness = RandomData::GetRandomFloat();
    return std::make_shared<RSHDRUIBrightnessFilter>(hdrUIBrightness);
}

std::shared_ptr<RSFilter> RandomRSFilter::GetRandomColorfulShadowFilter()
{
    float blurRadius = RandomData::GetRandomFloat();
    float offsetX = RandomData::GetRandomFloat();
    float offsetY = RandomData::GetRandomFloat();
    Drawing::Path path = RandomRSPath::GetRandomDrawingPath();
    bool isFill = RandomData::GetRandomBool();
    return std::make_shared<RSColorfulShadowFilter>(blurRadius, offsetX, offsetY, path, isFill);
}

std::shared_ptr<RSFilter> RandomRSFilter::GetRandomAttractionEffectFilter()
{
    float attractionFraction = RandomData::GetRandomFloat();
    auto filter = std::make_shared<RSAttractionEffectFilter>(attractionFraction);
    float width = RandomData::GetRandomFloat();
    float height = RandomData::GetRandomFloat();
    Vector2f pt = RandomData::GetRandomVector2f();
    filter->CalculateWindowStatus(width, height, pt);
    return filter;
}

std::shared_ptr<RSFilter> RandomRSFilter::GetRandomMagnifierShaderFilter()
{
    auto para = RandomData::GetRandomRSMagnifierParamsSharedPtr();
    auto shaderFilter = std::make_shared<RSMagnifierShaderFilter>(para);
    auto matrix = RandomData::GetRandomDrawingMatrix();
    shaderFilter->SetMagnifierOffset(matrix);
    auto filter = std::make_shared<RSDrawingFilter>(shaderFilter);
    filter->SetFilterType(RSFilter::MAGNIFIER);
    return filter;
}

std::shared_ptr<RSFilter> RandomRSFilter::GetRandomWaterRippleShaderFilter()
{
    auto para = RandomData::GetRandomRSWaterRipplePara();
    float progress = RandomData::GetRandomFloat();
    auto shaderFilter = std::make_shared<RSWaterRippleShaderFilter>(
        progress, para.waveCount, para.rippleCenterX, para.rippleCenterY, para.rippleMode);
    auto filter = std::make_shared<RSDrawingFilter>(shaderFilter);
    filter->SetFilterType(RandomData::GetRandomBool() ? RSFilter::WATER_RIPPLE : RSFilter::COMPOUND_EFFECT);
    return filter;
}

std::shared_ptr<RSFilter> RandomRSFilter::GetRandomFlyOutShaderFilter()
{
    float degree = RandomData::GetRandomFloat();
    auto para = RandomData::GetRandomRSFlyOutPara();
    return std::make_shared<RSFlyOutShaderFilter>(degree, para.flyMode);
}

std::shared_ptr<RSFilter> RandomRSFilter::GetRandomDistortionFilter()
{
    float distortion = RandomData::GetRandomFloat();
    float scaleCoord = RandomData::GetRandomFloat();
    float offsetX = RandomData::GetRandomFloat();
    float offsetY = RandomData::GetRandomFloat();
    auto filter = std::make_shared<RSDistortionFilter>(distortion);
    filter->scaleCoord_ = scaleCoord;
    filter->offsetX_ = offsetX;
    filter->offsetY_ = offsetY;
    return filter;
}
} // namespace Rosen
} // namespace OHOS
