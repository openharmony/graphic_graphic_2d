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

#include "feature/capture/rs_sync_capture_helper.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr const uint64_t INVALID_TIME_STAMP = 0;
}
void RSSyncCaptureHelper::InsertCaptureCmdsExecutedFlag(NodeId nodeId, bool flag)
{
    if (nodeId == 0) {
        RS_LOGW("InsertCaptureCmdsExecutedFlag invalid nodeId=0, ignored");
        return;
    }
    std::lock_guard<std::mutex> lock(captureCmdsExecutedMutex_);
    if (!flag) {
        auto iter = captureCmdsExecutedFlag_.find(nodeId);
        if (iter == captureCmdsExecutedFlag_.end()) {
            if (captureCmdsExecutedFlag_.size() >= MAX_CAPTURE_CMDS_FLAG_COUNT) {
                NodeId oldestId = 0;
                uint64_t oldestTime = UINT64_MAX;
                for (const auto& [id, pair] : captureCmdsExecutedFlag_) {
                    if (pair.second < oldestTime) {
                        oldestTime = pair.second;
                        oldestId = id;
                    }
                }
                RS_LOGW("InsertCaptureCmdsExecutedFlag reach size limit, erase id %{public}" PRIu64, oldestId);
                captureCmdsExecutedFlag_.erase(oldestId);
            }
            captureCmdsExecutedFlag_[nodeId] = std::make_pair(false, GetCurrentSteadyTimeMs());
        }
        return;
    }
    captureCmdsExecutedFlag_[nodeId] = std::make_pair(flag, GetCurrentSteadyTimeMs());
}

void RSSyncCaptureHelper::EraseCaptureCmdsExecutedFlag(NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(captureCmdsExecutedMutex_);
    captureCmdsExecutedFlag_.erase(nodeId);
}

std::pair<bool, uint64_t> RSSyncCaptureHelper::GetCaptureCmdsExecutedFlag(NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(captureCmdsExecutedMutex_);
    auto iter = captureCmdsExecutedFlag_.find(nodeId);
    if (iter != captureCmdsExecutedFlag_.end()) {
        return iter->second;
    } else {
        return std::make_pair(true, INVALID_TIME_STAMP);
    }
}

void RSSyncCaptureHelper::CleanupStaleEntries(uint64_t maxAgeMs)
{
    std::lock_guard<std::mutex> lock(captureCmdsExecutedMutex_);
    if (captureCmdsExecutedFlag_.empty()) {
        return;
    }
    uint64_t nowMs = GetCurrentSteadyTimeMs();
    for (auto iter = captureCmdsExecutedFlag_.begin(); iter != captureCmdsExecutedFlag_.end();) {
        if (nowMs - iter->second.second > maxAgeMs) {
            RS_LOGW("CleanupStaleEntries erase time out entry, nodeId: %{public}" PRIu64
                ", duration:%{public}" PRIu64 "ms", iter->first, nowMs - iter->second.second);
            iter = captureCmdsExecutedFlag_.erase(iter);
        } else {
            ++iter;
        }
    }
}

uint64_t RSSyncCaptureHelper::GetCurrentSteadyTimeMs() const
{
    auto curTime = std::chrono::steady_clock::now().time_since_epoch();
    uint64_t curSteadyTime =
        static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(curTime).count());
    return curSteadyTime;
}
} // namespace Rosen
} // namespace OHOS