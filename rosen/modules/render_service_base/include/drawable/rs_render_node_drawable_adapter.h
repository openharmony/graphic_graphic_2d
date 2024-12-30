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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_RENDER_NODE_DRAWABLE_ADAPTER_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_RENDER_NODE_DRAWABLE_ADAPTER_H

#include <memory>
#include <map>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include "drawable/rs_property_drawable.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "modifier_ng/background/rs_useeffect_render_modifier.h"
#include "recording/recording_canvas.h"
#include "utils/rect.h"

#ifndef ROSEN_CROSS_PLATFORM
#include <iconsumer_surface.h>
#endif

namespace OHOS::Rosen {
class RSRenderNode;
class RSRenderParams;
class RSDisplayRenderNode;
class RSSurfaceRenderNode;
class RSSurfaceHandler;
class RSContext;
class RSDirtyRegionManager;
class RSDrawWindowCache;
namespace Drawing {
class Canvas;
}
namespace ModifierNG {
class RSUseEffectRenderModifier;
}

struct DrawCmdIndex {
    int8_t envForeGroundColorIndex_    = -1;
    int8_t shadowIndex_                = -1;
    int8_t renderGroupBeginIndex_      = -1;
    int8_t foregroundFilterBeginIndex_ = -1;
    int8_t backgroundColorIndex_       = -1;
    int8_t backgroundImageIndex_       = -1;
    int8_t backgroundFilterIndex_      = -1;
    int8_t useEffectIndex_             = -1;
    int8_t backgroundEndIndex_         = -1;
    int8_t childrenIndex_              = -1;
    int8_t contentIndex_               = -1;
    int8_t foregroundBeginIndex_       = -1;
    int8_t renderGroupEndIndex_        = -1;
    int8_t foregroundFilterEndIndex_   = -1;
    int8_t endIndex_                   = -1;
};
namespace DrawableV2 {
enum class SkipType : uint8_t {
    NONE = 0,
    SKIP_SHADOW = 1,
    SKIP_BACKGROUND_COLOR = 2
};

enum class DrawSkipType : uint8_t {
    NONE = 0,
    SHOULD_NOT_PAINT = 1,
    CANVAS_NULL = 2,
    RENDER_THREAD_PARAMS_NULL = 3,
    RENDER_PARAMS_NULL = 4,
    SURFACE_PARAMS_SKIP_DRAW = 5,
    RENDER_ENGINE_NULL = 6,
    FILTERCACHE_OCCLUSION_SKIP = 7,
    OCCLUSION_SKIP = 8,
    UI_FIRST_CACHE_SKIP = 9,
    PARALLEL_CANVAS_SKIP = 10,
    INIT_SURFACE_FAIL = 11,
    RENDER_PARAMS_OR_UNI_PARAMS_NULL = 12,
    SCREEN_OFF = 13,
    SCREEN_MANAGER_NULL = 14,
    SKIP_FRAME = 15,
    CREATE_PROCESSOR_FAIL = 16,
    INIT_FOR_RENDER_THREAD_FAIL = 17,
    WIRED_SCREEN_PROJECTION = 18,
    EXPAND_PROCESSOR_NULL = 19,
    MIRROR_DRAWABLE_SKIP = 20,
    DISPLAY_NODE_SKIP = 21,
    REQUEST_FRAME_FAIL = 22,
    SURFACE_NULL = 23,
    GENERATE_EFFECT_DATA_ON_DEMAND_FAIL = 24,
    RENDER_SKIP_IF_SCREEN_OFF = 25,
    HARD_CURSOR_ENAbLED = 26,
    CHECK_MATCH_AND_WAIT_NOTIFY_FAIL = 27,
    DEAL_WITH_CACHED_WINDOW = 28,
    MULTI_ACCESS = 29,
    RENDER_SKIP_IF_SCREEN_SWITCHING = 30,
    UI_FIRST_CACHE_FAIL = 31,
    SURFACE_SKIP_IN_MIRROR = 32,
};

class RSB_EXPORT RSRenderNodeDrawableAdapter : public std::enable_shared_from_this<RSRenderNodeDrawableAdapter> {
public:
    explicit RSRenderNodeDrawableAdapter(std::shared_ptr<const RSRenderNode>&& node);
    virtual ~RSRenderNodeDrawableAdapter();

