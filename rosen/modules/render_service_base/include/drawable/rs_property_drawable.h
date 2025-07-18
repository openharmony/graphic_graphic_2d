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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_H

#include <memory>

#include "common/rs_common_def.h"
#include "common/rs_rect.h"
#include "render/rs_filter.h"
#include "recording/draw_cmd_list.h"

#include "drawable/rs_drawable.h"

namespace OHOS::Rosen {
class RSRenderNode;
class RSFilter;
class RSFilterCacheManager;
class ExtendRecordingCanvas;

namespace DrawableV2 {
class RSPropertyDrawable : public RSDrawable {
public:
    RSPropertyDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList) : drawCmdList_(std::move(drawCmdList)) {}
    RSPropertyDrawable() = default;
    ~RSPropertyDrawable() override = default;

    void OnSync() override;
    void OnPurge() override;
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override;

protected:
    bool needSync_ = false;
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList_;
    std::shared_ptr<Drawing::DrawCmdList> stagingDrawCmdList_;
    std::string propertyDescription_;
    std::string stagingPropertyDescription_;

    friend class RSPropertyDrawCmdListUpdater;
};

class RSPropertyDrawCmdListUpdater {
public:
    // not copyable and moveable
    RSPropertyDrawCmdListUpdater(const RSPropertyDrawCmdListUpdater&) = delete;
    RSPropertyDrawCmdListUpdater(const RSPropertyDrawCmdListUpdater&&) = delete;
    RSPropertyDrawCmdListUpdater& operator=(const RSPropertyDrawCmdListUpdater&) = delete;
    RSPropertyDrawCmdListUpdater& operator=(const RSPropertyDrawCmdListUpdater&&) = delete;

    explicit RSPropertyDrawCmdListUpdater(int width, int height, RSPropertyDrawable* target);
    virtual ~RSPropertyDrawCmdListUpdater();
    const std::unique_ptr<ExtendRecordingCanvas>& GetRecordingCanvas() const;

protected:
    std::unique_ptr<ExtendRecordingCanvas> recordingCanvas_;
    RSPropertyDrawable* target_;
};

// ============================================================================
// Frame offset
class RSFrameOffsetDrawable : public RSPropertyDrawable {
public:
    RSFrameOffsetDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSFrameOffsetDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
};

// ============================================================================
// Clip To Bounds and Clip To Frame
class RSClipToBoundsDrawable : public RSPropertyDrawable {
public:
    RSClipToBoundsDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSClipToBoundsDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
};

class RSClipToFrameDrawable : public RSPropertyDrawable {
public:
    RSClipToFrameDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSClipToFrameDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
};

class RSFilterDrawable : public RSDrawable {
public:
    RSFilterDrawable();
    ~RSFilterDrawable() override = default;

    virtual bool IsForeground() const
    {
        return false;
    }

    // set flags for clearing filter cache
    // All MarkXXX function should be called from render_service thread
    void MarkFilterRegionChanged();
    void MarkFilterRegionInteractWithDirty();
    void MarkFilterRegionIsLargeArea();
    void MarkFilterForceUseCache(bool forceUseCache = true);
    void MarkFilterForceClearCache();
    void MarkEffectNode();
    void MarkForceClearCacheWithLastFrame();
    void MarkRotationChanged();
    void MarkNodeIsOccluded(bool isOccluded);
    void MarkNeedClearFilterCache();
    void MarkBlurIntersectWithDRM(bool IsIntersectWithDRM, bool isDark);
    void MarkInForegroundFilterAndCheckNeedForceClearCache(NodeId offscreenCanvasNodeId);

    bool IsFilterCacheValid() const;
    bool IsForceClearFilterCache() const;
    bool IsForceUseFilterCache() const;
    bool NeedPendingPurge() const;
    bool IsSkippingFrame() const;
    bool IsAIBarFilter() const;
    bool CheckAndUpdateAIBarCacheStatus(bool intersectHwcDamage);
    bool WouldDrawLargeAreaBlur();
    bool WouldDrawLargeAreaBlurPrecisely();

    void OnSync() override;
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override;
    const RectI GetFilterCachedRegion() const;

    bool IsFilterCacheValidForOcclusion();

    void SetDrawBehindWindowRegion(RectI region);

protected:
    void RecordFilterInfos(const std::shared_ptr<RSFilter>& rsFilter);

    bool needSync_ = false;
    bool needDrawBehindWindow_ = false;
    bool stagingNeedDrawBehindWindow_ = false;

    bool stagingIntersectWithDRM_ = false;
    bool stagingIsDarkColorMode_ = false;

    bool renderIntersectWithDRM_  = false;
    bool renderIsDarkColorMode_  = false;

    NodeId stagingNodeId_ = INVALID_NODEID;
    NodeId renderNodeId_ = INVALID_NODEID;
    std::string stagingNodeName_ = "invalid0";
    std::string renderNodeName_ = "invalid0";

    std::shared_ptr<RSFilter> filter_;
    std::shared_ptr<RSFilter> stagingFilter_;
    std::unique_ptr<RSFilterCacheManager> stagingCacheManager_;
    std::unique_ptr<RSFilterCacheManager> cacheManager_;
    RectI drawBehindWindowRegion_;
    RectI stagingDrawBehindWindowRegion_;
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_H
