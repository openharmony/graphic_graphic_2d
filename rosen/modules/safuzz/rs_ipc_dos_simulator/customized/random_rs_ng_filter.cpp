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
#include "customized/random_rs_render_property_base.h"
#include "random/random_data.h"
#include "random/random_engine.h"

namespace OHOS {
namespace Rosen {
namespace {
bool g_isInit = false;
std::vector<std::function<std::shared_ptr<RSNGRenderFilterBase>()>> g_randomFilterGenerator;
}

std::shared_ptr<RSNGRenderFilterBase> RandomRSNGFilterPtr::GetRandomValue()
{
    if (!g_isInit) {
        g_randomFilterGenerator.push_back(RandomRSNGFilterPtr::GetNullValue);
#define DECLARE_FILTER(FilterName, FilterType, ...)     \
    g_randomFilterGenerator.push_back(RandomRSNGFilterPtr::GetRandom##FilterName)

#include "effect/rs_render_filter_def.in"

#undef DECLARE_FILTER
        g_isInit = true;
    }

    bool generateChain = RandomData::GetRandomBool();
    if (generateChain) {
        return RandomRSNGFilterPtr::GetRandomFilterChain();
    }

    return RandomRSNGFilterPtr::GetRandomSingleFilter();
}

std::shared_ptr<RSNGRenderFilterBase> RandomRSNGFilterPtr::GetNullValue()
{
    return nullptr;
}

std::shared_ptr<RSNGRenderFilterBase> RandomRSNGFilterPtr::GetRandomSingleFilter()
{
    int index = RandomEngine::GetRandomIndex(g_randomFilterGenerator.size() - 1);
    return g_randomFilterGenerator[index]();
}

std::shared_ptr<RSNGRenderFilterBase> RandomRSNGFilterPtr::GetRandomFilterChain()
{
    std::shared_ptr<RSNGRenderFilterBase> head = nullptr;
    auto current = head;
    int filterChainSize = RandomEngine::GetRandomSmallVectorLength();
    for (int i = 0; i < filterChainSize; ++i) {
        auto filter = GetRandomSingleFilter();
        if (!filter) {
            continue;
        }

        if (!current) {
            head = filter; // init head
        } else {
            current->nextEffect_ = filter;
        }
        current = filter;
    }

    return head;
}

#define ADD_PROPERTY_TAG(Effect, Prop) \
    value->Setter<Effect##Prop##RenderTag>( \
        RandomRSRenderPropertyBase::GetRandomValue<typename Effect##Prop##RenderTag::ValueType>())
#define DECLARE_FILTER(FilterName, FilterType, ...)                                 \
std::shared_ptr<RSNGRenderFilterBase> RandomRSNGFilterPtr::GetRandom##FilterName()  \
{                                                                                   \
    auto value = std::make_shared<RSNGRender##FilterName##Filter>();                \
    __VA_ARGS__;                                                                    \
    return value;                                                                   \
}

#include "effect/rs_render_filter_def.in"

#undef ADD_PROPERTY_TAG
#undef DECLARE_FILTER

} // namespace Rosen
} // namespace OHOS
