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
 
#include <chrono>
 
#include "ge_log.h"
#include "ge_water_ripple_filter.h"
 
namespace OHOS {
namespace Rosen {
// Advanced Filter
#define PROPERTY_HIGPU_VERSION "const.gpu.vendor"
#define PROPERTY_DEBUG_SUPPORT_AF "persist.sys.graphic.supports_af"
namespace {

const int SMALL2MEDIUM_RECV = 0;
const int SMALL2MEDIUM_SEND = 1;
const int SMALL2SMALL = 2;
const int MINI_RECV = 3;

} // namespace
 
 
GEWaterRippleFilter::GEWaterRippleFilter(const Drawing::GEWaterRippleFilterParams& params)
    : progress_(params.progress), waveCount_(params.waveCount), rippleCenterX_(params.rippleCenterX),
    rippleCenterY_(params.rippleCenterY), rippleMode_(params.rippleMode)
{}
 
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
    auto imageInfo = image->GetImageInfo();
    float height = imageInfo.GetHeight();
    float width = imageInfo.GetWidth();
    if (height < 1e-6 || width < 1e-6) {
        return nullptr;
    }
    auto waterRipple = GetWaterRippleEffect();
    if (waterRipple == nullptr) {
        LOGE("GEWaterRippleFilter::ProcessImage g_waterRippleEffect init failed");
        return nullptr;
    }
    Drawing::RuntimeShaderBuilder builder(waterRipple);
    builder.SetChild("image", shader);
    builder.SetUniform("iResolution", width, height);
    builder.SetUniform("progress", progress_);
    builder.SetUniform("waveCount", static_cast<float>(waveCount_));
    builder.SetUniform("rippleCenter", rippleCenterX_, rippleCenterY_);
#ifdef RS_ENABLE_GPU
    auto invertedImage = builder.MakeImage(canvas.GetGPUContext().get(), nullptr, imageInfo, false);
#else
    auto invertedImage = builder.MakeImage(nullptr, nullptr, imageInfo, false);
#endif
    if (invertedImage == nullptr) {
        LOGE("GEWaterRippleFilter::ProcessImage make image failed");
        return nullptr;
    }
    return invertedImage;
}

std::shared_ptr<Drawing::RuntimeEffect> GEWaterRippleFilter::GetWaterRippleEffect()
{
    switch (rippleMode_) {
        case SMALL2MEDIUM_RECV: {
            return GetWaterRippleEffectSM(SMALL2MEDIUM_RECV);
        }
        case SMALL2MEDIUM_SEND: {
            return GetWaterRippleEffectSM(SMALL2MEDIUM_SEND);
        }
        case SMALL2SMALL: {
            return GetWaterRippleEffectSS();
        }
        case MINI_RECV: {
            return GetWaterRippleEffectMR();
        }
        default: {
            LOGE("GEWaterRippleFilter::ProcessImage: Not support current ripple mode");
            return nullptr;
        }
    }
}

std::shared_ptr<Drawing::RuntimeEffect> GEWaterRippleFilter::GetWaterRippleEffectSM(const int rippleMode)
{
    static std::shared_ptr<Drawing::RuntimeEffect> g_waterRippleEffectSM = nullptr;
    if (g_waterRippleEffectSM == nullptr) {
        g_waterRippleEffectSM = (rippleMode == SMALL2MEDIUM_SEND) ?
            Drawing::RuntimeEffect::CreateForShader(shaderStringSMsend) :
            Drawing::RuntimeEffect::CreateForShader(shaderStringSMrecv);
    }
    return g_waterRippleEffectSM;
}

std::shared_ptr<Drawing::RuntimeEffect> GEWaterRippleFilter::GetWaterRippleEffectSS()
{
    static std::shared_ptr<Drawing::RuntimeEffect> g_waterRippleEffectSS = nullptr;
    if (g_waterRippleEffectSS == nullptr) {
        g_waterRippleEffectSS = Drawing::RuntimeEffect::CreateForShader(shaderStringSSmutual);
    }
    return g_waterRippleEffectSS;
}

std::shared_ptr<Drawing::RuntimeEffect> GEWaterRippleFilter::GetWaterRippleEffectMR()
{
    static std::shared_ptr<Drawing::RuntimeEffect> g_waterRippleEffectMR = nullptr;
    if (g_waterRippleEffectMR == nullptr) {
        g_waterRippleEffectMR = Drawing::RuntimeEffect::CreateForShader(shaderStringMiniRecv);
    }
    return g_waterRippleEffectMR;
}

} // namespace Rosen
} // namespace OHOS