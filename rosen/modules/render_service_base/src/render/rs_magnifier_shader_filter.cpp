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
#include "render/rs_magnifier_shader_filter.h"

#include "platform/common/rs_log.h"
#include "src/core/SkOpts.h"

namespace OHOS {
namespace Rosen {

RSMagnifierShaderFilter::RSMagnifierShaderFilter(const std::shared_ptr<RSMagnifierParams>& para)
    : magnifierPara_(para)
{
    type_ = ShaderFilterType::MAGNIFIER;
    hash_ = SkOpts::hash(&magnifierPara_, sizeof(magnifierPara_), hash_);
}

RSMagnifierShaderFilter::~RSMagnifierShaderFilter() = default;

void RSMagnifierShaderFilter::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    if (magnifierPara_ == nullptr) {
        ROSEN_LOGD("RSMagnifierShaderFilter::GenerateGEVisualEffect magnifierPara_ is nullptr!");
        return;
    }

    auto magnifierFilter = std::make_shared<Drawing::GEVisualEffect>("MAGNIFIER", Drawing::DrawingPaintType::BRUSH);
    if (magnifierFilter == nullptr) {
        ROSEN_LOGD("RSMagnifierShaderFilter::GenerateGEVisualEffect magnifierFilter is nullptr!");
        return;
    }
    magnifierFilter->SetParam("FACTOR", magnifierPara_->factor_);
    magnifierFilter->SetParam("WIDTH", magnifierPara_->width_);
    magnifierFilter->SetParam("HEIGHT", magnifierPara_->height_);
    magnifierFilter->SetParam("BORDERWIDTH", magnifierPara_->borderWidth_);
    magnifierFilter->SetParam("CORNERRADIUS", magnifierPara_->cornerRadius_);
    magnifierFilter->SetParam("SHADOWOFFSETX", magnifierPara_->shadowOffsetX_);
    magnifierFilter->SetParam("SHADOWOFFSETY", magnifierPara_->shadowOffsetY_);
    magnifierFilter->SetParam("SHADOWSIZE", magnifierPara_->shadowSize_);
    magnifierFilter->SetParam("SHADOWSTRENGTH", magnifierPara_->shadowStrength_);
    magnifierFilter->SetParam("GRADIENTMASKCOLOR1", magnifierPara_->gradientMaskColor1_);
    magnifierFilter->SetParam("GRADIENTMASKCOLOR2", magnifierPara_->gradientMaskColor2_);
    magnifierFilter->SetParam("OUTERCONTOURCOLOR1", magnifierPara_->outerContourColor1_);
    magnifierFilter->SetParam("OUTERCONTOURCOLOR2", magnifierPara_->outerContourColor2_);

    visualEffectContainer->AddToChainedFilter(magnifierFilter);
}

} // namespace Rosen
} // namespace OHOS
