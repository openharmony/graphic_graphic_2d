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

#ifndef RENDER_SERVICE_BASE_PARAMS_RS_RENDER_PARAMS_H
#define RENDER_SERVICE_BASE_PARAMS_RS_RENDER_PARAMS_H

#include "common/rs_common_def.h"
#include "common/rs_occlusion_region.h"
#include "common/rs_rect.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "pipeline/rs_render_node.h"
#include "property/rs_properties.h"
#include "screen_manager/screen_types.h"
#include "utils/matrix.h"
#include "utils/region.h"

#ifndef ROSEN_CROSS_PLATFORM
#include <iconsumer_surface.h>
#include <surface.h>
#include "sync_fence.h"
#endif

namespace OHOS::Rosen {
#define RENDER_BASIC_PARAM_TO_STRING(basicType) (std::string(#basicType "[") + std::to_string(basicType) + "] ")
#define RENDER_RECT_PARAM_TO_STRING(rect) (std::string(#rect "[") + (rect).ToString() + "] ")
#define RENDER_PARAM_TO_STRING(param) (std::string(#param "[") + (param).ToString() + "] ")

struct DirtyRegionInfoForDFX {
    RectI oldDirty;
    RectI oldDirtyInSurface;
    bool operator==(const DirtyRegionInfoForDFX& rhs) const
    {
        return oldDirty == rhs.oldDirty && oldDirtyInSurface == rhs.oldDirtyInSurface;
    }
};
struct RSLayerInfo;
struct ScreenInfo;

typedef enum {
    RS_PARAM_DEFAULT,
    RS_PARAM_OWNED_BY_NODE,
    RS_PARAM_OWNED_BY_DRAWABLE,
    RS_PARAM_OWNED_BY_DRAWABLE_UIFIRST,
    RS_PARAM_INVALID,
} RSRenderParamsType;

class RSB_EXPORT RSRenderParams {
public:
    RSRenderParams(NodeId id) : id_(id) {}
    virtual ~RSRenderParams() = default;

    struct SurfaceParam {
        int width = 0;
        int height = 0;
    };

    void SetDirtyType(RSRenderParamsDirtyType dirtyType);

    void SetAlpha(float alpha);
    float GetAlpha() const;

    void SetAlphaOffScreen(bool alphaOffScreen);
    bool GetAlphaOffScreen() const;

    void SetMatrix(const Drawing::Matrix& matrix);
    const Drawing::Matrix& GetMatrix() const;

    void ApplyAlphaAndMatrixToCanvas(RSPaintFilterCanvas& canvas, bool applyMatrix = true) const;

    void SetBoundsRect(const Drawing::RectF& boundsRect);
    const Drawing::Rect& GetBounds() const;

    void SetFrameRect(const Drawing::RectF& frameRect);
    const Drawing::Rect& GetFrameRect() const;

    // return to add some dirtynode does not mark pending
    bool SetLocalDrawRect(const RectF& localDrawRect);
    const RectF& GetLocalDrawRect() const;

    void SetHasSandBox(bool hasSandBox);
    bool HasSandBox() const;

    bool GetShouldPaint() const;
    void SetShouldPaint(bool shouldPaint);
    void SetContentEmpty(bool empty);

    bool NeedSync() const;
    void SetNeedSync(bool needSync);

    const std::shared_ptr<RSFilter>& GetForegroundFilterCache() const;
    void SetForegroundFilterCache(const std::shared_ptr<RSFilter>& foregroundFilterCache);

    inline NodeId GetId() const
    {
        return id_;
    }

    void SetParamsType(RSRenderParamsType paramsType)
    {
        paramsType_ = paramsType;
    }

    inline RSRenderParamsType GetParamsType() const
    {
        return paramsType_;
    }

    Gravity GetFrameGravity() const
    {
        return frameGravity_;
    }

    void SetFrameGravity(Gravity gravity);

    void SetHDRBrightness(float hdrBrightness);

    inline float GetHDRBrightness() const
    {
        return hdrBrightness_;
    }

    void SetNeedFilter(bool needFilter);

    inline bool NeedFilter() const
    {
        return needFilter_;
    }

