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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_NODE_STATS_H
#define ROSEN_RENDER_SERVICE_BASE_RS_NODE_STATS_H

#include <cstdint>
#include <mutex>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "common/rs_common_def.h"
#include "nocopyable.h"

namespace OHOS {
namespace Rosen {
using RSNodeCount = uint32_t;
using RSNodeDescription = std::string;
using RSNodeStatsType = std::tuple<RSNodeCount, NodeId, RSNodeDescription>;

inline RSNodeStatsType CreateRSNodeStatsItem(RSNodeCount nodeCount, NodeId nodeId, RSNodeDescription nodeDesc)
{
    RSNodeStatsType nodeStats{nodeCount, nodeId, nodeDesc};
    return nodeStats;
}

enum class RSNodeStatsUpdateMode : size_t {
    SIMPLE_APPEND = 0,
    DISCARD_DUPLICATE = 1
};

class RSNodeStats {
public:
    static RSNodeStats& GetInstance();
    void AddNodeStats(const RSNodeStatsType& nodeStats,
                      RSNodeStatsUpdateMode updateMode = RSNodeStatsUpdateMode::DISCARD_DUPLICATE);
    void ClearNodeStats();
    void ReportRSNodeLimitExceeded();

private:
    RSNodeStats() = default;
    ~RSNodeStats() = default;
    DISALLOW_COPY_AND_MOVE(RSNodeStats);

    void SortNodeStats(bool isSortByNodeCountDescendingOrder = true);
    std::pair<RSNodeDescription, RSNodeCount> GetNodeStatsToReportByIndex(size_t index) const;
    RSNodeDescription CheckEmptyAndReviseNodeDescription(const RSNodeDescription& nodeDescription) const;
    std::pair<uint32_t, uint32_t> GetCurrentRSNodeLimit() const;
    int64_t GetCurrentSystimeMs() const;
    int64_t GetCurrentSteadyTimeMs() const;

    static constexpr int64_t TIMESTAMP_INITIAL = -1;
    static constexpr int64_t REPORT_INTERVAL_LIMIT = 10000; // 10s
    static constexpr int RS_NODE_LIMIT_PROPERTY_MIN = 1;
    static constexpr int RS_NODE_LIMIT_PROPERTY_MAX = 1000;
    static constexpr float RS_NODE_LIMIT_REPORT_RATIO = 1.5f;
    static inline const std::string RS_NODE_LIMIT_EXCEEDED_EVENT_NAME = "RS_NODE_LIMIT_EXCEEDED";

    int64_t lastReportTime_ = TIMESTAMP_INITIAL;
    int64_t lastReportTimeSteady_ = TIMESTAMP_INITIAL;
    std::mutex mutex_;
    std::vector<RSNodeStatsType> rsNodeStatsVec_;
    std::unordered_set<NodeId> rsNodeIdSet_;
    uint32_t rsNodeCountTotal_ = 0;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_RS_NODE_STATS_H
