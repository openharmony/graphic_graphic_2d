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

#include "ivsync_manager.h"
#include <iremote_proxy.h>

namespace OHOS {
class VsyncManagerProxy : public IRemoteProxy<IVsyncManager> {
public:
    VsyncManagerProxy(const sptr<IRemoteObject>& impl);
    virtual ~VsyncManagerProxy();

    VsyncError ListenNextVsync(sptr<IVsyncCallback>& cb) override;

private:
    static inline BrokerDelegator<VsyncManagerProxy> delegator_;
};
} // namespace OHOS

#endif // FRAMEWORKS_VSYNC_INCLUDE_VSYNC_MANAGER_PROXY_H