    void SetNodeType(RSRenderNodeType type);
    inline RSRenderNodeType GetType() const
    {
        return renderNodeType_;
    }

    void SetEffectNodeShouldPaint(bool effectNodeShouldPaint);
    inline bool GetEffectNodeShouldPaint() const
    {
        return effectNodeShouldPaint_;
    }

    void SetHasGlobalCorner(bool hasGlobalCorner);
    inline bool HasGlobalCorner() const
    {
        return hasGlobalCorner_;
    }

    void SetHasBlurFilter(bool hasBlurFilter);
    inline bool HasBlurFilter() const
    {
        return hasBlurFilter_;
    }

    void SetGlobalAlpha(float alpha);
    inline float GetGlobalAlpha() const
    {
        return globalAlpha_;
    }

    inline bool IsInBlackList() const
    {
        return isInBlackList_;
    }

    inline void SetInBlackList(bool isInBlackList)
    {
        isInBlackList_ = isInBlackList;
    }
    
    inline bool IsSnapshotSkipLayer() const
    {
        return isSnapshotSkipLayer_;
    }

    inline bool IsLayerDirty() const
    {
        return dirtyType_.test(RSRenderParamsDirtyType::LAYER_INFO_DIRTY);
    }

    inline bool IsBufferDirty() const
    {
        return dirtyType_.test(RSRenderParamsDirtyType::BUFFER_INFO_DIRTY);
    }

    void SetChildHasVisibleFilter(bool val);
    bool ChildHasVisibleFilter() const;
    void SetChildHasVisibleEffect(bool val);
    bool ChildHasVisibleEffect() const;

    void SetCacheSize(Vector2f size);
    inline Vector2f GetCacheSize() const
    {
        return cacheSize_;
    }

    void SetDrawingCacheChanged(bool isChanged, bool lastFrameSynced);
    bool GetDrawingCacheChanged() const;

    void SetNeedUpdateCache(bool needUpdateCache);
    bool GetNeedUpdateCache() const;

    void SetDrawingCacheType(RSDrawingCacheType cacheType);
    RSDrawingCacheType GetDrawingCacheType() const;

    void OpincSetIsSuggest(bool isSuggest);
    bool OpincIsSuggest() const;
    void OpincUpdateSupportFlag(bool supportFlag);
    bool OpincGetSupportFlag() const;
    void OpincUpdateRootFlag(bool suggestFlag);
    bool OpincGetRootFlag() const;
    void OpincSetCacheChangeFlag(bool state, bool lastFrameSynced);
    bool OpincGetCacheChangeState();

    void SetDrawingCacheIncludeProperty(bool includeProperty);
    bool GetDrawingCacheIncludeProperty() const;

    void SetRSFreezeFlag(bool freezeFlag);
    bool GetRSFreezeFlag() const;
    void SetShadowRect(Drawing::Rect rect);
    Drawing::Rect GetShadowRect() const;

    // One-time trigger, needs to be manually reset false in main/RT thread after each sync operation
    void OnCanvasDrawingSurfaceChange(const std::unique_ptr<RSRenderParams>& target);
    bool GetCanvasDrawingSurfaceChanged() const;
    void SetCanvasDrawingSurfaceChanged(bool changeFlag);
    SurfaceParam GetCanvasDrawingSurfaceParams();
    void SetCanvasDrawingSurfaceParams(int width, int height);

    void SetStartingWindowFlag(bool b)
    {
        if (startingWindowFlag_ == b) {
            return;
        }
        startingWindowFlag_ = b;
        needSync_ = true;
    }

    bool GetStartingWindowFlag() const
    {
        return startingWindowFlag_;
    }

    bool IsRepaintBoundary() const;
    void MarkRepaintBoundary(bool isRepaintBoundary);

    bool SetFirstLevelNode(NodeId firstLevelNodeId);
    NodeId GetFirstLevelNodeId() const;
    bool SetUiFirstRootNode(NodeId uifirstRootNodeId);
    NodeId GetUifirstRootNodeId() const;

    // disable copy and move
    RSRenderParams(const RSRenderParams&) = delete;
    RSRenderParams(RSRenderParams&&) = delete;
    RSRenderParams& operator=(const RSRenderParams&) = delete;
    RSRenderParams& operator=(RSRenderParams&&) = delete;

