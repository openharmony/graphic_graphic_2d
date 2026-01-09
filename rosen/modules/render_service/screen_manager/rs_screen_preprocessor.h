/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#ifndef RS_SCREEN_PREPROCESSOR_H
#define RS_SCREEN_PREPROCESSOR_H

#include <memory>
#include <vector>
#include <event_handler.h>
#include <hdi_backend.h>

#include "callback/rs_screen_callback_manager.h"

namespace OHOS {
namespace Rosen {
class RSScreenManager;
struct ScreenHotPlugEvent {
    std::shared_ptr<HdiOutput> output;
    bool connected = false;
};
class RSScreenPreprocessor {
public:
    RSScreenPreprocessor(RSScreenManager& screenManager, RSScreenCallbackManager& callbackMgr,
        std::shared_ptr<AppExecFwk::EventHandler> handler, bool isFoldScreen);
    ~RSScreenPreprocessor() {};

    static void OnHotPlug(std::shared_ptr<HdiOutput>& output, bool connected, void* data);
    static void OnRefresh(ScreenId id, void* data);
    static void OnHwcDead(void* data);
    static void OnHwcEvent(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData, void* data);
    static void OnScreenVBlankIdle(uint32_t devId, uint64_t ns, void* data);

    bool Init() noexcept;
    void NotifyVirtualScreenConnected(ScreenId newId, ScreenId associatedScreenId, sptr<RSScreenProperty> property);
    void NotifyVirtualScreenDisconnected(ScreenId id);
    void NotifyActiveScreenIdChanged(ScreenId activeScreenId);
    void NotifyScreenPropertyChanged(ScreenId id, ScreenPropertyType type, const sptr<ScreenPropertyBase>& property);

private:
    void OnHotPlugEvent(std::shared_ptr<HdiOutput>& output, bool connected);
    void OnRefreshEvent(ScreenId id);
    void OnHwcDeadEvent();
    void OnHwcEventCallback(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData);
    void OnScreenVBlankIdleEvent(uint32_t devId, uint64_t ns);

    void ConfigureScreenConnected(std::shared_ptr<HdiOutput>& output);
    void ConfigureScreenDisconnected(std::shared_ptr<HdiOutput>& output);

    void ProcessScreenHotPlugEvents();
    void ScheduleTask(std::function<void()> task);

    std::atomic<bool> isHwcDead_ = false;
    HdiBackend* composer_ = nullptr;
    RSScreenManager& screenManager_;
    RSScreenCallbackManager& callbackMgr_;
    std::shared_ptr<AppExecFwk::EventHandler> mainHandler_;
    std::map<ScreenId, ScreenHotPlugEvent> pendingHotPlugEvents_;
    mutable std::mutex hotPlugMutex_;
    bool isFoldScreenFlag_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_SCREEN_PREPROCESSOR_H