/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_FRAME_RATE_LINKER_EXPECTED_FPS_UPDATE_CALLBACK_PROXY_H
#define ROSEN_RENDER_SERVICE_BASE_FRAME_RATE_LINKER_EXPECTED_FPS_UPDATE_CALLBACK_PROXY_H

#include <iremote_proxy.h>

#include "ipc_callbacks/rs_iframe_rate_linker_expected_fps_update_callback.h"

namespace OHOS {
namespace Rosen {
class RSFrameRateLinkerExpectedFpsUpdateCallbackProxy :
    public IRemoteProxy<RSIFrameRateLinkerExpectedFpsUpdateCallback> {
public:
    explicit RSFrameRateLinkerExpectedFpsUpdateCallbackProxy(const sptr<IRemoteObject>& impl);
    virtual ~RSFrameRateLinkerExpectedFpsUpdateCallbackProxy() noexcept = default;

    void OnFrameRateLinkerExpectedFpsUpdate(pid_t dstPid, int32_t expectedFps) override;

private:
    static inline BrokerDelegator<RSFrameRateLinkerExpectedFpsUpdateCallbackProxy> delegator_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_FRAME_RATE_LINKER_EXPECTED_FPS_UPDATE_CALLBACK_PROXY_H