    // delete
    RSRenderNodeDrawableAdapter(const RSRenderNodeDrawableAdapter&) = delete;
    RSRenderNodeDrawableAdapter(const RSRenderNodeDrawableAdapter&&) = delete;
    RSRenderNodeDrawableAdapter& operator=(const RSRenderNodeDrawableAdapter&) = delete;
    RSRenderNodeDrawableAdapter& operator=(const RSRenderNodeDrawableAdapter&&) = delete;

    using Ptr = RSRenderNodeDrawableAdapter*;
    using SharedPtr = std::shared_ptr<RSRenderNodeDrawableAdapter>;
    using WeakPtr = std::weak_ptr<RSRenderNodeDrawableAdapter>;

    virtual void Draw(Drawing::Canvas& canvas) = 0;
    virtual void DumpDrawableTree(int32_t depth, std::string& out, const RSContext& context) const;

    static SharedPtr OnGenerate(const std::shared_ptr<const RSRenderNode>& node);
    static SharedPtr GetDrawableById(NodeId id);
    static std::vector<RSRenderNodeDrawableAdapter::SharedPtr> GetDrawableVectorById(
        const std::unordered_set<NodeId>& ids);
    static SharedPtr OnGenerateShadowDrawable(
        const std::shared_ptr<const RSRenderNode>& node, const std::shared_ptr<RSRenderNodeDrawableAdapter>& drawable);

    inline const std::unique_ptr<RSRenderParams>& GetRenderParams() const
    {
        return renderParams_;
    }

    inline const std::unique_ptr<RSRenderParams>& GetUifirstRenderParams() const
    {
        return uifirstRenderParams_;
    }

    inline NodeId GetId() const
    {
        return nodeId_;
    }
    inline RSRenderNodeType GetNodeType() const
    {
        return nodeType_;
    }
    virtual std::shared_ptr<RSDirtyRegionManager> GetSyncDirtyManager() const
    {
        return nullptr;
    }

    using ClearSurfaceTask = std::function<void()>;
    void RegisterClearSurfaceFunc(ClearSurfaceTask task);
    void ResetClearSurfaceFunc();
    void TryClearSurfaceOnSync();

#ifndef ROSEN_CROSS_PLATFORM
    virtual void RegisterDeleteBufferListenerOnSync(sptr<IConsumerSurface> consumer) {}
#endif

    virtual bool IsNeedDraw() const
    {
        return false;
    }
    virtual void SetNeedDraw(bool flag) {}
    void SetSkip(SkipType type) { skipType_ = type; }
    SkipType GetSkipType() { return skipType_; }

    bool IsFilterCacheValidForOcclusion() const;
    const RectI GetFilterCachedRegion() const;

    void SetSkipCacheLayer(bool hasSkipCacheLayer);
    void SetChildInBlackList(bool hasChildInBlackList);

    size_t GetFilterNodeSize() const
    {
        return filterNodeSize_;
    }
    void ReduceFilterNodeSize()
    {
        if (filterNodeSize_ > 0) {
            --filterNodeSize_;
        }
    }
    struct FilterNodeInfo {
        FilterNodeInfo(NodeId nodeId, Drawing::Matrix matrix, std::vector<Drawing::RectI> rectVec)
            : nodeId_(nodeId), matrix_(matrix), rectVec_(rectVec) {};
        NodeId nodeId_ = 0;
        // Here, matrix_ and rectVec_ represent the transformation and FilterRect of the node relative to the off-screen
        Drawing::Matrix matrix_;
        std::vector<Drawing::RectI> rectVec_;
    };

    const std::vector<FilterNodeInfo>& GetfilterInfoVec() const
    {
        return filterInfoVec_;
    }
    const std::unordered_map<NodeId, Drawing::Matrix>& GetWithoutFilterMatrixMap() const
    {
        return withoutFilterMatrixMap_;
    }

