/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "customized/random_rs_ng_filter.h"

#include "random/random_data.h"
#include "random/random_engine.h"


namespace OHOS {
namespace Rosen {

static const std::vector<std::function<std::shared_ptr<RSNGRenderFilterBase>()>> filterRandomGenerator = {
    RandomRSNGFilterPtr::GetRandomDispDistortFilter,
};

std::shared_ptr<RSNGRenderFilterBase> RandomRSNGFilterPtr::GetRandomValue()
{
    bool generateChain = RandomData::GetRandomBool();
    if (generateChain) {
        return RandomRSNGFilterPtr::GetRandomFilterChain();
    }

    return RandomRSNGFilterPtr::GetRandomSingleFilter();
}

std::shared_ptr<RSNGRenderFilterBase> RandomRSNGFilterPtr::GetRandomSingleFilter()
{
    int index = RandomEngine::GetRandomIndex(filterRandomGenerator.size() - 1);
    return filterRandomGenerator[index]();
}

std::shared_ptr<RSNGRenderFilterBase> RandomRSNGFilterPtr::GetRandomFilterChain()
{
    std::shared_ptr<RSNGRenderFilterBase> head = nullptr;
    int filterChainSize = RandomEngine::GetRandomSmallVectorLength();
    for (int i = 0; i < filterChainSize; ++i) {
        auto filter = GetRandomSingleFilter();
        if (!head) {
            head = filter;
        } else {
            head->nextEffect_ = filter;
        }
    }

    return head;
}

std::shared_ptr<RSNGRenderFilterBase> RandomRSNGFilterPtr::GetRandomDispDistortFilter()
{
    auto filter = std::make_shared<RSNGRenderDispDistortFilter>();
    filter->Setter<DispDistortFactorRenderTag>(RandomData::GetRandomVector2f());
    return filter;
}
} // namespace Rosen
} // namespace OHOS
