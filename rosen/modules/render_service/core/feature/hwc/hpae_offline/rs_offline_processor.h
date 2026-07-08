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

#ifndef RS_CORE_FEATURE_HWC_OFFLINE_PROCESSOR_H
#define RS_CORE_FEATURE_HWC_OFFLINE_PROCESSOR_H

#include <atomic>
#include <cstdint>
#include <map>
#include <mutex>
#include <unordered_map>
#include <deque>

#include "feature/hwc/hpae_offline/rs_offline_result.h"
#include "feature/hwc/hpae_offline/rs_offline_util.h"
#include "feature/hwc/hpae_offline/rs_offline_device.h"
#include "feature/hwc/hpae_offline/rs_gpu_offline_device.h"
#include "feature/hwc/hpae_offline/rs_hpae_offline_device.h"

namespace OHOS {
namespace Rosen {

class RSOfflineProcessor : public std::enable_shared_from_this<RSOfflineProcessor> {
public:
    static RSOfflineProcessor& GetOfflineProcessor();
    ~RSOfflineProcessor();

    bool IsRSOfflineProcessorReady(std::shared_ptr<RSSurfaceRenderNode> surfaceNode,
        OfflineDeviceType offlineDeviceType);

    bool PostProcessOfflineTask(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable>& surfaceDrawable,
        offlineTaskId taskId);
    bool PostProcessOfflineTask(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, offlineTaskId taskId);
    bool WaitForProcessOfflineResult(offlineTaskId taskId, std::chrono::milliseconds timeout,
        ProcessOfflineResult& processOfflineResult);
    void CheckAndPostClearOfflineResourceTask(OfflineDeviceType offlineDeviceType,
        const std::vector<uint64_t>& offlineNodeIds = {});

private:
    RSOfflineProcessor();
    RSOfflineProcessor(const RSOfflineProcessor&) = delete;
    RSOfflineProcessor(const RSOfflineProcessor&&) = delete;
    RSOfflineProcessor& operator=(const RSOfflineProcessor&) = delete;
    RSOfflineProcessor& operator=(const RSOfflineProcessor&&) = delete;

    OfflineDeviceType GetOfflineDeviceType(offlineTaskId taskId);
    std::shared_ptr<RSOfflineDevice> CreateDevice(OfflineDeviceType type);
    std::shared_ptr<RSOfflineDevice> GetOrCreateOfflineDevice(OfflineDeviceType type, bool createIfNotExist);
    void DeleteOfflineDeviceFromType(OfflineDeviceType deviceType);

    mutable std::mutex deviceMutex_;
    std::unordered_map<OfflineDeviceType, std::shared_ptr<RSOfflineDevice>> devicePool_;
    mutable std::mutex deviceTypeMapMutex_;
    std::unordered_map<uint64_t, OfflineDeviceType> deviceTypeMap_;
    std::deque<uint64_t> deviceTypeInsertOrder_;
    static constexpr int deviceTypeMapMaxSize_ = 8;
};

} // namespace Rosen
} // namespace OHOS

#endif // RS_CORE_FEATURE_HWC_OFFLINE_PROCESSOR_H