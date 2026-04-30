/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_IACTIVE_SCREEN_ID_CHANGED_CALLBACK_PROXY_H
#define ROSEN_RENDER_SERVICE_BASE_IACTIVE_SCREEN_ID_CHANGED_CALLBACK_PROXY_H

#include <iremote_proxy.h>
#include "ipc_callbacks/active_screen_id_changed_callback.h"
#include "ipc_callbacks/active_screen_id_changed_callback_ipc_interface_code.h"

namespace OHOS {
namespace Rosen {
class RSActiveScreenIdChangedCallbackProxy : public IRemoteProxy<RSIActiveScreenIdChangedCallback> {
public:
    explicit RSActiveScreenIdChangedCallbackProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<RSIActiveScreenIdChangedCallback>(impl) {}
    virtual ~RSActiveScreenIdChangedCallbackProxy() noexcept = default;

    void OnActiveScreenIdChanged(ScreenId activeScreenId) override;

private:
    static inline BrokerDelegator<RSActiveScreenIdChangedCallbackProxy> delegator_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_IACTIVE_SCREEN_ID_CHANGED_CALLBACK_PROXY_H
