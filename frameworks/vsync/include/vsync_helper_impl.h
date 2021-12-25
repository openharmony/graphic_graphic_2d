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

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <queue>

#include <graphic_dumper_helper.h>
#include <refbase.h>
#include <vsync_helper.h>

#include "ivsync_manager.h"
#include "vsync_callback_stub.h"

namespace OHOS {
namespace Vsync {
struct VsyncElement {
    SyncFunc callback_;
    int64_t activeTime_;
    void *userdata_;
    uint32_t frequency_;

    bool operator <(const struct VsyncElement &other) const
    {
        return activeTime_ < other.activeTime_;
    }
};

class VsyncClient : public RefBase {
public:
    static sptr<VsyncClient> GetInstance();

    virtual GSError Init(bool restart = false);

    GSError RequestFrameCallback(const struct FrameCallback &cb);
    GSError GetSupportedVsyncFrequencys(std::vector<uint32_t>& freqs);

    virtual void DispatchFrameCallback(int64_t timestamp);

private:
    VsyncClient() = default;
    virtual ~VsyncClient() = default;
    static inline sptr<VsyncClient> instance = nullptr;

    GSError InitService();
    GSError InitVsyncFrequency();
    GSError InitListener();

    void DispatchMain(int64_t timestamp);
    void OnDump();

    std::map<uint32_t, std::priority_queue<struct VsyncElement>> callbacksMap_;
    std::mutex callbacksMapMutex_;

    std::atomic<uint32_t> lastID_ = 0;

    uint32_t vsyncFrequency_ = 0;
    sptr<IVsyncManager> service_ = nullptr;
    std::mutex serviceMutex_;
    sptr<IVsyncCallback> listener_ = nullptr;
    sptr<GraphicDumperHelper> dumper = nullptr;
    int32_t dumpListener = -1;
};

class VsyncHelperImpl : public VsyncHelper {
public:
    static sptr<VsyncHelperImpl> Current();

    VsyncHelperImpl(std::shared_ptr<AppExecFwk::EventHandler>& handler);
    virtual ~VsyncHelperImpl() override;

    virtual GSError RequestFrameCallback(const struct FrameCallback &cb) override;
    virtual GSError GetSupportedVsyncFrequencys(std::vector<uint32_t>& freqs) override;

private:
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    static inline thread_local sptr<VsyncHelperImpl> currentHelper_ = nullptr;
};

class VsyncCallback : public VsyncCallbackStub {
public:
    virtual GSError OnVsync(int64_t timestamp) override;
};

class VsyncManagerDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    VsyncManagerDeathRecipient() = default;
    ~VsyncManagerDeathRecipient() = default;
    void OnRemoteDied(const wptr<IRemoteObject> &remote);
};
} // namespace Vsync
} // namespace OHOS

#endif // FRAMEWORKS_VSYNC_INCLUDE_VSYNC_HELPER_IMPL_H
