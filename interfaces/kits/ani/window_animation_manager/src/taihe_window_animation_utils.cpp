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

#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "taihe/runtime.hpp"
#include "taihe_window_animation_utils.h"
#include "window_animation_finished_callback_impl.h"

using namespace taihe;
using namespace ohos::animation::windowAnimationManager;

namespace OHOS {
namespace Rosen {
WindowAnimationTarget TaiheWindowAnimationUtils::CreateJsWindowAnimationTarget(
    const sptr<RSWindowAnimationTarget>& target)
{
    ohos::animation::windowAnimationManager::RRect jsRRect {
        .left = target->windowBounds_.rect_.left_,
        .top = target->windowBounds_.rect_.top_,
        .width = target->windowBounds_.rect_.width_,
        .height = target->windowBounds_.rect_.height_,
        .radius = target->windowBounds_.radius_[0].x_,
    };
    WindowAnimationTarget jsTarget {
        .bundleName = target->bundleName_,
        .abilityName = target->abilityName_,
        .windowBounds = jsRRect,
        .missionId = target->missionId_,
        .windowId = target->windowId_,
    };
    return jsTarget;
}

WindowAnimationFinishedCallback TaiheWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    return make_holder<WindowAnimationFinishedCallbackImpl,
        WindowAnimationFinishedCallback>(finishedCallback);
}

bool TaiheWindowAnimationUtils::IsSystemApp()
{
    uint64_t tokenId = OHOS::IPCSkeleton::GetCallingFullTokenID();
    return Security::AccessToken::AccessTokenKit::IsSystemAppByFullTokenID(tokenId);
}
} // namespace Rosen
} // namespace OHOS