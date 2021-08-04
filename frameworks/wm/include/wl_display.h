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

#ifndef FRAMEWORKS_WM_INCLUDE_WL_DISPLAY_H
#define FRAMEWORKS_WM_INCLUDE_WL_DISPLAY_H

#include <functional>
#include <map>
#include <mutex>

#include <promise.h>
#include <refbase.h>
#include <wayland-client-protocol.h>

#include "singleton_delegator.h"

namespace OHOS {
using DispatchDeathFunc = std::function<void()>;

class WlDisplay : public RefBase {
public:
    static sptr<WlDisplay> GetInstance();

    MOCKABLE bool Connect(const char *name = nullptr);
    MOCKABLE void Disconnect();

    MOCKABLE struct wl_display *GetRawPtr() const;
    MOCKABLE int32_t GetFd() const;
    MOCKABLE int32_t GetError() const;

    MOCKABLE int32_t Flush();
    MOCKABLE int32_t Dispatch();
    MOCKABLE int32_t Roundtrip();

    MOCKABLE void Sync();

    MOCKABLE void StartDispatchThread();
    MOCKABLE void StopDispatchThread();
    MOCKABLE int32_t AddDispatchDeathListener(DispatchDeathFunc func);
    MOCKABLE void RemoveDispatchDeathListener(int32_t deathListener);

protected:
    void DispatchThreadMain();
    void InterruptDispatchThread();

    int32_t PrepareRead();
    int32_t DispatchPending();
    void CancelRead();
    int32_t ReadEvents();

    static void SyncDone(void *donePromise, struct wl_callback *, uint32_t data);

private:
    WlDisplay();
    MOCKABLE ~WlDisplay() override;
    static inline sptr<WlDisplay> instance = nullptr;
    static inline SingletonDelegator<WlDisplay> delegator;

    struct wl_display *display = nullptr;

    std::map<int32_t, DispatchDeathFunc> dispatchDeathFuncs;
    std::mutex dispatchDeathFuncsMutex;

    std::unique_ptr<std::thread> dispatchThread = nullptr;
    int32_t interruptFd = -1;
    std::unique_ptr<std::once_flag> startOnceFlag = nullptr;
    sptr<Promise<bool>> startPromise = nullptr;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_WL_DISPLAY_H
