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

#ifndef FRAMEWORKS_VSYNC_INCLUDE_VSYNC_CALLBACK_PROXY_H
#define FRAMEWORKS_VSYNC_INCLUDE_VSYNC_CALLBACK_PROXY_H

#include "ivsync_callback.h"
#include <iremote_proxy.h>

namespace OHOS {
class VsyncCallbackProxy : public IRemoteProxy<IVsyncCallback> {
public:
    VsyncCallbackProxy(const sptr<IRemoteObject>& impl);
    virtual ~VsyncCallbackProxy();

    void OnVsync(int64_t timestamp) override;

private:
    static inline BrokerDelegator<VsyncCallbackProxy> delegator_;
};
} // namespace OHOS

#endif // FRAMEWORKS_VSYNC_INCLUDE_VSYNC_CALLBACK_PROXY_H
