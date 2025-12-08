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

#ifndef RENDER_SERVICE_COMPOSER_BASE_LAYER_BACKEND_HDI_BACKEND_H
#define RENDER_SERVICE_COMPOSER_BASE_LAYER_BACKEND_HDI_BACKEND_H

#include <functional>
#include <unordered_map>
#include <refbase.h>

#include "hdi_device.h"
#include "hdi_screen.h"
#include "hdi_output.h"
#include "graphic_error.h"
#include "surface_type.h"

namespace OHOS {
namespace Rosen {

using OutputPtr = std::shared_ptr<HdiOutput>;

using OnScreenHotplugFunc = std::function<void(OutputPtr& output, bool connected, void* data)>;
using OnScreenRefreshFunc = std::function<void(uint32_t deviceId, void* data)>;

class HdiBackend {
public:
    /* for RS begin */
    static HdiBackend* GetInstance();
    RosenError RegScreenHotplug(OnScreenHotplugFunc func, void* data);
    RosenError RegScreenRefresh(OnScreenRefreshFunc func, void* data);
    RosenError RegHwcDeadListener(OnHwcDeadCallback func, void* data);
    RosenError RegScreenVBlankIdleCallback(OnVBlankIdleCallback func, void* data);
    void ResetDevice();
    /* for RS end */

    /* only used for mock tests */
    RosenError SetHdiBackendDevice(HdiDevice* device);
    void StartSample(const OutputPtr &output);
    /* set a temporary period used only for VSyncSampler::GetHardwarePeriod interface */
    void SetPendingMode(const OutputPtr& output, int64_t period, int64_t timestamp);
    void SetVsyncSamplerEnabled(const OutputPtr& output, bool enabled);
    bool GetVsyncSamplerEnabled(const OutputPtr& output);
    RosenError RegHwcEventCallback(RSHwcEventCallback func, void* data);

private:
    HdiBackend() = default;
    virtual ~HdiBackend() = default;

    HdiBackend(const HdiBackend& rhs) = delete;
    HdiBackend& operator=(const HdiBackend& rhs) = delete;
    HdiBackend(HdiBackend&& rhs) = delete;
    HdiBackend& operator=(HdiBackend&& rhs) = delete;

    HdiDevice *device_ = nullptr;
    void* onHotPlugCbData_ = nullptr;
    void* onRefreshCbData_ = nullptr;
    OnScreenHotplugFunc onScreenHotplugCb_ = nullptr;
    OnScreenRefreshFunc onScreenRefreshCb_ = nullptr;
    std::unordered_map<uint32_t, OutputPtr> outputs_;

    static void OnHdiBackendHotPlugEvent(uint32_t deviceId, bool connected, void* data);
    static void OnHdiBackendRefreshEvent(uint32_t deviceId, void* data);
    RosenError InitDevice();
    void OnHdiBackendConnected(uint32_t screenId, bool connected);
    void CreateHdiOutput(uint32_t screenId);
    void OnScreenHotplug(uint32_t screenId, bool connected);
    void OnScreenRefresh(uint32_t deviceId);
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_COMPOSER_BASE_LAYER_BACKEND_HDI_BACKEND_H