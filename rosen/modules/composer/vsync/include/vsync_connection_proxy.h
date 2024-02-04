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

#ifndef VSYNC_VSYNC_CONNECTION_PROXY_H
#define VSYNC_VSYNC_CONNECTION_PROXY_H

#include <iremote_proxy.h>

#include "graphic_common.h"
#include "ivsync_connection.h"

namespace OHOS {
namespace Rosen {
class VSyncConnectionProxy : public IRemoteProxy<IVSyncConnection> {
public:
    explicit VSyncConnectionProxy(const sptr<IRemoteObject>& impl);
    virtual ~VSyncConnectionProxy() = default;

    virtual VsyncError RequestNextVSync() override;
    virtual VsyncError RequestNextVSync(const std::string& fromWhom, int64_t lastVSyncTS) override;
    virtual VsyncError GetReceiveFd(int32_t &fd) override;
    virtual VsyncError SetVSyncRate(int32_t rate) override;
    virtual VsyncError Destroy() override;

private:
    static inline BrokerDelegator<VSyncConnectionProxy> delegator_;
};
} // namespace Vsync
} // namespace OHOS

#endif // VSYNC_VSYNC_CONNECTION_PROXY_H
