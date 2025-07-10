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

#ifndef RS_OPINC_DRAW_CACHE_H
#define RS_OPINC_DRAW_CACHE_H

#include "common/rs_common_def.h"
#include "drawable/rs_render_node_drawable_adapter.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
class RSOpincDrawCache {
public:
    RSOpincDrawCache() = default;
    ~RSOpincDrawCache() = default;

    static bool IsAutoCacheEnable();

    static void SetScreenRectInfo(RectI info)
    {
        screenRectInfo_ = info;
    }

    static NodeStrategyType& GetNodeCacheType()
    {
        return nodeCacheType_;
    }

    static void SetNodeCacheType(NodeStrategyType type)
    {
        nodeCacheType_ = type;
    }

    // opinc switch
    bool IsAutoCacheDebugEnable();
    int32_t GetOpincCacheMaxWidth() const;
    int32_t GetOpincCacheMaxHeight() const;

    void OpincCalculateBefore(Drawing::Canvas& canvas,
        const RSRenderParams& params, bool& isOpincDropNodeExt);
    void OpincCalculateAfter(Drawing::Canvas& canvas, bool& isOpincDropNodeExt);
    void BeforeDrawCache(Drawing::Canvas& canvas, RSRenderParams& params, bool& isOpincDropNodeExt);
    void AfterDrawCache(Drawing::Canvas& canvas, RSRenderParams& params, bool& isOpincDropNodeExt,
        int& opincRootTotalCount);

    bool DrawAutoCache(RSPaintFilterCanvas& canvas, Drawing::Image& image,
        const Drawing::SamplingOptions& samplingOption, Drawing::SrcRectConstraint constraint);
    void DrawAutoCacheDfx(RSPaintFilterCanvas& canvas,
        std::vector<std::pair<RectI, std::string>>& autoCacheRenderNodeInfos);
    void DrawOpincDisabledDfx(Drawing::Canvas& canvas, RSRenderParams& params);
    void DrawableCacheStateReset(RSRenderParams& params);
    bool PreDrawableCacheState(RSRenderParams& params, bool& isOpincDropNodeExt);
    void OpincCanvasUnionTranslate(RSPaintFilterCanvas& canvas);
    void ResumeOpincCanvasTranslate(RSPaintFilterCanvas& canvas);

    bool IsOpListDrawAreaEnable();

    bool IsTranslate(Drawing::Matrix& mat);

    // opinc root state
    bool IsOpincRootNode()
    {
        return isOpincRootNode_;
    }

    const Drawing::Rect& GetOpListUnionArea()
    {
        return opListDrawAreas_.GetOpInfo().unionRect;
    }

    const Drawing::OpListHandle& GetOpListHandle()
    {
        return opListDrawAreas_;
    }

    bool IsComputeDrawAreaSucc()
    {
        return isDrawAreaEnable_ == DrawAreaEnableState::DRAW_AREA_ENABLE;
    }

    DrawAreaEnableState GetDrawAreaEnableState()
    {
        return isDrawAreaEnable_;
    }

    bool OpincGetCachedMark() const
    {
        return isOpincMarkCached_;
    }

    NodeRecordState GetRecordState() const
    {
        return recordState_;
    }

    NodeStrategyType GetRootNodeStrategyType() const
    {
        return rootNodeStragyType_;
    }

    bool IsOpCanCache() const
    {
        return opCanCache_;
    }
protected:
    thread_local static inline NodeStrategyType nodeCacheType_ = NodeStrategyType::CACHE_NONE;
    static inline RectI screenRectInfo_ = {0, 0, 0, 0};
private:
    // opinc cache state
    void NodeCacheStateDisable();
    bool BeforeDrawCacheProcessChildNode(RSRenderParams& params);
    void BeforeDrawCacheFindRootNode(Drawing::Canvas& canvas, const RSRenderParams& params, bool& isOpincDropNodeExt);
    bool IsOpincNodeInScreenRect(RSRenderParams& params);

    NodeRecordState recordState_ = NodeRecordState::RECORD_NONE;
    NodeStrategyType rootNodeStragyType_ = NodeStrategyType::CACHE_NONE;
    NodeStrategyType temNodeStragyType_ = NodeStrategyType::CACHE_NONE;
    DrawAreaEnableState isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_INIT;
    Drawing::OpListHandle opListDrawAreas_;
    bool opCanCache_ = false;
    int64_t reuseCount_ = 0;
    bool isOpincRootNode_ = false;
    bool isOpincDropNodeExtTemp_ = true;
    bool isOpincCaculateStart_ = false;
    bool isOpincMarkCached_ = false;
}; // RSOpincDrawCache
}
}
#endif // RS_OPINC_DRAW_CACHE_H