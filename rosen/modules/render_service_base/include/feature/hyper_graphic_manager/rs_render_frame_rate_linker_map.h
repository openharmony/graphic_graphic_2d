/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_FRAME_RATE_LINKER_MAP_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_FRAME_RATE_LINKER_MAP_H

#include <unordered_map>
#include <unordered_set>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "feature/hyper_graphic_manager/rs_render_frame_rate_linker.h"

namespace OHOS {
namespace Rosen {
struct FrameRateLinkerUpdateInfo {
    FrameRateRange range;
    int32_t animatorExpectedFrameRate;
};
using FrameRateLinkerUpdateInfoMap = std::unordered_map<FrameRateLinkerId, FrameRateLinkerUpdateInfo>;

class RSRenderFrameRateLinker;

class RSB_EXPORT RSRenderFrameRateLinkerMap final {
public:
    RSRenderFrameRateLinkerMap() = default;
    ~RSRenderFrameRateLinkerMap() noexcept = default;

    bool RegisterFrameRateLinker(const std::shared_ptr<RSRenderFrameRateLinker>& linkerPtr);
    void UnregisterFrameRateLinker(FrameRateLinkerId id);
    void UnregisterFrameRateLinker(const std::unordered_set<FrameRateLinkerId>& unregisterIds);
    void UpdateFrameRateLinkers(const FrameRateLinkerUpdateInfoMap& updateInfoMap);
    void FilterFrameRateLinkerByPid(pid_t pid);
    std::shared_ptr<RSRenderFrameRateLinker> GetFrameRateLinker(FrameRateLinkerId id);

    const std::unordered_map<FrameRateLinkerId, std::shared_ptr<RSRenderFrameRateLinker>>& Get() const;

    bool RegisterFrameRateLinkerExpectedFpsUpdateCallback(pid_t listenerPid,
        int32_t dstPid, sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback> callback);
    void UnRegisterExpectedFpsUpdateCallbackByListener(pid_t listenerPid);

private:
    RSRenderFrameRateLinkerMap(const RSRenderFrameRateLinkerMap&) = delete;
    RSRenderFrameRateLinkerMap(const RSRenderFrameRateLinkerMap&&) = delete;
    RSRenderFrameRateLinkerMap& operator=(const RSRenderFrameRateLinkerMap&) = delete;
    RSRenderFrameRateLinkerMap& operator=(const RSRenderFrameRateLinkerMap&&) = delete;

    std::unordered_map<FrameRateLinkerId, std::shared_ptr<RSRenderFrameRateLinker>> frameRateLinkerMap_;

    friend class RSContext;
    friend class RSMainThread;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_FRAME_RATE_LINKER_MAP_H