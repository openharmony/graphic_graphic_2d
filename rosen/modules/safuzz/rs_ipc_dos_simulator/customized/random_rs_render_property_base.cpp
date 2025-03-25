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

#include "customized/random_rs_render_property_base.h"

#include <functional>

#include "modifier/rs_render_property.h"
#include "property/rs_properties_def.h"
#include "random/random_engine.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSRenderPropertyBase> RandomRSRenderPropertyBase::GetRandomRSRenderPropertyBase()
{
    static const std::vector<std::function<std::shared_ptr<RSRenderPropertyBase>()>> randomGenerator = {
#define DECLARE_RENDER_PROPERTY(TYPE, RANDOM_FACTORY, FUNC_NAME) \
    RandomRSRenderPropertyBase::GetRandom##FUNC_NAME##PropertyBase,
#define DECLARE_ANIMATABLE_RENDER_PROPERTY(TYPE, RANDOM_FACTORY, FUNC_NAME) \
    RandomRSRenderPropertyBase::GetRandomAnimatable##FUNC_NAME##PropertyBase,

#include "customized/rs_render_property.in"

#undef DECLARE_RENDER_PROPERTY
#undef DECLARE_ANIMATABLE_RENDER_PROPERTY
    };

    int index = RandomEngine::GetRandomIndex(randomGenerator.size() - 1);
    auto modifier = randomGenerator[index]();
    return modifier;
}

#define DECLARE_RENDER_PROPERTY(TYPE, RANDOM_FACTORY, FUNC_NAME)                           \
template<>                                                                                 \
std::shared_ptr<RSRenderPropertyBase> RandomRSRenderPropertyBase::GetRandom<TYPE, false>() \
{                                                                                          \
    return RandomRSRenderPropertyBase::GetRandom##FUNC_NAME##PropertyBase();               \
}

#define DECLARE_ANIMATABLE_RENDER_PROPERTY(TYPE, RANDOM_FACTORY, FUNC_NAME)                \
template<>                                                                                 \
std::shared_ptr<RSRenderPropertyBase> RandomRSRenderPropertyBase::GetRandom<TYPE, true>()  \
{                                                                                          \
    return RandomRSRenderPropertyBase::GetRandomAnimatable##FUNC_NAME##PropertyBase();     \
}

#include "customized/rs_render_property.in"

#undef DECLARE_RENDER_PROPERTY
#undef DECLARE_ANIMATABLE_RENDER_PROPERTY
} // namespace Rosen
} // namespace OHOS
