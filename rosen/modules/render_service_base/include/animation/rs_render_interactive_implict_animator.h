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
#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_INTERACTIVE_IMPLICT_ANIMATOR_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_INTERACTIVE_IMPLICT_ANIMATOR_H

#include <refbase.h>
#include <vector>

#include "animation/rs_animation_common.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSContext;

class RSB_EXPORT RSRenderInteractiveImplictAnimator :
    public std::enable_shared_from_this<RSRenderInteractiveImplictAnimator> {
public:
    explicit RSRenderInteractiveImplictAnimator
        (InteractiveImplictAnimatorId id, const std::weak_ptr<RSContext>& context = {});
    ~RSRenderInteractiveImplictAnimator() = default;

    void AddAnimations(std::vector<std::pair<NodeId, AnimationId>> animations);
    void PauseAnimator();
    void ContinueAnimator();
    void FinishAnimator(RSInteractiveAnimationPosition finishPos);
    void ReverseAnimator();
    void SetFractionAnimator(float fraction);

    inline InteractiveImplictAnimatorId GetId() const
    {
        return id_;
    }
private:
    RSRenderInteractiveImplictAnimator() = default;
    InteractiveImplictAnimatorId id_ = 0;
    std::weak_ptr<RSContext> context_ = {};
    std::vector<std::pair<NodeId, AnimationId>> animations_ = {};

    friend class RSContext;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_INTERACTIVE_IMPLICT_ANIMATOR_H
