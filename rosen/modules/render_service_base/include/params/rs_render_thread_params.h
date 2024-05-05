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

#ifndef RENDER_SERVICE_BASE_PARAMS_RS_RENDER_THREAD_PARAMS_H
#define RENDER_SERVICE_BASE_PARAMS_RS_RENDER_THREAD_PARAMS_H

#include <memory>
#include <vector>
#include "common/rs_occlusion_region.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/ohos/rs_jank_stats.h"
#include "property/rs_properties.h"

namespace OHOS::Rosen {
struct CaptureParam {
    bool isInCaptureFlag_ = false;
    bool isSingleSurface_ = false;
    bool isMirror_ = false;
    float scaleX_ = 0.0f;
    float scaleY_ = 0.0f;
    bool isFirstNode_ = false;
    CaptureParam() {}
    CaptureParam(bool isInCaptureFlag, bool isSingleSurface, bool isMirror,
        float scaleX, float scaleY, bool isFirstNode = false)
        : isInCaptureFlag_(isInCaptureFlag),
        isSingleSurface_(isSingleSurface),
        isMirror_(isMirror),
        scaleX_(scaleX),
        scaleY_(scaleY),
        isFirstNode_(isFirstNode) {}
};
class RSB_EXPORT RSRenderThreadParams {
public:
    RSRenderThreadParams() = default;
    virtual ~RSRenderThreadParams() = default;

    bool IsPartialRenderEnabled() const
    {
        return isPartialRenderEnabled_;
    }

    bool IsRegionDebugEnabled() const
    {
        return isRegionDebugEnabled_;
    }

    bool IsOpDropped() const
    {
        return isOpDropped_;
    }

    void SetOpDropped(bool opDropped)
    {
        isOpDropped_ = opDropped;
    }

    bool GetUIFirstDebugEnabled() const
    {
        return isUIFirstDebugEnable_;
    }

    void SetTimestamp(uint64_t timestamp)
    {
        timestamp_ = timestamp;
    }

    uint64_t GetCurrentTimestamp() const
    {
        return timestamp_;
    }

    const std::vector<std::shared_ptr<RSSurfaceRenderNode>>& GetSelfDrawingNodes() const
    {
        return selfDrawingNodes_;
    }

    const std::vector<std::shared_ptr<RSSurfaceRenderNode>>& GetHardwareEnabledTypeNodes() const
    {
        return hardwareEnabledTypeNodes_;
    }

    void SetPendingScreenRefreshRate(uint32_t rate)
    {
        pendingScreenRefreshRate_ = rate;
    }

    uint32_t GetPendingScreenRefreshRate() const
    {
        return pendingScreenRefreshRate_;
    }

    Occlusion::Region& GetAccumulatedDirtyRegion()
    {
        return accumulatedDirtyRegion_;
    }

    bool Reset()
    {
        accumulatedDirtyRegion_.Reset();
        return true;
    }

    bool GetWatermarkFlag() const
    {
        return watermarkFlag_;
    }

    std::shared_ptr<Drawing::Image> GetWatermarkImg() const
    {
        return watermarkImg_;
    }

    void SetWatermark(bool watermarkFlag, const std::shared_ptr<Drawing::Image>& watermarkImg)
    {
        watermarkFlag_ = watermarkFlag;
        watermarkImg_ = std::move(watermarkImg);
    }

    void SetOcclusionEnabled(bool isOcclusionEnabled)
    {
        isOcclusionEnabled_ = isOcclusionEnabled;
    }

    bool IsOcclusionEnabled() const
    {
        return isOcclusionEnabled_;
    }

    void SetCurtainScreenUsingStatus(bool isCurtainScreenOn)
    {
        isCurtainScreenOn_ = isCurtainScreenOn;
    }

    bool IsCurtainScreenOn() const
    {
        return isCurtainScreenOn_;
    }
    
    void SetForceCommitLayer(bool forceCommit)
    {
        isForceCommitLayer_ = forceCommit;
    }

    bool GetForceCommitLayer() const
    {
        return isForceCommitLayer_;
    }

    void SetRequestNextVsyncFlag(bool flag)
    {
        needRequestNextVsyncAnimate_ = flag;
    }