    const std::unordered_map<NodeId, Drawing::Matrix>& GetUnobscuredUECMatrixMap() const
    {
        return unobscuredUECMatrixMap_;
    }

    void SetLastDrawnFilterNodeId(NodeId nodeId)
    {
        lastDrawnFilterNodeId_ = nodeId;
    }

    NodeId GetLastDrawnFilterNodeId() const
    {
        return lastDrawnFilterNodeId_;
    }

    virtual void Purge()
    {
        if (purgeFunc_) {
            purgeFunc_();
        }
    }

    virtual void SetUIExtensionNeedToDraw(bool needToDraw) {}

    virtual bool UIExtensionNeedToDraw() const
    {
        return false;
    }

    void SetDrawSkipType(DrawSkipType type) {
        drawSkipType_ = type;
    }

    DrawSkipType GetDrawSkipType() {
        return drawSkipType_;
    }

    inline bool DrawableTryLockForDraw()
    {
        bool expected = false;
        return isOnDraw_.compare_exchange_strong(expected, true);
    }

    inline void DrawableResetLock()
    {
        isOnDraw_.store(false);
    }

    virtual std::shared_ptr<Drawing::Image> Snapshot() const
    {
        return nullptr;
    }

    virtual RSRenderNodeDrawableType GetDrawableType() const
    {
        return RSRenderNodeDrawableType::UNKNOW;
    }

protected:
    // Util functions
    std::string DumpDrawableVec(const std::shared_ptr<RSRenderNode>& renderNode) const;
    bool QuickReject(Drawing::Canvas& canvas, const RectF& localDrawRect);
    bool HasFilterOrEffect() const;
    int ClipHoleForCacheSize(const RSRenderParams& params) const;

