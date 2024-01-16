/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "animation/rs_animation_callback.h"

namespace OHOS {
namespace Rosen {
AnimationCallback::AnimationCallback(const std::function<void()>&& callback) : callback_(std::move(callback)) {}

AnimationCallback::~AnimationCallback()
{
    if (callback_ != nullptr) {
        callback_();
    }
}

AnimationFinishCallback::AnimationFinishCallback(
    std::function<void()> callback, Rosen::FinishCallbackType finishCallbackType)
    : AnimationCallback(std::move(callback)), finishCallbackType_(finishCallbackType)
{}

void AnimationFinishCallback::Execute()
{
    if (callback_ != nullptr) {
        callback_();
        // avoid callback_ being called repeatedly.
        callback_ = nullptr;
    }
}

void AnimationRepeatCallback::Execute()
{
    if (callback_ != nullptr) {
        callback_();
    }
}
} // namespace Rosen
} // namespace OHOS