    virtual void OnSync(const std::unique_ptr<RSRenderParams>& target);

    // dfx
    virtual std::string ToString() const;

    static void SetParentSurfaceMatrix(const Drawing::Matrix& parentSurfaceMatrix);
    static const Drawing::Matrix& GetParentSurfaceMatrix();

    // overrided surface params
#ifndef ROSEN_CROSS_PLATFORM
    virtual void SetBuffer(const sptr<SurfaceBuffer>& buffer, const Rect& damageRect) {}
    virtual sptr<SurfaceBuffer> GetBuffer() const { return nullptr; }
    virtual void SetPreBuffer(const sptr<SurfaceBuffer>& preBuffer) {}
    virtual sptr<SurfaceBuffer> GetPreBuffer() { return nullptr; }
    virtual void SetAcquireFence(const sptr<SyncFence>& acquireFence) {}
    virtual sptr<SyncFence> GetAcquireFence() const { return nullptr; }
    virtual const Rect& GetBufferDamage() const
    {
        static const Rect defaultRect = {};
        return defaultRect;
    }
#endif
    virtual const RSLayerInfo& GetLayerInfo() const;
    virtual const RectI& GetAbsDrawRect() const
    {
        return absDrawRect_;
    }

    void SetAbsDrawRect(RectI& absRect)
    {
        absDrawRect_ = absRect;
    }

    // surface params
    virtual bool GetOcclusionVisible() const { return true; }
    virtual bool IsLeashWindow() const { return true; }
    virtual bool IsAppWindow() const { return false; }
    virtual bool GetHardwareEnabled() const { return false; }
    virtual bool GetNeedMakeImage() const { return false; }
    virtual bool GetHardCursorStatus() const { return false; }
    virtual bool GetLayerCreated() const { return false; }
    virtual bool GetLastFrameHardwareEnabled() const { return false; }
    virtual void SetLayerCreated(bool layerCreated) {}
    virtual void SetOldDirtyInSurface(const RectI& oldDirtyInSurface) {}
    virtual RectI GetOldDirtyInSurface() const { return {}; }

    // overrided by displayNode
    virtual std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& GetAllMainAndLeashSurfaceDrawables()
    {
        static std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> defaultSurfaceVector;
        return defaultSurfaceVector;
    }

    virtual Occlusion::Region GetVisibleRegion() const
    {
        return {};
    }

    virtual void SetRotationChanged(bool changed) {}
    virtual bool IsRotationChanged() const
    {
        return false;
    }

    virtual const ScreenInfo& GetScreenInfo() const;
    virtual uint64_t GetScreenId() const
    {
        return 0;
    }
    virtual ScreenRotation GetScreenRotation() const
    {
        return ScreenRotation::ROTATION_0;
    }
    virtual void SetTotalMatrix(const Drawing::Matrix& totalMatrix) {}
    virtual const Drawing::Matrix& GetTotalMatrix();
    virtual void SetNeedClient(bool needClient) {}
    virtual bool GetNeedClient() const { return false; }
    virtual bool GetFingerprint() { return false; }
    virtual void SetFingerprint(bool hasFingerprint) {}
    // virtual display params
    virtual DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr GetMirrorSourceDrawable();
    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr GetCloneSourceDrawable() const;
    void SetCloneSourceDrawable(DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr drawable);
    virtual bool GetSecurityDisplay() const { return true; }
    // canvas drawing node
    virtual bool IsNeedProcess() const { return true; }
    virtual void SetNeedProcess(bool isNeedProcess) {}
    virtual bool IsFirstLevelCrossNode() const { return isFirstLevelCrossNode_; }
    virtual void SetFirstLevelCrossNode(bool firstLevelCrossNode) { isFirstLevelCrossNode_ = firstLevelCrossNode; }
    CrossNodeOffScreenRenderDebugType GetCrossNodeOffScreenStatus() const
    {
        return isCrossNodeOffscreenOn_;
    }
    void SetCrossNodeOffScreenStatus(CrossNodeOffScreenRenderDebugType isCrossNodeOffScreenOn)
    {
        isCrossNodeOffscreenOn_ = isCrossNodeOffScreenOn;
    }

