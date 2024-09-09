/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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

#include "rs_vsync_rate_reduce_manager.h"
#include <parameters.h>
#include <ratio>
#include <thread>
#include "common/rs_optional_trace.h"
#include "pipeline/rs_main_thread.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr int32_t DEFAULT_RATE = 1;
constexpr int32_t SIMI_VISIBLE_RATE = 2;
constexpr int32_t SYSTEM_ANIMATED_SCENES_RATE = 2;
constexpr int32_t INVISBLE_WINDOW_RATE = 10;
} // Anonymous namespace

void RSVsyncRateReduceManager::SetVSyncRateByVisibleLevel(std::map<NodeId, RSVisibleLevel>& visMapForVsyncRate,
    std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces)
{
    if (!vsyncControlEnabled_ || appVSyncDistributor_ == nullptr) {
        return;
    }
    if (!RSMainThread::Instance()->IsSystemAnimatedScenesListEmpty()) {
        visMapForVsyncRate.clear();
        for (auto it = curAllSurfaces.rbegin(); it != curAllSurfaces.rend(); ++it) {
            auto curSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
            if (curSurface == nullptr || curSurface->GetDstRect().IsEmpty() || curSurface->IsLeashWindow()) {
                continue;
            }
            visMapForVsyncRate[curSurface->GetId()] = RSVisibleLevel::RS_SYSTEM_ANIMATE_SCENE;
        }
        isReduceBySystemAnimatedScenes_ = true;
    }
    NotifyVSyncRates(visMapForVsyncRate);
}

void RSVsyncRateReduceManager::SetUniVSyncRateByVisibleLevel(const std::shared_ptr<RSUniRenderVisitor>& uniVisitor)
{
    if (!vsyncControlEnabled_ || appVSyncDistributor_ == nullptr) {
        return;
    }
    if (uniVisitor == nullptr) {
        RS_LOGE("RSVsyncRateReduceManager::SetUniVSyncRateByVisibleLevel GetUniRenderVisitor fail");
        return;
    }
    bool isSysAnimatesEmpty = RSMainThread::Instance()->IsSystemAnimatedScenesListEmpty();
    bool surfaceIdsChanged = lastAllMainAndLeashWindowNodesIds_ != curAllMainAndLeashWindowNodesIds_;
    bool focusChanged = lastFocusedNodeId_ != focusedNodeId_;
    bool needRefresh = isReduceBySystemAnimatedScenes_ && isSysAnimatesEmpty;
    bool notifyCdt = vSyncRatesChanged_ || surfaceIdsChanged || focusChanged || needRefresh;
    RS_LOGD("SetUniVSyncRate notifyCdt=%{public}d needRefresh=%{public}d, ratesC=%{public}d, surfaceIdsC=%{public}d,"
        " focusIdC=%{public}d, sysAnimated=%{public}d", notifyCdt, needRefresh, vSyncRatesChanged_, surfaceIdsChanged,
        focusChanged, !isSysAnimatesEmpty);
    RS_TRACE_NAME_FMT("SetUniVSyncRate notifyCdt=%d, needRefresh=%d, ratesC=%d, surfaceIdsC=%d, focusIdC=%d,"
        " sysAnimated=%d", notifyCdt, needRefresh, vSyncRatesChanged_, surfaceIdsChanged, focusChanged,
        !isSysAnimatesEmpty);
    if (!notifyCdt) {
        return;
    }
    if (surfaceIdsChanged) {
        curAllMainAndLeashWindowNodesIds_.swap(lastAllMainAndLeashWindowNodesIds_);
    }
    if (focusChanged) {
        lastFocusedNodeId_ = focusedNodeId_;
    }
    if (needRefresh) {
        isReduceBySystemAnimatedScenes_ = false;
    }
    if (!isSysAnimatesEmpty) {
        isReduceBySystemAnimatedScenes_ = true;
    }
    NotifyVSyncRates(visMapForVSyncRate_);
}

void RSVsyncRateReduceManager::NotifyVSyncRates(const std::map<NodeId, RSVisibleLevel>& vSyncRates)
{
    RS_TRACE_NAME_FMT("NotifyVSyncRates vSyncRates.size=[%lu]", vSyncRates.size());
    if (vSyncRates.empty()) {
        return;
    }
    auto notifyVsyncFunc = [distributor = appVSyncDistributor_] (NodeId nodeId, RSVisibleLevel level) {
        bool isSysAnimate = false;
        int32_t rate = DEFAULT_RATE;
        if (level == RSVisibleLevel::RS_SEMI_DEFAULT_VISIBLE) {
            rate = SIMI_VISIBLE_RATE;
        } else if (level == RSVisibleLevel::RS_SYSTEM_ANIMATE_SCENE) {
            isSysAnimate = true;
            rate = SYSTEM_ANIMATED_SCENES_RATE;
        } else if (level == RSVisibleLevel::RS_INVISIBLE) {
            rate = INVISBLE_WINDOW_RATE;
        } else {
            rate = DEFAULT_RATE;
        }
        distributor->SetQosVSyncRate(nodeId, rate, isSysAnimate);
        RS_OPTIONAL_TRACE_NAME_FMT("NotifyVSyncRates nodeId=%" PRIu64 " rate=%d isSysAnimate=%d", nodeId, rate,
            isSysAnimate);
    };

    bool isVisibleChanged = lastVisMapForVSyncRate_.size() != vSyncRates.size();
    if (!isVisibleChanged) {
        auto iterCur = vSyncRates.begin();
        auto iterLast = lastVisMapForVSyncRate_.begin();
        for (; iterCur != vSyncRates.end(); iterCur++, iterLast++) {
            if (iterCur->first != iterLast->first || iterCur->second != iterLast->second) {
                isVisibleChanged = true;
                notifyVsyncFunc(iterCur->first, iterCur->second);
            }
        }
        if (isVisibleChanged) {
            lastVisMapForVSyncRate_ = vSyncRates;
        }
    } else {
        lastVisMapForVSyncRate_.clear();
        for (const auto& [nodeId, visLevel] : vSyncRates) {
            notifyVsyncFunc(nodeId, visLevel);
            lastVisMapForVSyncRate_.emplace(nodeId, visLevel);
        }
    }
}

void RSVsyncRateReduceManager::CollectVSyncRate(RSSurfaceRenderNode& node, RSVisibleLevel visibleLevel)
{
    if (!vsyncControlEnabled_) {
        return;
    }
    auto& visMap = visMapForVSyncRate_;
    auto& lastVisMap = lastVisMapForVSyncRate_;
    auto updateVisLevelFunc = [&] (RSVisibleLevel level) {
        auto nodeId = node.GetId();
        auto& nodeName = node.GetName();
        visMapForVSyncRate_[nodeId] = level;
        RS_OPTIONAL_TRACE_NAME_FMT("CollectVSyncRate name=%s id=%" PRIu64 " visLevel=%d", nodeName.c_str(), nodeId,
            level);
        RS_LOGD("CollectVSyncRate name=%{public}s id=%{public}" PRIu64 " visLevel=%{public}d", nodeName.c_str(), nodeId,
            level);
        return lastVisMap.count(nodeId) < 1 || lastVisMap[nodeId] != level;
    };
    if (RSMainThread::Instance()->IsSystemAnimatedScenesListEmpty()) {
        RSVisibleLevel levelForVsync = node.IsSCBNode() ? visibleLevel : RSVisibleLevel::RS_ALL_VISIBLE;
        SetVSyncRatesChanged(updateVisLevelFunc(levelForVsync));
    } else {
        if (!(node.GetDstRect().IsEmpty() || node.IsLeashWindow())) {
            RSVisibleLevel levelForVsync = RSVisibleLevel::RS_SYSTEM_ANIMATE_SCENE;
            SetVSyncRatesChanged(updateVisLevelFunc(levelForVsync));
        }
    }
}

void RSVsyncRateReduceManager::Init(const sptr<VSyncDistributor>& appVSyncDistributor)
{
    appVSyncDistributor_ = appVSyncDistributor;
    auto deviceType = RSMainThread::Instance()->GetDeviceType();
    vsyncControlEnabled_ = (deviceType == DeviceType::PC) && RSSystemParameters::GetVSyncControlEnabled();
}

void RSVsyncRateReduceManager::ResetFrameValues()
{
    if (!vsyncControlEnabled_) {
        return;
    }
    vSyncRatesChanged_ = false;
    focusedNodeId_ = 0;
    std::vector<NodeId> curAllMainAndLeashWindowNodesIds;
    std::map<NodeId, RSVisibleLevel> visMapForVSyncRate;
    visMapForVSyncRate.swap(visMapForVSyncRate_);
    curAllMainAndLeashWindowNodesIds.swap(curAllMainAndLeashWindowNodesIds_);
}

} // namespace Rosen
} // namespace OHOS
