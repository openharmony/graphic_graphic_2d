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

#ifndef FRAMEWORKS_VSYNC_INCLUDE_VSYNC_MANAGER_PROXY_H
#define FRAMEWORKS_VSYNC_INCLUDE_VSYNC_MANAGER_PROXY_H

#include <iremote_proxy.h>

#include "ivsync_manager.h"

namespace OHOS {
namespace Vsync {
class VsyncManagerProxy : public IRemoteProxy<IVsyncManager> {
public:
    VsyncManagerProxy(const sptr<IRemoteObject>& impl);
    virtual ~VsyncManagerProxy() = default;

    virtual GSError ListenVsync(sptr<IVsyncCallback>& cb) override;
    virtual GSError RemoveVsync(sptr<IVsyncCallback>& cb) override;
    virtual GSError GetVsyncFrequency(uint32_t &freq) override;

private:
    static inline BrokerDelegator<VsyncManagerProxy> delegator_;
};
} // namespace Vsync
} // namespace OHOS

#endif // FRAMEWORKS_VSYNC_INCLUDE_VSYNC_MANAGER_PROXY_H
