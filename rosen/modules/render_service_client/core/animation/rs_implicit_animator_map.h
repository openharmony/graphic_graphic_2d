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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_IMPLICIT_ANIMATOR_MAP_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_IMPLICIT_ANIMATOR_MAP_H

#include <mutex>
#include <unordered_map>

namespace OHOS {
namespace Rosen {
class RSImplicitAnimator;

class RSImplicitAnimatorMap final {
public:
    static RSImplicitAnimatorMap& Instance();

    const std::shared_ptr<RSImplicitAnimator> GetAnimator(const int32_t id);

private:
    RSImplicitAnimatorMap() = default;
    ~RSImplicitAnimatorMap() = default;
    RSImplicitAnimatorMap(const RSImplicitAnimatorMap&) = delete;
    RSImplicitAnimatorMap(const RSImplicitAnimatorMap&&) = delete;
    RSImplicitAnimatorMap& operator=(const RSImplicitAnimatorMap&) = delete;
    RSImplicitAnimatorMap& operator=(const RSImplicitAnimatorMap&&) = delete;

    std::mutex mutex_;
    std::unordered_map<int32_t, std::shared_ptr<RSImplicitAnimator>> animatorMap_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_IMPLICIT_ANIMATOR_MAP_H