    // Draw functions
    void DrawAll(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;
    void DrawUifirstContentChildren(Drawing::Canvas& canvas, const Drawing::Rect& rect);
    void DrawBackground(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;
    void DrawLeashWindowBackground(Drawing::Canvas& canvas, const Drawing::Rect& rect,
        bool isStencilPixelOcclusionCullingEnabled = false, int64_t stencilVal = -1) const;
    void DrawContent(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;
    void DrawChildren(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;
    void DrawForeground(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;
    void ApplyForegroundColorIfNeed(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;

    // used for foreground filter
    void DrawBeforeCacheWithForegroundFilter(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;
    void DrawCacheWithForegroundFilter(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;
    void DrawAfterCacheWithForegroundFilter(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;

    // used for render group
    void DrawBackgroundWithoutFilterAndEffect(Drawing::Canvas& canvas, const RSRenderParams& params);
    void CheckShadowRectAndDrawBackground(Drawing::Canvas& canvas, const RSRenderParams& params);
    void DrawCacheWithProperty(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;
    void DrawBeforeCacheWithProperty(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;
    void DrawAfterCacheWithProperty(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;
    void CollectInfoForNodeWithoutFilter(Drawing::Canvas& canvas);
    void CollectInfoForUnobscuredUEC(Drawing::Canvas& canvas);

    // Note, the start is included, the end is excluded, so the range is [start, end)
    void DrawRangeImpl(Drawing::Canvas& canvas, const Drawing::Rect& rect, int8_t start, int8_t end) const;
    void DrawImpl(Drawing::Canvas& canvas, const Drawing::Rect& rect, int8_t index) const;

    // Register utils
    using Generator = Ptr (*)(std::shared_ptr<const RSRenderNode>);
    template<RSRenderNodeType type, Generator generator>
    class RenderNodeDrawableRegistrar {
    public:
        RenderNodeDrawableRegistrar()
        {
            RSRenderNodeDrawableAdapter::GeneratorMap.emplace(type, generator);
        }
    };

    const RSRenderNodeType nodeType_;
    // deprecated
    std::weak_ptr<const RSRenderNode> renderNode_;
    NodeId nodeId_;

    DrawCmdIndex uifirstDrawCmdIndex_;
    DrawCmdIndex drawCmdIndex_;
    std::unique_ptr<RSRenderParams> renderParams_;
    static std::unordered_map<NodeId, Drawing::Matrix> unobscuredUECMatrixMap_;
    std::shared_ptr<std::unordered_set<NodeId>> UECChildrenIds_ = std::make_shared<std::unordered_set<NodeId>>();
    std::unique_ptr<RSRenderParams> uifirstRenderParams_;
    std::vector<Drawing::RecordingCanvas::DrawFunc> uifirstDrawCmdList_;
    std::vector<Drawing::RecordingCanvas::DrawFunc> drawCmdList_;
    std::vector<FilterNodeInfo> filterInfoVec_;
    std::unordered_map<NodeId, Drawing::Matrix> withoutFilterMatrixMap_;
    size_t filterNodeSize_ = 0;
    std::shared_ptr<DrawableV2::RSFilterDrawable> backgroundFilterDrawable_ = nullptr;
    std::shared_ptr<DrawableV2::RSFilterDrawable> compositingFilterDrawable_ = nullptr;
    std::function<void()> purgeFunc_;
#ifdef ROSEN_OHOS
    static thread_local RSRenderNodeDrawableAdapter* curDrawingCacheRoot_;
#else
    static RSRenderNodeDrawableAdapter* curDrawingCacheRoot_;
#endif
    // if the node needs to avoid drawing cache because of some layers, such as the security layer...
    bool hasSkipCacheLayer_ = false;
    bool hasChildInBlackList_ = false;

    ClearSurfaceTask clearSurfaceTask_ = nullptr;
private:
    static void InitRenderParams(const std::shared_ptr<const RSRenderNode>& node,
                            std::shared_ptr<RSRenderNodeDrawableAdapter>& sharedPtr);
    static std::map<RSRenderNodeType, Generator> GeneratorMap;
    static std::map<NodeId, WeakPtr> RenderNodeDrawableCache_;
    static inline std::mutex cacheMapMutex_;
    SkipType skipType_ = SkipType::NONE;
    int8_t GetSkipIndex() const;
    DrawSkipType drawSkipType_ = DrawSkipType::NONE;
    static void RemoveDrawableFromCache(const NodeId nodeId);
    void UpdateFilterInfoForNodeGroup(RSPaintFilterCanvas* curCanvas);
    NodeId lastDrawnFilterNodeId_ = 0;
    std::atomic<bool> isOnDraw_ = false;

    friend class OHOS::Rosen::RSRenderNode;
    friend class OHOS::Rosen::RSDisplayRenderNode;
    friend class OHOS::Rosen::RSSurfaceRenderNode;
    friend class RSRenderNodeShadowDrawable;
    friend class RSUseEffectDrawable;
    friend class RSRenderNodeDrawable;
    friend class OHOS::Rosen::RSDrawWindowCache;
    friend class ModifierNG::RSUseEffectRenderModifier;
};

// RSRenderNodeSingleDrawableLocker: tool class that ensures drawable is exclusively used at the same time.
class RSB_EXPORT RSRenderNodeSingleDrawableLocker {
public:
    RSRenderNodeSingleDrawableLocker() = delete;
    inline RSRenderNodeSingleDrawableLocker(RSRenderNodeDrawableAdapter* drawable)
        : drawable_(drawable), locked_(LIKELY(drawable != nullptr) && drawable->DrawableTryLockForDraw())
    {}
    inline ~RSRenderNodeSingleDrawableLocker()
    {
        if (LIKELY(locked_)) {
            drawable_->DrawableResetLock();
        }
    }
    inline bool IsLocked() const
    {
        return LIKELY(locked_);
    }
    struct MultiAccessReportInfo {
        bool drawableNotNull = false;
        bool paramNotNull = false;
        RSRenderNodeType nodeType = RSRenderNodeType::UNKNOW;
        NodeId nodeId = INVALID_NODEID;
        NodeId uifirstRootNodeId = INVALID_NODEID;
        NodeId firstLevelNodeId = INVALID_NODEID;
    };
    void DrawableOnDrawMultiAccessEventReport(const std::string& func) const;
private:
    RSRenderNodeDrawableAdapter* drawable_;
    const bool locked_;
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_RENDER_NODE_DRAWABLE_ADAPTER_H
