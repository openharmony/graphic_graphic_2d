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

#ifndef FRAMEWORKS_WMSERVICE_INCLUDE_WINDOW_MANAGER_SERVICE_CLIENT_IMPL_H
#define FRAMEWORKS_WMSERVICE_INCLUDE_WINDOW_MANAGER_SERVICE_CLIENT_IMPL_H

#include <memory>
#include <mutex>
#include <thread>

#include <promise.h>
#include <window_manager_service_client.h>
#include <wms-client-protocol.h>

namespace OHOS {
class WindowManagerServiceClientImpl : public WindowManagerServiceClient {
public:
    static sptr<WindowManagerServiceClientImpl> GetInstance();

    virtual WMError Init() override;
    virtual WMError Deinit() override;
    virtual sptr<IWindowManagerService> GetService() const override;

private:
    WindowManagerServiceClientImpl() = default;
    virtual ~WindowManagerServiceClientImpl() = default;
    static inline sptr<WindowManagerServiceClientImpl> instance = nullptr;

    void StartDispatchThread();
    void StopDispatchThread();
    void DispatchThreadMain();
    void InterruptDispatchThread();
    int32_t interruptFd = -1;
    std::unique_ptr<std::once_flag> startOnceFlag = nullptr;
    sptr<Promise<bool>> startPromise = nullptr;

    struct wms *wms = nullptr;
    struct wl_display *display = nullptr;
    struct wl_registry *registry = nullptr;
    sptr<IWindowManagerService> wmservice;
    std::unique_ptr<std::thread> dispatchThread;
};
} // namespace OHOS

#endif // FRAMEWORKS_WMSERVICE_INCLUDE_WINDOW_MANAGER_SERVICE_CLIENT_IMPL_H
