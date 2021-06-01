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

#ifndef FRAMEWORKS_VSYNC_INCLUDE_VSYNC_HELPER_IMPL_H
#define FRAMEWORKS_VSYNC_INCLUDE_VSYNC_HELPER_IMPL_H

#include <mutex>
#include <queue>
#include <memory>

#include <vsync_helper.h>

#include "ivsync_manager.h"
#include "vsync_manager_proxy.h"
#include "vsync_callback_stub.h"

namespace OHOS {
namespace {
class VsyncCallback;
}

class VsyncHelperImpl : public VsyncHelper {
    friend class VsyncCallback;

public:
    static sptr<VsyncHelperImpl> Current();

    VsyncHelperImpl(std::shared_ptr<AppExecFwk::EventHandler>& handler);
    virtual ~VsyncHelperImpl();

    VsyncError RequestFrameCallback(struct FrameCallback& cb);

protected:
    virtual VsyncError InitSA();

    VsyncError InitSA(int32_t vsyncSystemAbilityId);

private:
    VsyncError Init();

    void DispatchFrameCallback(int64_t timestamp);
    void RequestNextVsync();

    bool isVsyncRequested = false;
    std::mutex callbacks_mutex_;
    std::priority_queue<struct FrameCallback> callbacks_;

    std::shared_ptr<AppExecFwk::EventHandler> handler_;
    sptr<IVsyncManager> service_;
    static thread_local sptr<VsyncHelperImpl> currentHelper;
};

namespace {
class VsyncCallback : public VsyncCallbackStub {
public:
    VsyncCallback(sptr<VsyncHelperImpl>& helper);
    virtual ~VsyncCallback();

    void OnVsync(int64_t timestamp) override;

private:
    sptr<VsyncHelperImpl> helper_;
};
}
} // namespace OHOS

#endif // FRAMEWORKS_VSYNC_INCLUDE_VSYNC_HELPER_IMPL_H
