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


#ifndef VSYNC_VSYNC_DISTRIBUTOR_H
#define VSYNC_VSYNC_DISTRIBUTOR_H

#include <refbase.h>

#include <mutex>
#include <vector>
#include <thread>
#include <condition_variable>

#include "local_socketpair.h"
#include "vsync_controller.h"
#include "vsync_connection_stub.h"

namespace OHOS {
namespace Rosen {
class VSyncDistributor;
class VSyncConnection : public VSyncConnectionStub {
public:

    VSyncConnection(const sptr<VSyncDistributor>& distributor, std::string name);
    ~VSyncConnection();

    virtual VsyncError RequestNextVSync() override;
    virtual VsyncError GetReceiveFd(int32_t &fd) override;
    virtual VsyncError SetVSyncRate(int32_t rate) override;

    int32_t PostEvent(int64_t now);
    std::string GetName() const
    {
        return name_;
    }

    int32_t rate_;
private:
    // Circular reference， need check
    wptr<VSyncDistributor> distributor_;
    sptr<LocalSocketPair> socketPair_;
    std::string name_;
};

class VSyncDistributor : public RefBase, public VSyncController::Callback {
public:

    VSyncDistributor(sptr<VSyncController> controller, std::string name);
    ~VSyncDistributor();
    // nocopyable
    VSyncDistributor(const VSyncDistributor &) = delete;
    VSyncDistributor &operator=(const VSyncDistributor &) = delete;

    void AddConnection(const sptr<VSyncConnection>& connection);
    void RequestNextVSync(const sptr<VSyncConnection>& connection);
    VsyncError SetVSyncRate(int32_t rate, const sptr<VSyncConnection>& connection);

private:

    // check, add more info
    struct VSyncEvent {
        int64_t timestamp;
        int64_t vsyncCount;
    };
    void ThreadMain();
    void RemoveConnection(const sptr<VSyncConnection> &connection);
    void EnableVSync();
    void DisableVSync();
    void OnVSyncEvent(int64_t now);

    std::thread threadLoop_;
    sptr<VSyncController> controller_;
    std::mutex mutex_;
    std::condition_variable con_;
    std::vector<sptr<VSyncConnection> > connections_;
    VSyncEvent event_;
    bool vsyncEnabled_;
    std::string name_;
    bool vsyncThreadRunning_;
};
} // namespace Rosen
} // namespace OHOS

#endif
