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

#include "animation/rs_render_interactive_implict_animator_map.h"
#include "animation/rs_render_interactive_implict_animator.h"

namespace OHOS {
namespace Rosen {
using AnimatorPtr = std::shared_ptr<RSRenderInteractiveImplictAnimator>;
RSRenderInteractiveImplictAnimatorMap::RSRenderInteractiveImplictAnimatorMap()
{
}

bool RSRenderInteractiveImplictAnimatorMap::RegisterInteractiveImplictAnimator(const AnimatorPtr& animatorPtr)
{
    auto id = animatorPtr->GetId();
    if (interactiveImplictAnimatorMap_.count(id)) {
        return false;
    }
    interactiveImplictAnimatorMap_.emplace(id, animatorPtr);
    return true;
}

void RSRenderInteractiveImplictAnimatorMap::UnregisterInteractiveImplictAnimator(InteractiveImplictAnimatorId id)
{
    interactiveImplictAnimatorMap_.erase(id);
}

AnimatorPtr RSRenderInteractiveImplictAnimatorMap::GetInteractiveImplictAnimator(InteractiveImplictAnimatorId id)
{
    return interactiveImplictAnimatorMap_.count(id) ? interactiveImplictAnimatorMap_[id] : nullptr;
}
} // namespace Rosen
} // namespace OHOS
