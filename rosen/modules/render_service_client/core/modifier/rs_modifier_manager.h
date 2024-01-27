/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_MANAGER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_MANAGER_H

#include <memory>
#include <set>
#include <unordered_map>

#include "animation/rs_animation_rate_decider.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "render_service_base/include/pipeline/rs_render_display_sync.h"

namespace OHOS {
namespace Rosen {
class RSModifier;
class RSRenderAnimation;

class RSC_EXPORT RSModifierManager {
public:
    RSModifierManager() = default;
    virtual ~RSModifierManager() = default;

    void AddModifier(const std::shared_ptr<RSModifier>& modifier);
    void AddAnimation(const std::shared_ptr<RSRenderAnimation>& animation);
    void RemoveAnimation(const AnimationId keyId);
    bool HasUIAnimation();

    bool Animate(int64_t time, int64_t vsyncPeriod = 0);
    void Draw();

    void SetFrameRateGetFunc(const FrameRateGetFunc& func);
    const FrameRateRange& GetFrameRateRange() const;

    // spring animation related
    void RegisterSpringAnimation(PropertyId propertyId, AnimationId animId);
    void UnregisterSpringAnimation(PropertyId propertyId, AnimationId animId);
    std::shared_ptr<RSRenderAnimation> QuerySpringAnimation(PropertyId propertyId);

    bool JudgeAnimateWhetherSkip(AnimationId animId, int64_t time, int64_t vsyncPeriod);
    void SetDisplaySyncEnable(bool isDisplaySyncEnabled);
    bool IsDisplaySyncEnabled() const;
    void FlushStartAnimation(int64_t time);

private:
    void OnAnimationFinished(const std::shared_ptr<RSRenderAnimation>& animation);
    const std::shared_ptr<RSRenderAnimation> GetAnimation(AnimationId id) const;

    std::set<std::shared_ptr<RSModifier>> modifiers_;
    std::unordered_map<AnimationId, std::weak_ptr<RSRenderAnimation>> animations_;
    std::unordered_map<PropertyId, AnimationId> springAnimations_;

    RSAnimationRateDecider rateDecider_;
    FrameRateGetFunc frameRateGetFunc_;

    std::unordered_map<AnimationId, std::shared_ptr<RSRenderDisplaySync>> displaySyncs_;
    bool isDisplaySyncEnabled_ = false;

    template <typename T>
    friend class RSAnimatableProperty;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_MANAGER_H
