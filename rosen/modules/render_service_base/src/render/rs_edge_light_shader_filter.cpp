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
#include "common/rs_color.h"
#include "common/rs_vector4.h"
#include "ge_shader_filter_params.h"

#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

namespace OHOS {
namespace Rosen {
RSEdgeLightShaderFilter::RSEdgeLightShaderFilter(const EdgeLightShaderFilterParams& param)
    : alpha_(param.alpha),
    color_(param.color),
    mask_(param.mask)
{
    type_ = ShaderFilterType::EDGE_LIGHT;
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    hash_ = hashFunc(&alpha_, sizeof(alpha_), hash_);
    hash_ = hashFunc(&color_, sizeof(color_), hash_);
    if (mask_) {
        auto maskHash = mask_->Hash();
        hash_ = hashFunc(&maskHash, sizeof(maskHash), hash_);
    }
}

RSEdgeLightShaderFilter::~RSEdgeLightShaderFilter() {}

void RSEdgeLightShaderFilter::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    if (visualEffectContainer == nullptr) {
        return;
    }

    Vector4f color;
    if (color_.has_value()) {
        color = color_.value();
    }

    auto edgeLightShaderFilter = std::make_shared<Drawing::GEVisualEffect>(
        Drawing::GE_FILTER_EDGE_LIGHT, Drawing::DrawingPaintType::BRUSH);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_ALPHA, alpha_);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_EDGE_COLOR_R, color.x_);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_EDGE_COLOR_G, color.y_);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_EDGE_COLOR_B, color.z_);
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_USE_RAW_COLOR, !color_.has_value());
    edgeLightShaderFilter->SetParam(Drawing::GE_FILTER_EDGE_LIGHT_MASK,
        mask_ != nullptr ? mask_->GenerateGEShaderMask() : nullptr);
    visualEffectContainer->AddToChainedFilter(edgeLightShaderFilter);
}

} // namespace Rosen
} // namespace OHOS