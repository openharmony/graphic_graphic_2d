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

#ifndef INTERFACES_KITS_ANI_WINDOW_ANIMATION_FINISHED_CALLBACK_IMPL_H
#define INTERFACES_KITS_ANI_WINDOW_ANIMATION_FINISHED_CALLBACK_IMPL_H

#include <functional>
#include <rs_window_animation_finished_callback.h>

#include "ohos.animation.windowAnimationManager.impl.hpp"
#include "ohos.animation.windowAnimationManager.proj.hpp"
#include "taihe/runtime.hpp"

namespace OHOS {
namespace Rosen {
class WindowAnimationFinishedCallbackImpl {
public:
    WindowAnimationFinishedCallbackImpl() {};
    explicit WindowAnimationFinishedCallbackImpl(const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback);
    ~WindowAnimationFinishedCallbackImpl() = default;

    void onAnimationFinish();

private:
    sptr<RSIWindowAnimationFinishedCallback> finishedCallback_;
};

} // namespace Rosen
} // namespace OHOS

#endif // INTERFACES_KITS_ANI_WINDOW_ANIMATION_FINISHED_CALLBACK_IMPL_H