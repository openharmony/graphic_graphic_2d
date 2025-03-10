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

#include "drawable/rs_render_node_drawable.h"
#ifdef DDGR_ENABLE_FEATURE_OPINC_DFX
#include "string_utils.h"
#endif
#include "common/rs_optional_trace.h"
#include "params/rs_render_params.h"
namespace OHOS::Rosen::DrawableV2 {

namespace {
constexpr int32_t BITMAP_CACHE_SIZE_MIN = 50;
constexpr int32_t REALDRAW_WIDTH_EX = 200;
constexpr int32_t OPINC_ROOT_TOTAL_MAX = 1;
constexpr int32_t OPINC_CACHE_HEIGHT_THRESHOLD = 2720;
constexpr int32_t OPINC_CACHE_WIDTH_MAX = 1460;
constexpr int32_t OPINC_CACHE_SIZE_MAX = 1314000;
}

bool RSRenderNodeDrawable::ShouldPaint() const
{
#ifdef RS_ENABLE_GPU
    return LIKELY(renderParams_ != nullptr) && renderParams_->GetShouldPaint();
#else
    return false;
#endif
}

void RSRenderNodeDrawable::SetAutoCacheEnable(bool autoCacheEnable)
{
    autoCacheEnable_ = autoCacheEnable;
}

bool RSRenderNodeDrawable::IsAutoCacheDebugEnable()
{
    return RSSystemProperties::GetAutoCacheDebugEnabled() && autoCacheEnable_;
}

void RSRenderNodeDrawable::OpincCalculateBefore(Drawing::Canvas& canvas,
    const RSRenderParams& params, bool& isOpincDropNodeExt)
{
#ifdef RS_ENABLE_GPU
    isOpincDropNodeExtTemp_ = isOpincDropNodeExt;
    isOpincCaculateStart_ = false;
    if (autoCacheEnable_ && IsOpListDrawAreaEnable()) {
        isOpincCaculateStart_ = canvas.OpCalculateBefore(params.GetMatrix());
        isOpincDropNodeExt = false;
    }
#endif
}

void RSRenderNodeDrawable::OpincCalculateAfter(Drawing::Canvas& canvas, bool& isOpincDropNodeExt)
{
    if (isOpincCaculateStart_) {
        isOpincCaculateStart_ = false;
        auto localBound =
            Drawing::Rect(0.f, 0.f, (float)(OPINC_CACHE_WIDTH_MAX), (float)OPINC_CACHE_HEIGHT_THRESHOLD);
        auto drawAreaTemp = canvas.OpCalculateAfter(localBound);
        isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_DISABLE;
        opCanCache_ = false;
        if (drawAreaTemp) {
            opCanCache_ = drawAreaTemp->GetOpInfo().canReUseCache;
        }
        if (opCanCache_) {
            opListDrawAreas_ = std::move(*drawAreaTemp);
            isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
        }
    }
    isOpincDropNodeExt = isOpincDropNodeExtTemp_;
}

bool RSRenderNodeDrawable::PreDrawableCacheState(RSRenderParams& params, bool& isOpincDropNodeExt)
{
#ifdef RS_ENABLE_GPU
    if (params.OpincGetCacheChangeState()) {
        RS_OPTIONAL_TRACE_NAME_FMT("OpincGetCacheChangeState Changed %llx", GetId());
        DrawableCacheStateReset(params);
    }
    return isOpincDropNodeExt && (!IsOpincRootNode());
#else
    return false;
#endif
}

void RSRenderNodeDrawable::OpincCanvasUnionTranslate(RSPaintFilterCanvas& canvas)
{
    if (!IsComputeDrawAreaSucc()) {
        return;
    }
    auto& unionRect = GetOpListUnionArea();
    canvas.Translate(-unionRect.GetLeft(), -unionRect.GetTop());
}

void RSRenderNodeDrawable::ResumeOpincCanvasTranslate(RSPaintFilterCanvas& canvas)
{
    if (!IsComputeDrawAreaSucc()) {
        return;
    }
    auto& unionRect = GetOpListUnionArea();
    canvas.Translate(unionRect.GetLeft(), unionRect.GetTop());
}

void RSRenderNodeDrawable::DrawableCacheStateReset(RSRenderParams& params)
{
    isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_INIT;
    rootNodeStragyType_ = NodeStrategyType::CACHE_NONE;
    temNodeStragyType_ = NodeStrategyType::CACHE_NONE;
    recordState_ = NodeRecordState::RECORD_NONE;
    opListDrawAreas_.ResetOpInfo();
    isOpincRootNode_ = false;
    opCanCache_ = false;
    isOpincMarkCached_ = false;
}

bool RSRenderNodeDrawable::IsOpListDrawAreaEnable()
{
    return (rootNodeStragyType_ == NodeStrategyType::OPINC_AUTOCACHE) &&
        (recordState_ == NodeRecordState::RECORD_CALCULATE);
}

bool RSRenderNodeDrawable::IsTranslate(Drawing::Matrix& mat)
{
    return (mat.Get(Drawing::Matrix::SCALE_X) == 1.0f) && (mat.Get(Drawing::Matrix::SCALE_Y) == 1.0f) &&
        (mat.Get(Drawing::Matrix::SKEW_X) == 0.0f) && (mat.Get(Drawing::Matrix::SKEW_Y) == 0.0f);
}

void RSRenderNodeDrawable::NodeCacheStateDisable()
{
    recordState_ = NodeRecordState::RECORD_DISABLE;
    rootNodeStragyType_ = NodeStrategyType::CACHE_DISABLE;
    temNodeStragyType_ = NodeStrategyType::CACHE_DISABLE;
    isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_DISABLE;
    if (opCanCache_) {
        opCanCache_ = false;
        opListDrawAreas_.ResetOpInfo();
    }
}

bool RSRenderNodeDrawable::BeforeDrawCacheProcessChildNode(NodeStrategyType& cacheStragy,
    RSRenderParams& params)
{
#ifdef RS_ENABLE_GPU
#ifdef DDGR_ENABLE_FEATURE_OPINC_DFX
    RS_TRACE_NAME_FMT("BeforeDrawCacheProcessChildNode cs:%d rs:%d csBak:%d",
        cacheStragy, recordState_, temNodeStragyType_);
#endif
    // find root node
    if (cacheStragy != NodeStrategyType::CACHE_NONE || !params.OpincGetRootFlag()) {
        if (recordState_ == NodeRecordState::RECORD_CACHED &&
            rootNodeStragyType_ == NodeStrategyType::OPINC_AUTOCACHE) {
            DrawableCacheStateReset(params);
        }
        if (rootNodeStragyType_ == NodeStrategyType::CACHE_DISABLE) {
            NodeCacheStateDisable();
        }
        return false;
    }
    return true;
#else
    return false;
#endif
}

void RSRenderNodeDrawable::BeforeDrawCacheFindRootNode(Drawing::Canvas& canvas,
    const RSRenderParams& params, bool& isOpincDropNodeExt)
{
#ifdef RS_ENABLE_GPU
    if (autoCacheEnable_ && !params.OpincGetRootFlag()) {
        return;
    }
    auto size = params.GetCacheSize();
    if (size.x_ > OPINC_CACHE_WIDTH_MAX || size.y_ > OPINC_CACHE_HEIGHT_THRESHOLD) {
        RS_TRACE_NAME_FMT("opinc oversize: width:%d, height:%d", size.x_, size.y_);
        return;
    }
    auto isOffscreen = (canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::OFFSCREEN);
    if (!isOffscreen &&
        size.y_ > BITMAP_CACHE_SIZE_MIN && size.x_ > BITMAP_CACHE_SIZE_MIN &&
        size.x_ * size.y_ < OPINC_CACHE_SIZE_MAX) {
        recordState_ = NodeRecordState::RECORD_CALCULATE;
        rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    }
#ifdef DDGR_ENABLE_FEATURE_OPINC_DFX
    RS_TRACE_NAME_FMT("BeforeDrawCacheFindRootNode rootS:%d xy:%d", rootNodeStragyType_,
        (size.y_ > BITMAP_CACHE_SIZE_MIN && size.x_ > BITMAP_CACHE_SIZE_MIN));
#endif
#endif
}

void RSRenderNodeDrawable::BeforeDrawCache(NodeStrategyType& cacheStragy,
    Drawing::Canvas& canvas, RSRenderParams& params, bool& isOpincDropNodeExt)
{
    if (!autoCacheEnable_) {
        return;
    }
    temNodeStragyType_ = cacheStragy;
    if (!BeforeDrawCacheProcessChildNode(cacheStragy, params)) {
        OpincCalculateBefore(canvas, params, isOpincDropNodeExt);
        return;
    }
    switch (recordState_) {
        case NodeRecordState::RECORD_NONE:
            // find root node
            BeforeDrawCacheFindRootNode(canvas, params, isOpincDropNodeExt);
            break;
        case NodeRecordState::RECORD_CALCULATE:
            // cal img
            break;
        case NodeRecordState::RECORD_CACHING:
            // recording
            break;
        case NodeRecordState::RECORD_CACHED:
            reuseCount_++;
            break;
        default:
            break;
    }
    cacheStragy = rootNodeStragyType_;
    OpincCalculateBefore(canvas, params, isOpincDropNodeExt);
}

bool RSRenderNodeDrawable::IsOpincNodeInScreenRect(RSRenderParams& params)
{
    auto nodeAbsRect = params.GetAbsDrawRect();
    RS_OPTIONAL_TRACE_NAME_FMT("opincNodeAbsRect{%d %d %d %d}, screenRect{%d %d %d %d}",
        nodeAbsRect.GetLeft(), nodeAbsRect.GetTop(), nodeAbsRect.GetRight(), nodeAbsRect.GetBottom(),
        screenRectInfo_.GetLeft(), screenRectInfo_.GetTop(), screenRectInfo_.GetRight(), screenRectInfo_.GetBottom());
    if (!nodeAbsRect.IsEmpty() && nodeAbsRect.IsInsideOf(screenRectInfo_)) {
        return true;
    }
    return false;
}

void RSRenderNodeDrawable::AfterDrawCache(NodeStrategyType& cacheStragy,
    Drawing::Canvas& canvas, RSRenderParams& params, bool& isOpincDropNodeExt, int& opincRootTotalCount)
{
    if (!autoCacheEnable_) {
        return;
    }
    OpincCalculateAfter(canvas, isOpincDropNodeExt);
    if (rootNodeStragyType_ == NodeStrategyType::OPINC_AUTOCACHE && recordState_ == NodeRecordState::RECORD_CALCULATE) {
        bool isOnlyTranslate = false;
        auto totalMatrix = canvas.GetTotalMatrix();
        auto rootAlpha = canvas.GetAlpha();
        if (IsTranslate(totalMatrix) && (rootAlpha == 0.0f || rootAlpha == 1.0f)) {
            isOnlyTranslate = true;
        }
        if (autoCacheEnable_) {
            if (isDrawAreaEnable_ == DrawAreaEnableState::DRAW_AREA_ENABLE && isOnlyTranslate) {
                recordState_ = NodeRecordState::RECORD_CACHING;
            } else if (isDrawAreaEnable_ == DrawAreaEnableState::DRAW_AREA_DISABLE) {
                NodeCacheStateDisable();
            }
        }
    } else if (rootNodeStragyType_ == NodeStrategyType::OPINC_AUTOCACHE &&
        recordState_ == NodeRecordState::RECORD_CACHING) {
        if ((opincRootTotalCount < OPINC_ROOT_TOTAL_MAX) && (!OpincGetCachedMark()) &&
            IsOpincNodeInScreenRect(params)) {
            opincRootTotalCount++;
            isOpincMarkCached_ = true;
            recordState_ = NodeRecordState::RECORD_CACHED;
            reuseCount_ = 0;
            isOpincRootNode_ = true;
        }
    }
    cacheStragy = temNodeStragyType_;
}

bool RSRenderNodeDrawable::DrawAutoCache(RSPaintFilterCanvas& canvas, Drawing::Image& image,
    const Drawing::SamplingOptions& samplingOption, Drawing::SrcRectConstraint constraint)
{
    if (!IsComputeDrawAreaSucc() || !opCanCache_) {
        return false;
    }
    auto& unionRect = opListDrawAreas_.GetOpInfo().unionRect;
    auto& drawRects = opListDrawAreas_.GetOpInfo().drawAreaRects;
    if (unionRect.IsEmpty() || drawRects.size() == 0) {
        return false;
    }
    for (auto& rect : drawRects) {
        auto srcRect = rect;
        srcRect.Offset(-unionRect.GetLeft(), -unionRect.GetTop());
        canvas.DrawImageRect(image, srcRect, rect, samplingOption, constraint);
    }
    RS_OPTIONAL_TRACE_NAME_FMT("opinc_size:%d", drawRects.size());
    return true;
}

void RSRenderNodeDrawable::DrawAutoCacheDfx(RSPaintFilterCanvas& canvas,
    std::vector<std::pair<RectI, std::string>>& autoCacheRenderNodeInfos)
{
#ifdef DDGR_ENABLE_FEATURE_OPINC_DFX
    if (!IsAutoCacheDebugEnable()) {
        return;
    }
    auto& unionRect = opListDrawAreas_.GetOpInfo().unionRect;
    auto& drawRects = opListDrawAreas_.GetOpInfo().drawAreaRects;
    if (unionRect.IsEmpty() || drawRects.size() == 0) {
        return;
    }
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color(0x807C7CD5));
    canvas.AttachBrush(brush);
    canvas.DrawRect(unionRect);
    canvas.DetachBrush();
    Drawing::Pen pen;
    pen.SetColor(Drawing::Color(0x80FF00FF));
    pen.SetWidth(1);
    canvas.AttachPen(pen);
    for (auto& rect : drawRects) {
        canvas.DrawRect(rect);
    }
    canvas.DetachPen();
#else
    if (!IsAutoCacheDebugEnable()) {
        return;
    }
    Drawing::Rect dst;
    canvas.GetTotalMatrix().MapRect(dst, opListDrawAreas_.GetOpInfo().unionRect);
    RectI dfxRect(static_cast<int>(dst.GetLeft()), static_cast<int>(dst.GetTop()),
        static_cast<int>(dst.GetWidth()), static_cast<int>(dst.GetHeight()));
    auto& info = opListDrawAreas_.GetOpInfo();
    std::string extra =
        "Re:" + std::to_string(reuseCount_) +
        " S:" + (rootNodeStragyType_ == NodeStrategyType::OPINC_AUTOCACHE ? "img" : "op") +
        " Op" + std::to_string(info.num) +
        " Pe" + std::to_string(info.percent);
    autoCacheRenderNodeInfos.push_back({dfxRect, extra});
#endif
}