    bool GetRequestNextVsyncFlag() const
    {
        return needRequestNextVsyncAnimate_;
    }

    void SetOnVsyncStartTime(int64_t time)
    {
        onVsyncStartTime_ = time;
    }

    int64_t GetOnVsyncStartTime() const
    {
        return onVsyncStartTime_;
    }

    void SetOnVsyncStartTimeSteady(int64_t timeSteady)
    {
        onVsyncStartTimeSteady_ = timeSteady;
    }

    int64_t GetOnVsyncStartTimeSteady() const
    {
        return onVsyncStartTimeSteady_;
    }

    void SetIsUniRenderAndOnVsync(bool isUniRenderAndOnVsync)
    {
        isUniRenderAndOnVsync_ = isUniRenderAndOnVsync;
    }

    bool IsUniRenderAndOnVsync() const
    {
        return isUniRenderAndOnVsync_;
    }

    void SetStartVisit(bool startVisit)
    {
        startVisit_ = startVisit;
    }

    bool GetStartVisit() const
    {
        return startVisit_;
    }

    void SetHasCaptureImg(bool hasCaptureImg)
    {
        hasCaptureImg_ = hasCaptureImg;
    }

    bool GetHasCaptureImg() const
    {
        return hasCaptureImg_;
    }

    void SetRootIdOfCaptureWindow(NodeId rootIdOfCaptureWindow)
    {
        rootIdOfCaptureWindow_ = rootIdOfCaptureWindow;
    }

    NodeId GetRootIdOfCaptureWindow() const
    {
        return rootIdOfCaptureWindow_;
    }

    void SetContext(std::shared_ptr<RSContext> context)
    {
        context_ = context;
    }

    const std::shared_ptr<RSContext>& GetContext() const
    {
        return context_.lock();
    }

    void SetImplicitAnimationEnd(bool isImplicitAnimationEnd)
    {
        isImplicitAnimationEnd_ = isImplicitAnimationEnd;
    }

    bool GetImplicitAnimationEnd() const
    {
        return isImplicitAnimationEnd_;
    }

private:
    bool startVisit_ = false;
    bool hasCaptureImg_ = false;
    NodeId rootIdOfCaptureWindow_ = INVALID_NODEID;
    // Used by hardware thred
    uint64_t timestamp_ = 0;
    uint32_t pendingScreenRefreshRate_ = 0;
    // RSDirtyRectsDfx dfx
    std::vector<std::string> dfxTargetSurfaceNames_;
    bool isRegionDebugEnabled_ = false;
    bool isPartialRenderEnabled_ = false;
    bool isDirtyRegionDfxEnabled_ = false;
    bool isTargetDirtyRegionDfxEnabled_ = false;
    bool isDisplayDirtyDfxEnabled_ = false;
    bool isOpaqueRegionDfxEnabled_ = false;
    bool isVisibleRegionDfxEnabled_ = false;
    bool isOpDropped_ = false;
    bool isOcclusionEnabled_ = false;
    bool isUIFirstDebugEnable_ = false;
    DirtyRegionDebugType dirtyRegionDebugType_ = DirtyRegionDebugType::DISABLED;
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> selfDrawingNodes_;
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledTypeNodes_;
    bool isForceCommitLayer_ = false;
    // accumulatedDirtyRegion to decide whether to skip tranasparent nodes.
    Occlusion::Region accumulatedDirtyRegion_;
    bool watermarkFlag_ = false;
    std::shared_ptr<Drawing::Image> watermarkImg_ = nullptr;

    bool needRequestNextVsyncAnimate_ = false;

    int64_t onVsyncStartTime_ = TIMESTAMP_INITIAL;
    int64_t onVsyncStartTimeSteady_ = TIMESTAMP_INITIAL;
    bool isUniRenderAndOnVsync_ = false;
    std::weak_ptr<RSContext> context_;
    bool isCurtainScreenOn_ = false;

    bool isImplicitAnimationEnd_ = false;

    friend class RSMainThread;
    friend class RSUniRenderVisitor;
    friend class RSDirtyRectsDfx;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PARAMS_RS_RENDER_THREAD_PARAMS_H
