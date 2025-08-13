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

#include "ui_effect/property/include/rs_ui_color_gradient_filter.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

#undef LOG_TAG
#define LOG_TAG "RSNGColorGradientFilterImpl"

void RSNGColorGradientFilter::SetColors(std::vector<float> colors)
{
    Setter<ColorGradientColorsTag>(colors);
}

void RSNGColorGradientFilter::SetPositions(std::vector<float> positions)
{
    Setter<ColorGradientPositionsTag>(positions);
}

void RSNGColorGradientFilter::SetStrengths(std::vector<float> strengths)
{
    Setter<ColorGradientStrengthsTag>(strengths);
}

std::shared_ptr<RSNGRenderFilterBase> RSNGColorGradientFilterImpl::GetRenderEffect()
{
    return nullptr;
}

bool RSNGColorGradientFilterImpl::SetValue(
    const std::shared_ptr<RSNGFilterBase>& other, RSNode& node, const std::weak_ptr<ModifierNG::RSModifier>& modifier)
{
    if (other == nullptr || GetType() != other->GetType()) {
        return false;
    }

    auto otherDown = std::static_pointer_cast<RSNGColorGradientFilterImpl>(other);
    bool updateFlag = SetColors(otherDown->colors_) &&
        SetPositions(otherDown->positions_) &&
        SetStrengths(otherDown->strengths_);
    if (!updateFlag) {
        return false;
    }

    return Base::SetValue(other, node, modifier);
}

void RSNGColorGradientFilterImpl::Attach(RSNode& node, const std::weak_ptr<ModifierNG::RSModifier>& modifier)
{
    std::for_each(colors_.begin(), colors_.end(), [&node, &modifier](const auto& propTag) {
        (RSNGEffectUtils::Attach(propTag.value_, node, modifier));
    });
    std::for_each(positions_.begin(), positions_.end(), [&node, &modifier](const auto& propTag) {
        (RSNGEffectUtils::Attach(propTag.value_, node, modifier));
    });
    std::for_each(strengths_.begin(), strengths_.end(), [&node, &modifier](const auto& propTag) {
        (RSNGEffectUtils::Attach(propTag.value_, node, modifier));
    });
    Base::Attach(node, modifier);
}

void RSNGColorGradientFilterImpl::Detach()
{
    std::for_each(colors_.begin(), colors_.end(), [](const auto& propTag) {
        (RSNGEffectUtils::Detach(propTag.value_));
    });
    std::for_each(positions_.begin(), positions_.end(), [](const auto& propTag) {
        (RSNGEffectUtils::Detach(propTag.value_));
    });
    std::for_each(strengths_.begin(), strengths_.end(), [](const auto& propTag) {
        (RSNGEffectUtils::Detach(propTag.value_));
    });
    Base::Detach();
}

bool RSNGColorGradientFilterImpl::SetColors(std::vector<ColorGradientColorsTag> colors)
{
    size_t colorSize = colors.size();
    if (colorSize != colors_.size()) {
        return false;
    }

    for (size_t index = 0; index < colorSize; ++index) {
        colors_[index].value_->Set(colors[index].value_->Get());
    }
    return true;
}

bool RSNGColorGradientFilterImpl::SetPositions(std::vector<ColorGradientPositionsTag> positions)
{
    size_t positionSize = positions.size();
    if (positionSize != positions_.size()) {
        return false;
    }

    for (size_t index = 0; index < positionSize; ++index) {
        positions_[index].value_->Set(positions[index].value_->Get());
    }
    return true;
}

bool RSNGColorGradientFilterImpl::SetStrengths(std::vector<ColorGradientStrengthsTag> strengths)
{
    size_t strengthSize = strengths.size();
    if (strengthSize != strengths_.size()) {
        return false;
    }

    for (size_t index = 0; index < strengthSize; ++index) {
        strengths_[index].value_->Set(strengths[index].value_->Get());
    }
    return true;
}

} // namespace Rosen
} // namespace OHOS