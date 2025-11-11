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

#include <memory>
#include <singleton_container.h>
#include <window_adapter.h>

#include "ohos.animation.windowAnimationManager.impl.hpp"
#include "ohos.animation.windowAnimationManager.proj.hpp"
#include "taihe/runtime.hpp"
#include "taihe_rs_window_animation_controller.h"
#include "taihe_rs_window_animation_log.h"
#include "taihe_window_animation_finished_callback.h"
#include "taihe_window_animation_utils.h"
#include "window_animation_finished_callback_impl.h"

using namespace taihe;
using namespace ohos::animation::windowAnimationManager;

namespace OHOS {
namespace Rosen {
constexpr int32_t ERR_NOT_OK = 0;
constexpr int32_t ERR_NOT_SYSTEM_APP = 202;

void SetController(WindowAnimationController const& controller)
{
    TWA_LOGD("SetController");
    if (!TaiheWindowAnimationUtils::IsSystemApp()) {
        TWA_LOGE("SetController failed");
        set_business_error(ERR_NOT_SYSTEM_APP,
            "WindowAnimationManager setController failed, is not system app");
        return;
    }

    sptr<TaiheRSWindowAnimationController> taiheController = new TaiheRSWindowAnimationController();
    taiheController->SetJsController(controller);
    SingletonContainer::Get<WindowAdapter>().SetWindowAnimationController(taiheController);
    return;
}

WindowAnimationFinishedCallback OnMinimizeWindowWithAnimation(WindowAnimationTarget const& windowTarget)
{
    TWA_LOGD("OnMinimizeWindowWithAnimation");
    uint32_t windowId = 0;
    std::vector<uint32_t> windowIds = { windowId };
    sptr<RSIWindowAnimationFinishedCallback> finishedCallback;
    SingletonContainer::Get<WindowAdapter>().MinimizeWindowsByLauncher(windowIds, true, finishedCallback);
    if (finishedCallback == nullptr) {
        TWA_LOGE("Window animation finished callback is null!");
        set_business_error(ERR_NOT_OK, "Animation finished callback is null!");
    }
    return make_holder<WindowAnimationFinishedCallbackImpl, WindowAnimationFinishedCallback>(finishedCallback);
}

WindowAnimationFinishedCallback MinimizeWindowWithAnimationSync(WindowAnimationTarget const& windowTarget)
{
    TWA_LOGD("MinimizeWindowWithAnimationSync");
    if (!TaiheWindowAnimationUtils::IsSystemApp()) {
        TWA_LOGE("MinimizeWindowWithAnimationSync failed");
        set_business_error(ERR_NOT_SYSTEM_APP,
            "WindowAnimationManager minimizeWindowWithAnimation failed, is not system app");
        return make_holder<WindowAnimationFinishedCallbackImpl, WindowAnimationFinishedCallback>();
    }
    return OnMinimizeWindowWithAnimation(windowTarget);
}

} // namespace Rosen
} // namespace OHOS

TH_EXPORT_CPP_API_setController(OHOS::Rosen::SetController);
TH_EXPORT_CPP_API_MinimizeWindowWithAnimationSync(OHOS::Rosen::MinimizeWindowWithAnimationSync);
TH_EXPORT_CPP_API_MinimizeWindowWithAnimationAsync(OHOS::Rosen::MinimizeWindowWithAnimationSync);
TH_EXPORT_CPP_API_MinimizeWindowWithAnimationPromise(OHOS::Rosen::MinimizeWindowWithAnimationSync);