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

#include "platform/ohos/rs_node_stats.h"

#include <algorithm>
#include <chrono>
#include <sys/time.h>
#include <unistd.h>

#include "hisysevent.h"
#include "rs_trace.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t TOP1_INDEX = 0;
constexpr size_t TOP2_INDEX = 1;
constexpr size_t TOP3_INDEX = 2;
constexpr size_t EMPTY_NODE_DESCRIPTION_SIZE = 0;
constexpr RSNodeCount NONE_NODE_COUNT = 0;
const RSNodeDescription NONE_NODE_DESCRIPTION = "NaN";
const RSNodeDescription EMPTY_NODE_DESCRIPTION = "EmptyName";
}

RSNodeStats& RSNodeStats::GetInstance()
{
    static RSNodeStats instance;
    return instance;
}

void RSNodeStats::AddNodeStats(const RSNodeStatsType& nodeStats, RSNodeStatsUpdateMode updateMode)
{
    std::lock_guard<std::mutex> lock(mutex_);
    NodeId nodeId = std::get<NodeId>(nodeStats);
    if (updateMode == RSNodeStatsUpdateMode::DISCARD_DUPLICATE && rsNodeIdSet_.find(nodeId) != rsNodeIdSet_.end()) {
        return;
    }
    rsNodeIdSet_.insert(nodeId);
    rsNodeStatsVec_.push_back(nodeStats);
    rsNodeCountTotal_ += std::get<RSNodeCount>(nodeStats);
}

void RSNodeStats::ClearNodeStats()
{
    std::lock_guard<std::mutex> lock(mutex_);
    rsNodeIdSet_.clear();
    rsNodeStatsVec_.clear();
    rsNodeCountTotal_ = 0;
}

void RSNodeStats::ReportRSNodeLimitExceeded()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (rsNodeCountTotal_ < RS_NODE_REPORT_LIMIT) {
        return;
    }
    int64_t curSysTime = GetCurrentSystimeMs();
    int64_t curSteadyTime = GetCurrentSteadyTimeMs();
    if (lastReportTime_ != TIMESTAMP_INITIAL && lastReportTimeSteady_ != TIMESTAMP_INITIAL &&
        (curSysTime - lastReportTime_ <= REPORT_INTERVAL_LIMIT ||
        curSteadyTime - lastReportTimeSteady_ <= REPORT_INTERVAL_LIMIT)) {
        return;
    }
    SortNodeStats();
    const auto nodeInfoTop1 = GetNodeStatsToReportByIndex(TOP1_INDEX);
    const auto nodeInfoTop2 = GetNodeStatsToReportByIndex(TOP2_INDEX);
    const auto nodeInfoTop3 = GetNodeStatsToReportByIndex(TOP3_INDEX);
    RS_TRACE_NAME_FMT("RSNodeStats::ReportRSNodeLimitExceeded "
                      "nodeCount=%u, top1=%u [%s], top2=%u [%s], top3=%u [%s], timestamp=%" PRId64,
                      rsNodeCountTotal_, nodeInfoTop1.second, nodeInfoTop1.first.c_str(), nodeInfoTop2.second,
                      nodeInfoTop2.first.c_str(), nodeInfoTop3.second, nodeInfoTop3.first.c_str(), curSysTime);
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC,
                    RS_NODE_LIMIT_EXCEEDED_EVENT_NAME,
                    OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
                    "RS_NODE_LIMIT", RS_NODE_LIMIT,
                    "RS_ACTUAL_NODE", rsNodeCountTotal_,
                    "TIMESTAMP", static_cast<uint64_t>(curSysTime),
                    "RS_APP_WINDOW_TOTAL", static_cast<uint32_t>(rsNodeStatsVec_.size()),
                    "RS_TOP1_APP_NAME", nodeInfoTop1.first,
                    "RS_TOP1_APP_NODE", nodeInfoTop1.second,
                    "RS_TOP2_APP_NAME", nodeInfoTop2.first,
                    "RS_TOP2_APP_NODE", nodeInfoTop2.second,
                    "RS_TOP3_APP_NAME", nodeInfoTop3.first,
                    "RS_TOP3_APP_NODE", nodeInfoTop3.second);
    lastReportTime_ = curSysTime;
    lastReportTimeSteady_ = curSteadyTime;
}

void RSNodeStats::SortNodeStats(bool isSortByNodeCountDescendingOrder)
{
    std::sort(rsNodeStatsVec_.begin(), rsNodeStatsVec_.end());
    if (isSortByNodeCountDescendingOrder) {
        std::reverse(rsNodeStatsVec_.begin(), rsNodeStatsVec_.end());
    }
}

std::pair<RSNodeDescription, RSNodeCount> RSNodeStats::GetNodeStatsToReportByIndex(size_t index) const
{
    if (index < rsNodeStatsVec_.size()) {
        const RSNodeStatsType& nodeStats = rsNodeStatsVec_.at(index);
        RSNodeDescription nodeDescription = std::get<RSNodeDescription>(nodeStats);
        nodeDescription = CheckEmptyAndReviseNodeDescription(nodeDescription);
        RSNodeCount nodeCount = std::get<RSNodeCount>(nodeStats);
        return std::make_pair(nodeDescription, nodeCount);
    }
    return std::make_pair(NONE_NODE_DESCRIPTION, NONE_NODE_COUNT);
}

RSNodeDescription RSNodeStats::CheckEmptyAndReviseNodeDescription(const RSNodeDescription& nodeDescription) const
{
    if (nodeDescription.size() == EMPTY_NODE_DESCRIPTION_SIZE) {
        return EMPTY_NODE_DESCRIPTION;
    }
    return nodeDescription;
}

int64_t RSNodeStats::GetCurrentSystimeMs() const
{
    auto curTime = std::chrono::system_clock::now().time_since_epoch();
    int64_t curSysTime = std::chrono::duration_cast<std::chrono::milliseconds>(curTime).count();
    return curSysTime;
}

int64_t RSNodeStats::GetCurrentSteadyTimeMs() const
{
    auto curTime = std::chrono::steady_clock::now().time_since_epoch();
    int64_t curSteadyTime = std::chrono::duration_cast<std::chrono::milliseconds>(curTime).count();
    return curSteadyTime;
}

} // namespace Rosen
} // namespace OHOS
