/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PROPERTY_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PROPERTY_ANIMATION_H

#include <memory>

#include "animation/rs_animation.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSPropertyBase;

class RSC_EXPORT RSPropertyAnimation : public RSAnimation {
public:
    RSPropertyAnimation() = delete;
    virtual ~RSPropertyAnimation() = default;

    void SetIsCustom(const bool isCustom);

    void DumpAnimationInfo(std::string& dumpInfo) const override;

    RSModifierType GetModifierType() const override
    {
        return property_ ? property_->type_ : RSModifierType::INVALID;
    }

    ModifierNG::RSPropertyType GetPropertyType() const override
    {
        return property_ ? property_->GetPropertyTypeNG() : ModifierNG::RSPropertyType::INVALID;
    }

protected:
    RSPropertyAnimation(std::shared_ptr<RSPropertyBase> property);

    void SetAdditive(bool isAdditive);

    bool GetAdditive() const;

    const std::shared_ptr<RSPropertyBase> GetOriginValue() const;

    void SetPropertyValue(const std::shared_ptr<RSPropertyBase>& value);

    const std::shared_ptr<RSPropertyBase> GetPropertyValue() const;

    PropertyId GetPropertyId() const override;

    void OnStart() override;

    void SetOriginValue(const std::shared_ptr<RSPropertyBase>& originValue);

    virtual void InitInterpolationValue();

    void OnUpdateStagingValue(bool isFirstStart) override;

    void UpdateStagingValueOnInteractiveFinish(RSInteractiveAnimationPosition pos) override;

    void SetPropertyOnAllAnimationFinish() override;

    std::shared_ptr<RSPropertyBase> property_;
    std::shared_ptr<RSPropertyBase> byValue_ {};
    std::shared_ptr<RSPropertyBase> startValue_ {};
    std::shared_ptr<RSPropertyBase> endValue_ {};
    std::shared_ptr<RSPropertyBase> originValue_ {};
    bool isAdditive_ { true };
    bool isCustom_ { false };
    bool isDelta_ { false };
    bool hasOriginValue_ { false };

private:
    void InitAdditiveMode();
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PROPERTY_ANIMATION_H
