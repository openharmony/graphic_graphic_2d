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
#include "render/rs_render_magnifier_filter.h"

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "src/core/SkOpts.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr static float FLOAT_ZERO_THRESHOLD = 0.001f;
} // namespace

RSMagnifierShaderFilter::RSMagnifierShaderFilter(const std::shared_ptr<RSMagnifierParams>& para)
    : RSRenderFilterParaBase(RSUIFilterType::MAGNIFIER), magnifierPara_(para)
{
    hash_ = SkOpts::hash(&magnifierPara_, sizeof(magnifierPara_), hash_);
}

void RSMagnifierShaderFilter::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    if (magnifierPara_ == nullptr || scaleX_ < FLOAT_ZERO_THRESHOLD || scaleY_ < FLOAT_ZERO_THRESHOLD) {
        ROSEN_LOGD("RSMagnifierShaderFilter::GenerateGEVisualEffect magnifierPara_ is nullptr!");
        return;
    }

    auto magnifierFilter = std::make_shared<Drawing::GEVisualEffect>("MAGNIFIER", Drawing::DrawingPaintType::BRUSH);
    if (magnifierFilter == nullptr) {
        ROSEN_LOGD("RSMagnifierShaderFilter::GenerateGEVisualEffect magnifierFilter is nullptr!");
        return;
    }
    magnifierFilter->SetParam("FACTOR", magnifierPara_->factor_);
    magnifierFilter->SetParam("WIDTH", magnifierPara_->width_ * scaleX_);
    magnifierFilter->SetParam("HEIGHT", magnifierPara_->height_ * scaleY_);
    magnifierFilter->SetParam("CORNERRADIUS", magnifierPara_->cornerRadius_ * scaleY_);
    magnifierFilter->SetParam("BORDERWIDTH", magnifierPara_->borderWidth_ * scaleY_);
    magnifierFilter->SetParam("SHADOWOFFSETX", magnifierPara_->shadowOffsetX_ * scaleX_);
    magnifierFilter->SetParam("SHADOWOFFSETY", magnifierPara_->shadowOffsetY_ * scaleY_);
    magnifierFilter->SetParam("SHADOWSIZE", magnifierPara_->shadowSize_ * scaleY_);
    magnifierFilter->SetParam("SHADOWSTRENGTH", magnifierPara_->shadowStrength_);
    magnifierFilter->SetParam("GRADIENTMASKCOLOR1", magnifierPara_->gradientMaskColor1_);
    magnifierFilter->SetParam("GRADIENTMASKCOLOR2", magnifierPara_->gradientMaskColor2_);
    magnifierFilter->SetParam("OUTERCONTOURCOLOR1", magnifierPara_->outerContourColor1_);
    magnifierFilter->SetParam("OUTERCONTOURCOLOR2", magnifierPara_->outerContourColor2_);
    magnifierFilter->SetParam("ROTATEDEGREE", rotateDegree_);

    visualEffectContainer->AddToChainedFilter(magnifierFilter);
}

void RSMagnifierShaderFilter::SetMagnifierOffset(Drawing::Matrix& mat)
{
    if (!magnifierPara_) {
        ROSEN_LOGD("RSMagnifierShaderFilter::SetMagnifierOffset magnifierPara_ is nullptr!");
        return;
    }

    // 1 and 3 represents index
    if ((mat.Get(1) > FLOAT_ZERO_THRESHOLD) && (mat.Get(3) < (0 - FLOAT_ZERO_THRESHOLD))) {
        rotateDegree_ = 90; // 90 represents rotate degree
        scaleX_ = mat.Get(1);
        scaleY_ = -mat.Get(3); // 3 represents index
        offsetX_ = magnifierPara_->offsetY_ * scaleX_;
        offsetY_ = -magnifierPara_->offsetX_ * scaleY_;
    // 0 and 4 represents index
    } else if ((mat.Get(0) < (0 - FLOAT_ZERO_THRESHOLD)) && (mat.Get(4) < (0 - FLOAT_ZERO_THRESHOLD))) {
        rotateDegree_ = 180; // 180 represents rotate degree
        scaleX_ = -mat.Get(0);
        scaleY_ = -mat.Get(4); // 4 represents index
        offsetX_ = -magnifierPara_->offsetX_ * scaleX_;
        offsetY_ = -magnifierPara_->offsetY_ * scaleY_;
    // 1 and 3 represents index
    } else if ((mat.Get(1) < (0 - FLOAT_ZERO_THRESHOLD)) && (mat.Get(3) > FLOAT_ZERO_THRESHOLD)) {
        rotateDegree_ = 270; // 270 represents rotate degree
        scaleX_ = -mat.Get(1);
        scaleY_ = mat.Get(3); // 3 represents index
        offsetX_ = -magnifierPara_->offsetY_ * scaleX_;
        offsetY_ = magnifierPara_->offsetX_ * scaleY_;
    } else {
        rotateDegree_ = 0;
        scaleX_ = mat.Get(0);
        scaleY_ = mat.Get(4); // 4 represents index
        offsetX_ = magnifierPara_->offsetX_ * scaleX_;
        offsetY_ = magnifierPara_->offsetY_ * scaleY_;
    }
}

} // namespace Rosen
} // namespace OHOS
