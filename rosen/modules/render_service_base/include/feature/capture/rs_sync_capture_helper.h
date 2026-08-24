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

#ifndef RENDER_SERVICE_BASE_FEATURE_RS_SYNC_CAPTURE_HELPER_H
#define RENDER_SERVICE_BASE_FEATURE_RS_SYNC_CAPTURE_HELPER_H

#include <mutex>
#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSSyncCaptureHelper {
public:
    RSSyncCaptureHelper() = default;
    ~RSSyncCaptureHelper() = default;

    void InsertCaptureCmdsExecutedFlag(NodeId nodeId, bool flag);
    std::pair<bool, uint64_t> GetCaptureCmdsExecutedFlag(NodeId nodeId);
    void EraseCaptureCmdsExecutedFlag(NodeId nodeId);
    void CleanupStaleEntries(uint64_t maxAgeMs);
    uint64_t GetCurrentSteadyTimeMs() const;

private:
    static constexpr size_t MAX_CAPTURE_CMDS_FLAG_COUNT = 256;
    std::unordered_map<NodeId, std::pair<bool, uint64_t>> captureCmdsExecutedFlag_;
    std::mutex captureCmdsExecutedMutex_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_FEATURE_RS_SYNC_CAPTURE_HELPER_H