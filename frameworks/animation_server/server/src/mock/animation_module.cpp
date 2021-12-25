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

#include "animation_module.h"

namespace OHOS {
GSError AnimationModule::Init()
{
    return GSERROR_NOT_SUPPORT;
}

GSError AnimationModule::StartRotationAnimation(int32_t did, int32_t degree)
{
    return GSERROR_NOT_SUPPORT;
}

void AnimationModule::OnScreenShot(const struct WMImageInfo &info)
{
}

void AnimationModule::StartAnimation(struct Animation &animation)
{
}

void AnimationModule::AnimationSync(int64_t time, void *data)
{
}
} // namespace OHOS
