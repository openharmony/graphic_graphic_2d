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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_GROUP_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_GROUP_H

#include <functional>
#include <list>
#include <memory>

#include "animation/rs_animatable_property.h"
#include "animation/rs_animation.h"
#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSNode;
class AnimationCallback;

class RS_EXPORT RSAnimationGroup : public RSAnimation {
public:
    RSAnimationGroup() = default;
    ~RSAnimationGroup() = default;

    void AddAnimation(const std::shared_ptr<RSAnimation>& animation);
    void RemoveAnimation(const std::shared_ptr<RSAnimation>& animation);

protected:
    void OnStart() override;
    void OnPause() override;
    void OnResume() override;
    void OnFinish() override;
    void OnReverse() override;
    void OnSetFraction(float fraction) override;
    RSAnimatableProperty GetProperty() const override;

    std::list<std::shared_ptr<RSAnimation>> animations_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_GROUP_H
