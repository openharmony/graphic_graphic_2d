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
#include "render/rs_hps_blur_shader_filter.h"

#include "include/gpu/GrDirectContext.h"
#include "src/core/SkOpts.h"

#include "effect/color_matrix.h"
#include "effect/runtime_shader_builder.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
RSHpsBlurShaderFilter::RSHpsBlurShaderFilter(float radius, float saturation, float brightness)
    : radius_(radius), saturation_(saturation), brightness_(brightness)
{
    type_ = ShaderFilterType::HPS;

    hash_ = SkOpts::hash(&radius_, sizeof(radius_), 0);
    hash_ = SkOpts::hash(&saturation_, sizeof(saturation_), hash_);
    hash_ = SkOpts::hash(&brightness_, sizeof(brightness_), hash_);
}

RSHpsBlurShaderFilter::~RSHpsBlurShaderFilter() = default;

void RSHpsBlurShaderFilter::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    auto hpsFilter = std::make_shared<Drawing::GEVisualEffect>("HPS_BLUR", Drawing::DrawingPaintType::BRUSH);
    if ((hpsFilter == nullptr) || (hpsFilter->IsImplNull())) {
        ROSEN_LOGE("RSHpsBlurShaderFilter::GenerateGEVisualEffect error");
        return;
    }

    hpsFilter->SetParam("HPS_BLUR_RADIUS", radius_); // blur radius
    hpsFilter->SetParam("HPS_BLUR_SATURATION", saturation_);
    hpsFilter->SetParam("HPS_BLUR_BRIGHTNESS", brightness_);

    visualEffectContainer->AddToChainedFilter(hpsFilter);
}
} // namespace Rosen
} // namespace OHOS