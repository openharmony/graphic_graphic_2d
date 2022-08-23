/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_UI_ANIMATION_MANAGER_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_UI_ANIMATION_MANAGER_H

#include <list>
#include <memory>
#include <unordered_map>

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSModifierManager;
class RSAnimation;
class RSRenderAnimation;
class RSPropertyBase;
class RSRenderPropertyBase;

class RS_EXPORT RSUIAnimationManager {
public:
    RSUIAnimationManager();
    virtual ~RSUIAnimationManager() = default;

    void AddAnimation(const std::shared_ptr<RSRenderAnimation>& animation,
        const std::shared_ptr<RSAnimation>& uiAnimation);
    void RemoveAnimation(const AnimationId keyId);
    const std::shared_ptr<RSRenderAnimation> GetAnimation(AnimationId id) const;
    void AddAnimatableProp(const PropertyId id,
        const std::shared_ptr<RSPropertyBase>& uiProperty,
        const std::shared_ptr<RSRenderPropertyBase>& renderProperty);
    const std::shared_ptr<RSRenderPropertyBase> GetRenderProperty(
        const PropertyId id);
    void RemoveProperty(const PropertyId id);

    bool Animate(int64_t time);
    void Draw();

private:
    void OnAnimationRemove(const std::shared_ptr<RSRenderAnimation>& animation);
    void OnAnimationAdd(const std::shared_ptr<RSRenderAnimation>& animation);
    void OnAnimationFinished(const std::shared_ptr<RSRenderAnimation>& animation);
    bool UpdateAnimateValue(const std::shared_ptr<RSRenderAnimation>& animation, int64_t time);
    void RemoveUIAnimation(const AnimationId id);

    std::shared_ptr<RSModifierManager> modifierManager_;
    std::unordered_map<AnimationId, std::shared_ptr<RSRenderAnimation>> animations_;
    std::unordered_map<AnimationId, std::shared_ptr<RSAnimation>> uiAnimations_;
    std::unordered_map<PropertyId, int> animationNum_;
    std::unordered_map<PropertyId, std::pair<std::shared_ptr<RSPropertyBase>,
        std::shared_ptr<RSRenderPropertyBase>>> properties_;

};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_UI_ANIMATION_MANAGER_H
