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
#include "property/rs_properties.h"

namespace OHOS::Rosen {
struct CaptureParam {
    bool isInCaptureFlag_ = false;
    bool isCaptureDisplay_ = false;
    float scaleX_ = 0.0f;
    float scaleY_ = 0.0f;
    CaptureParam() {}
    CaptureParam(bool isInCaptureFlag, bool isCaptureDisplay, float scaleX, float scaleY)
        : isInCaptureFlag_(isInCaptureFlag)
        , isCaptureDisplay_(isCaptureDisplay)
        , scaleX_(scaleX)
        , scaleY_(scaleY) {}
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
    
    void SetForceCommitLayer(bool forceCommit)
    {
        isForceCommitLayer_ = forceCommit;
    }

    bool GetForceCommitLayer() const
    {
        return isForceCommitLayer_;
    }

private:
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
    bool isUIFirstDebugEnable_ = false;
    DirtyRegionDebugType dirtyRegionDebugType_ = DirtyRegionDebugType::DISABLED;
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> selfDrawingNodes_;
    bool isForceCommitLayer_ = false;
    // accumulatedDirtyRegion to decide whether to skip tranasparent nodes.
    Occlusion::Region accumulatedDirtyRegion_;
    bool watermarkFlag_ = false;
    std::shared_ptr<Drawing::Image> watermarkImg_ = nullptr;

    friend class RSMainThread;
    friend class RSUniRenderVisitor;
    friend class RSDirtyRectsDfx;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PARAMS_RS_RENDER_THREAD_PARAMS_H