    void SetAbsRotation(float degree)
    {
        absRotation_ = degree;
    }

    float GetAbsRotation() const
    {
        return absRotation_;
    }

    bool HasUnobscuredUEC() const;
    void SetHasUnobscuredUEC(bool flag);

    // [Attention] Only used in PC window resize scene now
    void EnableWindowKeyFrame(bool enable);
    bool IsWindowKeyFrameEnabled() const;
    void SetLinkedRootNodeDrawable(DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr drawable);
    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr GetLinkedRootNodeDrawable();
    void SetNeedSwapBuffer(bool needSwapBuffer);
    bool GetNeedSwapBuffer() const;
    void SetCacheNodeFrameRect(const Drawing::RectF& cacheNodeFrameRect);
    const Drawing::RectF& GetCacheNodeFrameRect() const;

    void SetIsOnTheTree(bool isOnTheTree);
    bool GetIsOnTheTree() const;

protected:
    bool needSync_ = false;
    std::bitset<RSRenderParamsDirtyType::MAX_DIRTY_TYPE> dirtyType_;

private:
    NodeId id_;
    RSRenderParamsType paramsType_ = RSRenderParamsType::RS_PARAM_DEFAULT;
    RSRenderNodeType renderNodeType_ = RSRenderNodeType::RS_NODE;
    Drawing::Matrix matrix_;
    Drawing::RectF boundsRect_;
    Drawing::RectF frameRect_;
    float alpha_ = 1.0f;
    float globalAlpha_ = 1.0f;
    // this rect should map display coordination
    RectF localDrawRect_;
    RectI absDrawRect_;
    Vector2f cacheSize_;
    Gravity frameGravity_ = Gravity::CENTER;
    // default 1.0f means max available headroom
    float hdrBrightness_ = 1.0f;
    bool freezeFlag_ = false;
    bool childHasVisibleEffect_ = false;
    bool childHasVisibleFilter_ = false;
    bool hasSandBox_ = false;
    bool isDrawingCacheChanged_ = false;
    std::atomic_bool isNeedUpdateCache_ = false;
    bool drawingCacheIncludeProperty_ = false;
    bool isInBlackList_ = false;
    bool isSnapshotSkipLayer_ = false;
    bool shouldPaint_ = false;
    bool contentEmpty_  = false;
    std::atomic_bool canvasDrawingNodeSurfaceChanged_ = false;
    bool alphaOffScreen_ = false;
    Drawing::Rect shadowRect_;
    RSDrawingCacheType drawingCacheType_ = RSDrawingCacheType::DISABLED_CACHE;
    DirtyRegionInfoForDFX dirtyRegionInfoForDFX_;
    std::shared_ptr<RSFilter> foregroundFilterCache_ = nullptr;
    bool isOpincSuggestFlag_ = false;
    bool isOpincSupportFlag_ = false;
    bool isOpincRootFlag_ = false;
    bool isOpincStateChanged_ = false;
    bool startingWindowFlag_ = false;
    bool needFilter_ = false;
    bool effectNodeShouldPaint_ = false;
    bool hasGlobalCorner_ = false;
    bool hasBlurFilter_ = false;
    SurfaceParam surfaceParams_;
    NodeId firstLevelNodeId_ = INVALID_NODEID;
    NodeId uifirstRootNodeId_ = INVALID_NODEID;
    bool isFirstLevelCrossNode_ = false;
    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr cloneSourceDrawable_;
    CrossNodeOffScreenRenderDebugType isCrossNodeOffscreenOn_ = CrossNodeOffScreenRenderDebugType::ENABLE;
    // The angle at which the node rotates about the Z-axis
    float absRotation_ = 0.f;
    bool hasUnobscuredUEC_ = false;

    // [Attention] Only used in PC window resize scene now
    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr linkedRootNodeDrawable_;
    bool windowKeyframeEnabled_ = false;
    bool needSwapBuffer_ = false;
    Drawing::RectF cacheNodeFrameRect_;
    bool isRepaintBoundary_ = false;

    // used for DFX
    bool isOnTheTree_ = false;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PARAMS_RS_RENDER_PARAMS_H
