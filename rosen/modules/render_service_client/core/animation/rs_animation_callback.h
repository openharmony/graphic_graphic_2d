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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_CALLBACK_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_CALLBACK_H

#include <functional>

#include "animation/rs_animation_timing_protocol.h"

namespace OHOS {
namespace Rosen {
class AnimationCallback {
public:
    explicit AnimationCallback(const std::function<void()>& callback);
    virtual ~AnimationCallback();

protected:
    std::function<void()> callback_;
};

class AnimationFinishCallback : public AnimationCallback {
public:
    AnimationFinishCallback(const std::function<void()>& callback,
        FinishCallbackType finishCallbackType = FinishCallbackType::TIME_SENSITIVE);
    ~AnimationFinishCallback() override = default;
    // Execute the callback function immediately.
    void Execute();

    const FinishCallbackType finishCallbackType_;
};

class AnimationRepeatCallback {
public:
    AnimationRepeatCallback(const std::function<void()>& callback) : callback_(callback) {};
    ~AnimationRepeatCallback() {};
    // Execute the callback function repeatitive.
    void Execute();

protected:
    std::function<void()> callback_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_CALLBACK_H
