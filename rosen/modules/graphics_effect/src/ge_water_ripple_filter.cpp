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
 
#include "chrono"
 
#include "common/rs_common_def.h"
#include "ge_log.h"
#include "ge_water_ripple_filter.h"
 
namespace OHOS {
namespace Rosen {
// Advanced Filter
#define PROPERTY_HIGPU_VERSION "const.gpu.vendor"
#define PROPERTY_DEBUG_SUPPORT_AF "persist.sys.graphic.supports_af"
namespace {
 
static std::shared_ptr<Drawing::RuntimeEffect> g_waterRippleEffect = nullptr;
static std::shared_ptr<Drawing::RuntimeEffect> g_waterRipplePcEffect = nullptr;
 
} // namespace
 
 
GEWaterRippleFilter::GEWaterRippleFilter(const Drawing::GEWaterRippleFilterParams& params)
    : progress_(params.progress), waveCount_(params.waveCount), rippleCenterX_(params.rippleCenterX),
    rippleCenterY_(params.rippleCenterY)
{
    if (!InitWaterRippleEffect()) {
        LOGE("GEWaterRippleFilter::GEWaterRippleFilter failed when initializing WaterRippleEffect.");
        return;
    }
}
 
std::shared_ptr<Drawing::Image> GEWaterRippleFilter::ProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (image == nullptr) {
        LOGE("GEWaterRippleFilter::ProcessImage input is invalid");
        return nullptr;
    }
 
    Drawing::Matrix matrix;
    auto shader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    if (g_waterRippleEffect == nullptr) {
        if (InitWaterRippleEffect() == false) {
            LOGE("GEWaterRippleFilter::ProcessImage g_waterRippleEffect init failed");
            return nullptr;
        }
    }
    if (g_waterRipplePcEffect == nullptr) {
        if (InitWaterRipplePcEffect() == false) {
            LOGE("GEWaterRippleFilter::ProcessImage g_waterRipplePcEffect init failed");
            return nullptr;
        }
    }
    auto imageInfo = image->GetImageInfo();
    float height = imageInfo.GetHeight();
    float width = imageInfo.GetWidth();
    std::shared_ptr<Drawing::RuntimeEffect> waterRipple;
    if (ROSEN_GNE(rippleCenterY_, 0.5f)) {
        waterRipple = g_waterRipplePcEffect;
    } else {
        waterRipple = g_waterRippleEffect;
    }
    Drawing::RuntimeShaderBuilder builder(waterRipple);
 
    builder.SetChild("image", shader);
    builder.SetUniform("iResolution", width, height);
    builder.SetUniform("progress", progress_);
    builder.SetUniform("waveCount", waveCount_);
    builder.SetUniform("rippleCenter", rippleCenterX_, rippleCenterY_);
 
    auto invertedImage = builder.MakeImage(canvas.GetGPUContext().get(), nullptr, imageInfo, false);
    if (invertedImage == nullptr) {
        LOGE("GEWaterRippleFilter::ProcessImage make image failed");
        return nullptr;
    }
    return invertedImage;
}

bool GEWaterRippleFilter::InitWaterRippleEffect()
{
    g_waterRippleEffect = Drawing::RuntimeEffect::CreateForShader(shaderString);
    if (g_waterRippleEffect == nullptr) {
        LOGE("GEWaterRippleFilter::RuntimeShader failed to create water ripple filter");
        return false;
    }
    return true;
}

bool GEWaterRippleFilter::InitWaterRipplePcEffect()
{
    g_waterRipplePcEffect = Drawing::RuntimeEffect::CreateForShader(shaderPcString);
    if (g_waterRipplePcEffect == nullptr) {
        LOGE("GEWaterRippleFilter::RuntimeShader failed to create water ripple filter");
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS