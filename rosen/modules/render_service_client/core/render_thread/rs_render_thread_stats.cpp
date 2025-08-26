/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifdef ROSEN_OHOS
#include "app_mgr_client.h"
#include "platform/common/rs_hisysevent.h"
#endif
#include "render_thread/rs_render_thread_stats.h"
#include <algorithm>
#include "rs_trace.h"
#include "common/rs_background_thread.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
RSRenderThreadStats& RSRenderThreadStats::GetInstance()
{
    static RSRenderThreadStats instance;
    return instance;
}

std::string RSRenderThreadStats::RenderFitToString(const Gravity& fit)
{
    switch (fit) {
        case Gravity::CENTER: return "CENTER";
        case Gravity::TOP: return "TOP";
        case Gravity::BOTTOM: return "BOTTOM";
        case Gravity::LEFT: return "LEFT";
        case Gravity::RIGHT: return "RIGHT";
        case Gravity::TOP_LEFT: return "TOP_LEFT";
        case Gravity::TOP_RIGHT: return "TOP_RIGHT";
        case Gravity::BOTTOM_LEFT: return "BOTTOM_LEFT";
        case Gravity::BOTTOM_RIGHT: return "BOTTOM_RIGHT";
        case Gravity::RESIZE: return "RESIZE";
        case Gravity::RESIZE_ASPECT: return "RESIZE_ASPECT";
        case Gravity::RESIZE_ASPECT_TOP_LEFT: return "RESIZE_ASPECT_TOP_LEFT";
        case Gravity::RESIZE_ASPECT_BOTTOM_RIGHT: return "RESIZE_ASPECT_BOTTOM_RIGHT";
        case Gravity::RESIZE_ASPECT_FILL: return "RESIZE_ASPECT_FILL";
        case Gravity::RESIZE_ASPECT_FILL_TOP_LEFT: return "RESIZE_ASPECT_FILL_TOP_LEFT";
        case Gravity::RESIZE_ASPECT_FILL_BOTTOM_RIGHT: return "RESIZE_ASPECT_FILL_BOTTOM_RIGHT";
        default: return "Unknown";
    }
}

bool RSRenderThreadStats::IsRenderFitInfoFull(const RSRenderFitInfo& info) const
{
    return info.size() >= RENDERFIT_INFO_MAP_LIMIT;
}

#ifdef ROSEN_OHOS
void RSRenderThreadStats::AddStaticInfo(const float frameWidth, const float frameHeight,
    const RSProperties& property, const ScalingMode scalingMode)
{
    const float boundsWidth = property.GetBoundsWidth();
    const float boundsHeight = property.GetBoundsHeight();
    const Gravity renderfit = property.GetFrameGravity();
    if (!(ROSEN_EQ(frameWidth, boundsWidth) && ROSEN_EQ(frameHeight, boundsHeight))) {
        if (renderfit != Gravity::RESIZE || scalingMode == ScalingMode::SCALING_MODE_SCALE_FIT) {
            static const auto appMgrClient = std::make_shared<AppExecFwk::AppMgrClient>();
            AppExecFwk::RunningProcessInfo info;
            appMgrClient->GetProcessRunningInformation(info);
            if (info.bundleNames.empty()) {
                RS_LOGD("AddStaticInfo: bundleNames is empty");
                return;
            }
            AddRenderFitInfo(rsRenderFitRenderThreadInfo_, recentUpdatedRenderFitRenderThreadInfo_,
                info.bundleNames[0], renderfit);
            ReportRenderFitInfo();
        }
    }
}
#endif

void RSRenderThreadStats::AddRenderFitInfo(RSRenderFitInfo& info,
    RSRecentUpdatedRenderFitInfo& recentUpdateInfo, const std::string& packageName,
    Gravity renderfit)
{
    std::lock_guard<std::mutex> lock(renderfitMutex_);
    auto packageIter = info.find(packageName);
    if (packageIter == info.end()) {
        auto& renderFitSet = info[packageName];
        renderFitSet.insert(std::move(renderfit));
        recentUpdateInfo[packageName] = true;
    } else {
        auto& renderFitSet = packageIter->second;
        if (renderFitSet.find(renderfit) == renderFitSet.end()) {
            renderFitSet.insert(std::move(renderfit));
            recentUpdateInfo[packageName] = true;
        }
    }
}

void RSRenderThreadStats::ClearNodeRenderFitInfo()
{
    rsRenderFitRenderThreadInfo_.clear();
}

void RSRenderThreadStats::ClearRecentUpdatedRenderFitInfo()
{
    recentUpdatedRenderFitRenderThreadInfo_.clear();
}

void RSRenderThreadStats::ReportRenderFitInfo()
{
    std::lock_guard<std::mutex> lock(renderfitMutex_);
    std::string renderFitInfo =
        ConvertInfoToString(rsRenderFitRenderThreadInfo_, recentUpdatedRenderFitRenderThreadInfo_);
    if (renderFitInfo.empty()) {
        RS_LOGD("ReportRenderFitInfo: renderFitInfo is empty");
        return;
    }
#ifdef ROSEN_OHOS
    RSBackgroundThread::Instance().PostTask([renderFitInfo]() {
        RS_TRACE_NAME("RSRenderThreadStats::ReportRenderFitInfo in RSBackgroundThread");
        RSHiSysEvent::EventWrite(RSEventName::RENDERFIT_TYPE_RT_MONITORING, RSEventType::RS_BEHAVIOR,
            "RENDERFIT_INFO", renderFitInfo);
        RS_LOGW("ReportRenderFitInfo renderFitInfo[%{public}s]", renderFitInfo.c_str());
    });
#endif
    if (IsRenderFitInfoFull(rsRenderFitRenderThreadInfo_)) {
        ClearNodeRenderFitInfo();
    }
    ClearRecentUpdatedRenderFitInfo();
}

std::string RSRenderThreadStats::ConvertInfoToString(const RSRenderFitInfo& info,
    const RSRecentUpdatedRenderFitInfo& recentUpdateInfo)
{
    if (info.empty() || recentUpdateInfo.empty()) {
        RS_LOGD("ConvertInfoToString: Info is empty or is not updated");
        return "";
    }
    std::ostringstream oss;
    bool firstPackage = true;
    for (const auto& packageGroup : info) {
        const std::string& packageName = packageGroup.first;
        const auto& renderFitSet = packageGroup.second;
        auto packageUpdatedIter = recentUpdateInfo.find(packageName);
        if (packageUpdatedIter == recentUpdateInfo.end() || !packageUpdatedIter->second) {
            continue;
        }
        std::ostringstream packageOss;
        bool firstRenderFit = true;
        for (const auto& renderFit : renderFitSet) {
            std::string renderFitStr = RenderFitToString(renderFit);
            if (!firstRenderFit) {
                packageOss << ",";
            } else {
                firstRenderFit = false;
            }
            packageOss << "renderFit:" << renderFitStr;
        }
        if (!firstPackage) {
            oss << "; ";
        } else {
            firstPackage = false;
        }
        oss << "packageName:" << packageName << " " << packageOss.str();
    }
    return oss.str();
}
} // namespace Rosen
} // namespace OHOS