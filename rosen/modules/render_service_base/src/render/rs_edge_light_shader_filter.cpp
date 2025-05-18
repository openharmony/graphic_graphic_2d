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
#include "render/rs_edge_light_shader_filter.h"
#include <string>
#include "SkOpts.h"
#include "ge_shader_filter_params.h"
#include "platform/common/rs_log.h"

#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

namespace OHOS {
namespace Rosen {
static constexpr float COLOR_MAX = 255.0;

RSEdgeLightShaderFilter::RSEdgeLightShaderFilter(const EdgeLightShaderFilterParams& param)
    :detectColor_(param.detectColor),
    color_(param.color),
    edgeThreshold_(param.edgeThreshold),
    edgeIntensity_(param.edgeIntensity),
    edgeSoftThreshold_(param.edgeSoftThreshold),
    bloomLevel_(param.bloomLevel),
    useRawColor_(param.useRawColor),
    gradient_(param.gradient),
    alphaProgress_(param.alphaProgress),
    addImage_(param.addImage)
{
    type_ = ShaderFilterType::EDGE_LIGHT;
#ifdef USE_M133_SKIA
    hash_ = SkChecksum::Hash32(&detectColor_, sizeof(detectColor_), hash_);
    hash_ = SkChecksum::Hash32(&color_, sizeof(color_), hash_);
    hash_ = SkChecksum::Hash32(&edgeThreshold_, sizeof(edgeThreshold_), hash_);
    hash_ = SkChecksum::Hash32(&edgeIntensity_, sizeof(edgeIntensity_), hash_);
    hash_ = SkChecksum::Hash32(&edgeSoftThreshold_, sizeof(edgeSoftThreshold_), hash_);
    hash_ = SkChecksum::Hash32(&bloomLevel_, sizeof(bloomLevel_), hash_);
    hash_ = SkChecksum::Hash32(&useRawColor_, sizeof(useRawColor_), hash_);
    hash_ = SkChecksum::Hash32(&gradient_, sizeof(gradient_), hash_);
    hash_ = SkChecksum::Hash32(&alphaProgress_, sizeof(alphaProgress_), hash_);
    hash_ = SkChecksum::Hash32(&addImage_, sizeof(addImage_), hash_);
#else
    hash_ = SkOpts::hash(&detectColor_, sizeof(detectColor_), hash_);
    hash_ = SkOpts::hash(&color_, sizeof(color_), hash_);
    hash_ = SkOpts::hash(&edgeThreshold_, sizeof(edgeThreshold_), hash_);
    hash_ = SkOpts::hash(&edgeIntensity_, sizeof(edgeIntensity_), hash_);
    hash_ = SkOpts::hash(&edgeSoftThreshold_, sizeof(edgeSoftThreshold_), hash_);
    hash_ = SkOpts::hash(&bloomLevel_, sizeof(bloomLevel_), hash_);
    hash_ = SkOpts::hash(&useRawColor_, sizeof(useRawColor_), hash_);
    hash_ = SkOpts::hash(&gradient_, sizeof(gradient_), hash_);
    hash_ = SkOpts::hash(&alphaProgress_, sizeof(alphaProgress_), hash_);
    hash_ = SkOpts::hash(&addImage_, sizeof(addImage_), hash_);
#endif
}

RSEdgeLightShaderFilter::~RSEdgeLightShaderFilter() {}

void RSEdgeLightShaderFilter::Dump(std::string& out)
{
    std::string detectColorDump;
    detectColor_.Dump(detectColorDump);
    std::string colorDump;
    color_.Dump(colorDump);
    out += "[RSEdgeLightShaderFilter:[detectColor_:" + detectColorDump;
    out += " color:" + colorDump;
    out += " edgeThreshold:" + std::to_string(edgeThreshold_);
    out += " edgeIntensity" +  std::to_string(edgeIntensity_);
    out += " edgeSoftThreshold" +  std::to_string(edgeSoftThreshold_);
    out += " bloomLevel" +  std::to_string(bloomLevel_);
    out += " useRawColor" +  std::to_string(useRawColor_);
    out += " gradient" +  std::to_string(gradient_);
    out += " alphaProgress" +  std::to_string(alphaProgress_);
    out += " addImage" +  std::to_string(addImage_);
    out += "]]";
}


void RSEdgeLightShaderFilter::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    if (visualEffectContainer == nullptr) {
        return;
    }
    std::string out;
    Dump(out);
    ROSEN_LOGI("RSEdgeLightShaderFilter: %{public}s", out.c_str());

    auto edgeLightShaderFilter = std::make_shared<Drawing::GEVisualEffect>(
        Drawing::GE_FILTER_EDGE_LIGHT, Drawing::DrawingPaintType::BRUSH);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_EDGE_THRESHOLD, edgeThreshold_);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_EDGE_INTENSITY, edgeIntensity_);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_EDGE_SOFT_THRESHOLD, edgeSoftThreshold_);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_EDGE_DETECT_COLOR_R,
        detectColor_.GetRed() / COLOR_MAX);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_EDGE_DETECT_COLOR_G,
        detectColor_.GetGreen() / COLOR_MAX);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_EDGE_DETECT_COLOR_B,
        detectColor_.GetBlue() / COLOR_MAX);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_EDGE_COLOR_R, color_.GetRed() / COLOR_MAX);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_EDGE_COLOR_G, color_.GetGreen() / COLOR_MAX);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_EDGE_COLOR_B, color_.GetBlue() / COLOR_MAX);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_IF_RAW_COLOR, useRawColor_);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_MIP_LEVEL, bloomLevel_);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_IF_GRADIENT, gradient_);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_ALPHA_PROGRESS, alphaProgress_);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_MERGE_IF_IMAGE, addImage_);
    visualEffectContainer->AddToChainedFilter(edgeLightShaderFilter);
}

} // namespace Rosen
} // namespace OHOS