/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "anim_dynamic_configs.h"
#include "hgm_log.h"

namespace OHOS::Rosen {
AnimDynamicConfigs* AnimDynamicConfigs::GetInstance()
{
    static AnimDynamicConfigs instance;
    return &instance;
}

AnimDynamicConfigs::AnimDynamicConfigs()
{
}

AnimDynamicConfigs::~AnimDynamicConfigs()
{
}

std::vector<AnimDynamicAttribute> AnimDynamicConfigs::GetAnimDynamicConfigs()
{
    return animDynamicCfgs_;
}

void AnimDynamicConfigs::AddAnimDynamicAttribute(AnimDynamicAttribute attr)
{
    animDynamicCfgs_.emplace_back(attr);
}

int AnimDynamicConfigs::GetAnimDynamicAttributesCount()
{
    return animDynamicCfgs_.size();
}
} // namespace OHOS::Rosen