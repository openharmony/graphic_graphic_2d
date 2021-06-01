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

#ifndef FRAMEWORKS_VSYNC_INCLUDE_VSYNC_MANAGER_H
#define FRAMEWORKS_VSYNC_INCLUDE_VSYNC_MANAGER_H

#include <list>
#include <mutex>

#include "ivsync_manager.h"
#include <iremote_stub.h>
#include <message_parcel.h>
#include <message_option.h>

namespace OHOS {
class VsyncManager : public IRemoteStub<IVsyncManager> {
public:
    virtual int32_t OnRemoteRequest(uint32_t code, MessageParcel& data,
                                MessageParcel& reply, MessageOption& option) override;

    VsyncError ListenNextVsync(sptr<IVsyncCallback>& cb) override;

    void Callback(int64_t timestamp);

    void CheckVsyncRequest();
    void StopCheck();

private:
    std::list<sptr<IVsyncCallback>> callbacks;
    std::mutex callbacks_mutex;
    std::condition_variable condition_;
};
} // namespace OHOS

#endif // FRAMEWORKS_VSYNC_INCLUDE_VSYNC_MANAGER_H
