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

#include "animation/rs_implicit_animator_map.h"

#include "animation/rs_implicit_animator.h"

namespace OHOS {
namespace Rosen {
RSImplicitAnimatorMap& RSImplicitAnimatorMap::Instance()
{
    static RSImplicitAnimatorMap animatorMap;
    return animatorMap;
}

const std::shared_ptr<RSImplicitAnimator>& RSImplicitAnimatorMap::GetAnimator(const int32_t id)
{
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = animatorMap_.find(id);
    if (iter == animatorMap_.end()) {
        auto animator = std::make_shared<RSImplicitAnimator>();
        animatorMap_.emplace(id, animator);
        return animator;
    } else {
        return iter->second;
    }
}
} // namespace Rosen
} // namespace OHOS
