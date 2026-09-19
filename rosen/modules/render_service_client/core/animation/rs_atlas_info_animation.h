/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ATLAS_INFO_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ATLAS_INFO_ANIMATION_H

#include "animation/rs_animation.h"
#include "common/rs_atlas_info.h"
#include <memory>

namespace OHOS {
namespace Rosen {

class RSRenderAtlasInfoAnimation;

/**
 * @brief AtlasInfo animation class (client side).
 *
 * Inherits RSAnimation. OnStart() creates RSRenderAtlasInfoAnimation and sends
 * it to the render service via IPC command. Render-side OnAnimate(fraction) does:
 *   fraction → interpolated currentFrame → ComputeRectInfo() → property_->Set()
 *
 * Time transformations (speed/pause/repeat/autoReverse/startDelay) are handled
 * by the RSAnimationFraction base class via UpdateParamToRenderAnimation().
 */
class RSC_EXPORT RSAtlasInfoAnimation : public RSAnimation {
public:
    RSAtlasInfoAnimation(const std::shared_ptr<RSUIContext>& rsUIContext,
        const std::shared_ptr<RSPropertyBase>& property,
        const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue);
    ~RSAtlasInfoAnimation() override = default;

    PropertyId GetPropertyId() const override
    {
        return propertyId_;
    }

    ModifierNG::RSPropertyType GetPropertyType() const override
    {
        return ModifierNG::RSPropertyType::ATLAS_INFO;
    }

protected:
    void OnStart() override;
    void OnUpdateStagingValue(bool isFirstStart) override;
    void RebuildInRender() override;

private:
    std::shared_ptr<RSRenderAtlasInfoAnimation> CreateRenderAnimation();
    PropertyId propertyId_ = 0;
    std::shared_ptr<RSPropertyBase> startValue_;
    std::shared_ptr<RSPropertyBase> endValue_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ATLAS_INFO_ANIMATION_H
