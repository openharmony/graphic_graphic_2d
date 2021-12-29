/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_MANAGER_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_MANAGER_H

#include <list>
#include <memory>
#include <unordered_map>

#include "animation/rs_animatable_property.h"
#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSDirtyRegionManager;
class RSPaintFilterCanvas;
class RSProperties;
class RSRenderAnimation;
class RSPropertyRenderNode;

class RSAnimationManager {
public:
    using TransitionCallback = std::function<void(RSPaintFilterCanvas& canvas, const RSProperties& renderProperties)>;
    explicit RSAnimationManager(RSPropertyRenderNode* target);
    ~RSAnimationManager() = default;

    void AddAnimation(const std::shared_ptr<RSRenderAnimation>& animation);
    void RemoveAnimation(AnimationId keyId);
    bool Animate(int64_t time);
    std::shared_ptr<RSRenderAnimation> GetAnimation(AnimationId id) const;
    void RegisterTransition(AnimationId id, const TransitionCallback& transition);
    void UnregisterTransition(AnimationId id);
    void AddDisappearingChild(std::weak_ptr<RSPropertyRenderNode> child);
    void RemoveDisappearingChild(std::weak_ptr<RSPropertyRenderNode> child);
    void UpdateDisappearingChildren(RSDirtyRegionManager& dirtyManager, const RSProperties* parent, bool parentDirty);
    void RenderDisappearingChildren(RSPaintFilterCanvas& canvas);
    void DoTransition(RSPaintFilterCanvas& canvas, const RSProperties& renderProperties);
    bool HasTransition();

private:
    void OnAnimationRemove(const RSAnimatableProperty& property);
    void OnAnimationAdd(const RSAnimatableProperty& property);
    void OnAnimationFinished(std::shared_ptr<RSRenderAnimation>& animation);
    void ClearTransition(AnimationId id);

    RSPropertyRenderNode* target_;
    std::unordered_map<AnimationId, std::shared_ptr<RSRenderAnimation>> animations_;
    std::unordered_map<RSAnimatableProperty, int> animationNum_;
    std::list<std::pair<AnimationId, TransitionCallback>> transition_;
    std::list<std::weak_ptr<RSPropertyRenderNode>> disappearingChildren_;
    friend class RSPropertyRenderNode;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_MANAGER_H