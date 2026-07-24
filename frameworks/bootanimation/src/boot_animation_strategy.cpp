/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "boot_animation_strategy.h"

#include <dlfcn.h>
#include "log.h"
#include <parameters.h>
#include "util.h"
#include "transaction/rs_interfaces.h"
#include "display_manager_lite.h"

namespace OHOS {
namespace {
    constexpr const char* DUE_UPDATE_TYPE_PARAM = "persist.dupdate_engine.update_type";
    constexpr const char* HMOS_UPDATE_PARAM = "persist.update.hmos_to_next_flag";
    constexpr uint32_t WAIT_FOR_ACTIVE_SCREEN_ID_CHANGE = 1000;
    const std::string DUE_UPDATE_TYPE_MANUAL = "manual";
    const std::string DUE_UPDATE_TYPE_NIGHT = "night";
}

bool BootAnimationStrategy::CheckExitAnimation()
{
    if (!isAnimationEnd_) {
        LOGI("boot animation is end");
        if (!system::GetBoolParameter(BOOT_ANIMATION_STARTED, false)) {
            system::SetParameter(BOOT_ANIMATION_STARTED, "true");
        }
        system::SetParameter(BOOT_ANIMATION_FINISHED, "true");
        isAnimationEnd_ = true;
    }
    bool bootEventCompleted = system::GetBoolParameter(BOOT_COMPLETED, false);
    if (bootEventCompleted) {
        LOGI("read boot completed is true");
        // Notify WMS that boot animation has finished, so the fold screen power-on
        // sequence runs at the boot-animation-decided timing.
        Rosen::DisplayManagerLite::GetInstance().NotifyBootAnimationFinished();
#ifdef FEATURE_CHECK_EXIT_ANIMATION_EXT
        return CheckExitAnimationExt();
#else
        return true;
#endif
    }
    return false;
}

#ifdef FEATURE_CHECK_EXIT_ANIMATION_EXT
#if (defined(__aarch64__) || defined(__x86_64__))
#define CHECK_EXIT_ANIMATION_EXT_PATH "/system/lib64/libwatch_bootanimation_ext.z.so"
#else
#define CHECK_EXIT_ANIMATION_EXT_PATH "/system/lib/libwatch_bootanimation_ext.z.so"
#endif
#define CHECK_EXIT_ANIMATION_EXT_FUNC_NAME "CheckExitAnimationExt"
typedef bool(*Func)();
bool BootAnimationStrategy::CheckExitAnimationExt()
{
    LOGI("CheckExitAnimationExt");
    void *handler = dlopen(CHECK_EXIT_ANIMATION_EXT_PATH, RTLD_LAZY | RTLD_NODELETE);
    if (handler == nullptr) {
        LOGI("CheckExitAnimationExt Dlopen failed, reason: %{public}s", dlerror());
        return true;
    }

    Func CheckExitAnimationExtFunc = (Func)dlsym(handler, CHECK_EXIT_ANIMATION_EXT_FUNC_NAME);
    if (CheckExitAnimationExtFunc == nullptr) {
        LOGI("CheckExitAnimationExt find function failed, reason: %{public}s", dlerror());
        dlclose(handler);
        return true;
    }

    bool resCode = CheckExitAnimationExtFunc();
    dlclose(handler);
    return resCode;
}
#endif

bool BootAnimationStrategy::IsOtaUpdate() const
{
    std::string dueUpdateType = system::GetParameter(DUE_UPDATE_TYPE_PARAM, "");
    LOGI("dueUpdateType is: %{public}s", dueUpdateType.c_str());
    bool isSingleUpdate = dueUpdateType == DUE_UPDATE_TYPE_MANUAL || dueUpdateType == DUE_UPDATE_TYPE_NIGHT;
    bool isHmosUpdate = system::GetIntParameter(HMOS_UPDATE_PARAM, -1) == 1;
    LOGI("isSingleUpdate: %{public}d, isHmosUpdate: %{public}d", isSingleUpdate, isHmosUpdate);
    return isSingleUpdate || isHmosUpdate;
}

void BootAnimationStrategy::GetConnectToRenderMap(int count)
{
    LOGI("BootAnimationStrategy::%{public}s set screen change callback start.", __func__);
    auto cv = std::make_shared<std::condition_variable>();
    std::weak_ptr<BootAnimationStrategy> weakThis = shared_from_this();
    Rosen::RSInterfaces::GetInstance().SetScreenChangeCallback(
        [cv, weakThis](Rosen::ScreenId rsScreenId, Rosen::ScreenEvent screenEvent,
            Rosen::ScreenChangeReason reason, sptr<IRemoteObject> connectToRender) {
            auto sharedThis = weakThis.lock();
            if (!sharedThis) {
                LOGE("BootAnimationStrategy::GetConnectToRenderMap shared this is null.");
                return;
            }
            sharedThis->OnScreenChanged(rsScreenId, screenEvent, reason, connectToRender);
            cv->notify_all();
        });
    {
        std::unique_lock<std::mutex> lock(connectToRenderMapMtx_);
        LOGI("GetConnectToRenderMap start infinite waiting for %{public}d screens.", count);
        cv->wait(lock, [this, count]() {
            return this->connectToRenderMap_.size() >= static_cast<size_t>(count) || noScreen_.load();
        });
        LOGI("GetConnectToRenderMap wait finished. Currently got %{public}zu.", this->connectToRenderMap_.size());
    }
    LOGI("GetConnectToRenderMap %{public}s set screen change callback end.", __func__);
}

void BootAnimationStrategy::OnScreenChanged(Rosen::ScreenId rsScreenId, Rosen::ScreenEvent screenEvent,
                                            Rosen::ScreenChangeReason reason, sptr<IRemoteObject> connectToRender)
{
    if (rsScreenId == Rosen::INVALID_SCREEN_ID) {
        noScreen_ = true;
        return;
    }
    {
        std::lock_guard<std::mutex> lock(connectToRenderMapMtx_);
        if (screenEvent == Rosen::ScreenEvent::CONNECTED) {
            LOGI("BootAnimationStrategy::%{public}s Screen connected:" BPUBU64 "", __func__, rsScreenId);
            connectToRenderMap_.emplace(rsScreenId, connectToRender);
        }
        if (screenEvent == Rosen::ScreenEvent::DISCONNECTED) {
            LOGI("BootAnimationStrategy::%{public}s Screen disconnected:" BPUBU64 "", __func__, rsScreenId);
            connectToRenderMap_.erase(rsScreenId);
        }
    }
}

void BootAnimationStrategy::SubscribeActiveScreenIdChanged()
{
    LOGI("BootAnimationStrategy::%{public}s get active screen id start.", __func__);
    auto cv = std::make_shared<std::condition_variable>();
    std::weak_ptr<BootAnimationStrategy> weakThis = shared_from_this();
    Rosen::RSInterfaces::GetInstance().SetActiveScreenIdChangedCallback(
        [cv, weakThis](Rosen::ScreenId changedActiveScreenId) {
            auto sharedThis = weakThis.lock();
            if (!sharedThis) {
                LOGE("BootAnimationStrategy::Subscribe... shared this is null, screenId:" BPUBU64 "",
                     changedActiveScreenId);
                return;
            }
            LOGI("BootAnimationStrategy::Subscribe... active screen id changed to " BPUBU64 "",
                 changedActiveScreenId);
            {
                std::lock_guard<std::mutex> lock(sharedThis->activeScreenIdMtx_);
                sharedThis->activeScreenId_ = changedActiveScreenId;
            }
            cv->notify_all();
        });
    {
        std::unique_lock<std::mutex> lock(activeScreenIdMtx_);
        bool success = cv->wait_for(lock,
            std::chrono::milliseconds(WAIT_FOR_ACTIVE_SCREEN_ID_CHANGE),
            [weakThis]() {
                auto sharedThis = weakThis.lock();
                if (!sharedThis) {
                    LOGE("SubscribeActiveScreenIdChanged shared this is null during wait.");
                    return true;
                }
                return sharedThis->activeScreenId_ != Rosen::INVALID_SCREEN_ID;
            });
        if (!success) {
            LOGI("BootAnimationStrategy::%{public}s wait for active screenId timeout.", __func__);
        }
    }
    LOGI("BootAnimationStrategy::%{public}s get active screen id end", __func__);
}

Rosen::ScreenId BootAnimationStrategy::GetActiveScreenId()
{
    std::unique_lock<std::mutex> lock(activeScreenIdMtx_);
    return activeScreenId_;
}
} // namespace OHOS
