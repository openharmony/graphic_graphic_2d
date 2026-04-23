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

#ifndef RENDER_SERVICE_CORE_FEATURE_FRAME_STABILITY_RS_FRAME_STABILITY_MANAGER_H
#define RENDER_SERVICE_CORE_FEATURE_FRAME_STABILITY_RS_FRAME_STABILITY_MANAGER_H

#include <chrono>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "common/rs_common_def.h"
#include "common/rs_occlusion_region.h"
#include "ipc_callbacks/rs_iframe_stability_callback.h"
#include "params/rs_screen_render_params.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "rs_frame_stability_state.h"
#include "transaction/rs_frame_stability_types.h"

namespace OHOS {
namespace Rosen {
inline const size_t MAX_FRAME_STABILITY_CONNECTION_NUM = 10;
inline const std::string FRAME_STABILITY_TAG = "RSFrameStability";
class RSFrameStabilityManager {
public:
    static RSFrameStabilityManager& GetInstance();

    int32_t RegisterFrameStabilityDetection(
        pid_t pid,
        const FrameStabilityTarget& target,
        const FrameStabilityConfig& config,
        sptr<RSIFrameStabilityCallback> callback
    );

    int32_t UnregisterFrameStabilityDetection(pid_t pid, const FrameStabilityTarget& target);

    int32_t StartFrameStabilityCollection(
        pid_t pid,
        const FrameStabilityTarget& target,
        const FrameStabilityConfig& config
    );

    int32_t GetFrameStabilityResult(pid_t pid, const FrameStabilityTarget& target, bool& result);

    // Clean contexts related to the pid, called when process exits
    void CleanResourcesByPid(pid_t pid);

    // Clean contexts related to the screenId, called when ScreenRenderNodeDrawable is destructed
    void CleanResourcesByScreenId(ScreenId screenId);

    void RecordCurrentFrameDirty(ScreenId screenId, const std::vector<RectI>& damageRegionRects, float screenArea);
private:
    RSFrameStabilityManager() = default;
    ~RSFrameStabilityManager() = default;
    RSFrameStabilityManager(const RSFrameStabilityManager&) = delete;
    RSFrameStabilityManager(const RSFrameStabilityManager&&) = delete;
    RSFrameStabilityManager& operator=(const RSFrameStabilityManager&) = delete;
    RSFrameStabilityManager& operator=(const RSFrameStabilityManager&&) = delete;

    void HandleStabilityTimeout(uint64_t targetId);
    void TriggerCallback(uint64_t targetId, bool isStable);
    float CalculateRegionPercentage(const Occlusion::Region& region, float screenArea);
    void RecordDirtyToCollector(ScreenId screenId, const std::vector<RectI>& damageRegionRects, float screenArea);
    void RecordDirtyToDetector(ScreenId screenId, const std::vector<RectI>& damageRegionRects, float screenArea);

    std::mutex detectorMutex_;
    std::mutex collectorMutex_;
    std::unordered_map<ScreenId, std::shared_ptr<DetectorContext>> screenDetectorContexts_;
    std::unordered_map<ScreenId, std::shared_ptr<CollectorContext>> screenCollectorContexts_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_FEATURE_FRAME_STABILITY_RS_FRAME_STABILITY_MANAGER_H
