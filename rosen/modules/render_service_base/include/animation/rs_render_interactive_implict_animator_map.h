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
#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_INTERACTIVE_IMPLICT_ANIMATOR_MAP_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_INTERACTIVE_IMPLICT_ANIMATOR_MAP_H

#include <unordered_map>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSRenderInteractiveImplictAnimator;
class RSB_EXPORT RSRenderInteractiveImplictAnimatorMap final {
public:
    bool RegisterInteractiveImplictAnimator(const std::shared_ptr<RSRenderInteractiveImplictAnimator>& animatorPtr);
    void UnregisterInteractiveImplictAnimator(InteractiveImplictAnimatorId id);
    std::shared_ptr<RSRenderInteractiveImplictAnimator> GetInteractiveImplictAnimator(
        InteractiveImplictAnimatorId id);

private:
    explicit RSRenderInteractiveImplictAnimatorMap();
    ~RSRenderInteractiveImplictAnimatorMap() = default;
    RSRenderInteractiveImplictAnimatorMap(const RSRenderInteractiveImplictAnimatorMap&) = delete;
    RSRenderInteractiveImplictAnimatorMap(const RSRenderInteractiveImplictAnimatorMap&&) = delete;
    RSRenderInteractiveImplictAnimatorMap& operator=(const RSRenderInteractiveImplictAnimatorMap&) = delete;
    RSRenderInteractiveImplictAnimatorMap& operator=(const RSRenderInteractiveImplictAnimatorMap&&) = delete;

private:
    std::unordered_map<InteractiveImplictAnimatorId,
        std::shared_ptr<RSRenderInteractiveImplictAnimator>> interactiveImplictAnimatorMap_;

    friend class RSContext;
    friend class RSMainThread;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_INTERACTIVE_IMPLICT_ANIMATOR_MAP_H