// opinc dfx
std::string RSRenderNodeDrawable::GetNodeDebugInfo()
{
    std::string ret("");
#ifdef DDGR_ENABLE_FEATURE_OPINC_DFX
    const auto& params = GetRenderParams();
    if (!params) {
        return ret;
    }
    auto& unionRect = opListDrawAreas_.GetOpInfo().unionRect;
    AppendFormat(ret, "%llx, rootF:%d record:%d rootS:%d opCan:%d isRD:%d, GetOpDropped:%d, isOpincDropNodeExt:%d",
        params->GetId(), params->OpincGetRootFlag(),
        recordState_, rootNodeStragyType_, opCanCache_, isDrawAreaEnable_, GetOpDropped(), isOpincDropNodeExt_);
    auto& info = opListDrawAreas_.GetOpInfo();
    AppendFormat(ret, " opNum%d", info.num, info.percent);
    auto bounds = params->GetBounds();
    AppendFormat(ret, ", rr{%.1f %.1f %.1f %.1f}",
        0.f, 0.f, bounds.GetWidth(), bounds.GetHeight());
    AppendFormat(ret, ", ur{%.1f %.1f %.1f %.1f}",
        unionRect.GetLeft(), unionRect.GetTop(), unionRect.GetWidth(), unionRect.GetHeight());
#endif
    return ret;
}
} // namespace OHOS::Rosen::DrawableV2
