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

#include "window_animation_finished_callback_impl.h"
#include "taihe_rs_window_animation_log.h"

namespace OHOS {
namespace Rosen {
WindowAnimationFinishedCallbackImpl::WindowAnimationFinishedCallbackImpl(
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
    : finishedCallback_(finishedCallback)
{}

void WindowAnimationFinishedCallbackImpl::onAnimationFinish()
{
    TWA_LOGD("WindowAnimationFinishedCallbackImpl::onAnimationFinish");
    if (finishedCallback_ == nullptr) {
        return;
    }

    if (finishedCallback_.GetRefPtr()->GetSptrRefCount() == 1) {
        finishedCallback_->OnAnimationFinished();
    } else {
        finishedCallback_ = nullptr;
    }
}
} // namespace Rosen
} // namespace OHOS