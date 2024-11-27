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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_IFRAME_RATE_LINKER_EXPECTED_FPS_UPDATE_CALLBACK_H
#define ROSEN_RENDER_SERVICE_BASE_RS_IFRAME_RATE_LINKER_EXPECTED_FPS_UPDATE_CALLBACK_H

#include <iremote_broker.h>
#include "pipeline/rs_render_frame_rate_linker.h"

namespace OHOS {
namespace Rosen {
class RSIFrameRateLinkerExpectedFpsUpdateCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.frameRateLinkerExpectedFpsUpdateCallback");
    RSIFrameRateLinkerExpectedFpsUpdateCallback() = default;
    virtual ~RSIFrameRateLinkerExpectedFpsUpdateCallback() noexcept = default;
    virtual void OnFrameRateLinkerExpectedFpsUpdate(pid_t dstPid, int32_t expectedFps) = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_RS_IFRAME_RATE_LINKER_EXPECTED_FPS_UPDATE_CALLBACK_H
