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

#include <iremote_stub.h>
#include <message_parcel.h>
#include <message_option.h>

#include "ivsync_manager.h"

namespace OHOS {
namespace Vsync {
class VsyncCallbackDeathRecipient;
class VsyncManager : public IRemoteStub<IVsyncManager> {
    friend class VsyncCallbackDeathRecipient;

public:
    virtual int32_t OnRemoteRequest(uint32_t code, MessageParcel &data,
                                    MessageParcel &reply, MessageOption &option) override;

    virtual GSError ListenVsync(sptr<IVsyncCallback>& cb) override;
    virtual GSError RemoveVsync(sptr<IVsyncCallback>& cb) override;
    virtual GSError GetVsyncFrequency(uint32_t &freq) override;

    virtual void Callback(int64_t timestamp);

private:
    std::list<sptr<IVsyncCallback>> callbacks_;
    std::mutex callbacksMutex_;
};
} // namespace Vsync
} // namespace OHOS

#endif // FRAMEWORKS_VSYNC_INCLUDE_VSYNC_MANAGER_H
