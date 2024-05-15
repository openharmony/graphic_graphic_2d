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

namespace OHOS::Rosen::DrawableV2 {

namespace {
constexpr int32_t BITMAP_CACHE_SIZE_MIN = 50;
constexpr int32_t REALDRAW_WIDTH_EX = 200;
constexpr int32_t OPINC_ROOT_TOTAL_MAX = 1;
}

bool RSRenderNodeDrawable::IsOpincRenderCacheEnable()
{
    return RSSystemProperties::GetDdgrOpincType() == OHOS::Rosen::DdgrOpincType::DDGR_AUTOCACHE;
}

bool RSRenderNodeDrawable::IsOpincRealDrawCacheEnable()
{
    return RSSystemProperties::IsOpincRealDrawCacheEnable();
}

bool RSRenderNodeDrawable::IsAutoCacheDebugEnable()
{
    return RSSystemProperties::GetAutoCacheDebugEnabled() && RSSystemProperties::IsDdgrOpincEnable();
}

void RSRenderNodeDrawable::OpincCalculateBefore(Drawing::Canvas& canvas,
    const RSRenderParams& params, bool& isOpincDropNodeExt)
{
    isOpincDropNodeExtTemp_ = isOpincDropNodeExt;
    isOpincCaculateStart_ = false;
    if (IsOpincRealDrawCacheEnable() && IsOpListDrawAreaEnable()) {
        isOpincCaculateStart_ = canvas.OpCalculateBefore(params.GetMatrix());
        isOpincDropNodeExt = false;
    }
}

void RSRenderNodeDrawable::OpincCalculateAfter(Drawing::Canvas& canvas, bool& isOpincDropNodeExt)
{
    if (isOpincCaculateStart_) {
        isOpincCaculateStart_ = false;
        auto localBound =
            Drawing::Rect(0.f, 0.f, (float)(1260 + REALDRAW_WIDTH_EX), (float)2720);
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
    if (params.OpincGetCacheChangeState()) {
        DrawableCacheStateReset(params);
    }
    return isOpincDropNodeExt && (!IsOpincRootNode());
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
    rootNodeStragyType_ = NodeStragyType::CACHE_NONE;
    temNodeStragyType_ = NodeStragyType::CACHE_NONE;
    recordState_ = NodeRecordState::RECORD_NONE;
    opListDrawAreas_.ResetOpInfo();
    isOpincRootNode_ = false;
    opCanCache_ = false;
    params.OpincSetCachedMark(false);
}

bool RSRenderNodeDrawable::IsOpListDrawAreaEnable()
{
    return (rootNodeStragyType_ == NodeStragyType::DDGR_AUTOCACHE) &&
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
    rootNodeStragyType_ = NodeStragyType::CACHE_DISABLE;
    temNodeStragyType_ = NodeStragyType::CACHE_DISABLE;
    isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_DISABLE;
    if (opCanCache_) {
        opCanCache_ = false;
        opListDrawAreas_.ResetOpInfo();
    }
}

bool RSRenderNodeDrawable::BeforeDrawCacheProcessChildNode(NodeStragyType& cacheStragy,
    RSRenderParams& params)
{
#ifdef DDGR_ENABLE_FEATURE_OPINC_DFX
    RS_TRACE_NAME_FMT("BeforeDrawCacheProcessChildNode cs:%d rs:%d csBak:%d",
        cacheStragy, recordState_, temNodeStragyType_);
#endif
    // find root node
    if (cacheStragy != NodeStragyType::CACHE_NONE) {
        if (recordState_ == NodeRecordState::RECORD_CACHED &&
            rootNodeStragyType_ == NodeStragyType::DDGR_AUTOCACHE) {
            DrawableCacheStateReset(params);
        }
        if (rootNodeStragyType_ == NodeStragyType::CACHE_DISABLE) {
            NodeCacheStateDisable();
        }
        return false;
    }
    return true;
}

void RSRenderNodeDrawable::BeforeDrawCacheFindRootNode(Drawing::Canvas& canvas,
    const RSRenderParams& params, bool& isOpincDropNodeExt)
{
    auto isOffscreen = (canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::OFFSCREEN);
    auto size = params.GetCacheSize();
    if (!isOffscreen &&
        size.y_ > BITMAP_CACHE_SIZE_MIN && size.x_ > BITMAP_CACHE_SIZE_MIN) {
        if (IsOpincRealDrawCacheEnable()) {
            recordState_ = NodeRecordState::RECORD_NONE;
            rootNodeStragyType_ = NodeStragyType::CACHE_NONE;
        }
    }
#ifdef DDGR_ENABLE_FEATURE_OPINC_DFX
    RS_TRACE_NAME_FMT("BeforeDrawCacheFindRootNode rootS:%d xy:%d", rootNodeStragyType_,
        (size.y_ > BITMAP_CACHE_SIZE_MIN && size.x_ > BITMAP_CACHE_SIZE_MIN));
#endif
}

void RSRenderNodeDrawable::BeforeDrawCache(NodeStragyType& cacheStragy,
    Drawing::Canvas& canvas, RSRenderParams& params, bool& isOpincDropNodeExt)
{
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

void RSRenderNodeDrawable::AfterDrawCache(NodeStragyType& cacheStragy,
    Drawing::Canvas& canvas, RSRenderParams& params, bool& isOpincDropNodeExt, int& opincRootTotalCount)
{
    OpincCalculateAfter(canvas, isOpincDropNodeExt);
    if (rootNodeStragyType_ == NodeStragyType::DDGR_AUTOCACHE && recordState_ == NodeRecordState::RECORD_CALCULATE) {
        bool isOnlyTranslate = false;
        auto totalMatrix = canvas.GetTotalMatrix();
        auto rootAlpha = canvas.GetAlpha();
        if (IsTranslate(totalMatrix) && (rootAlpha == 0.0f || rootAlpha == 1.0f)) {
            isOnlyTranslate = true;
        }
        if (IsOpincRealDrawCacheEnable()) {
            if (isDrawAreaEnable_ == DrawAreaEnableState::DRAW_AREA_ENABLE && isOnlyTranslate) {
                recordState_ = NodeRecordState::RECORD_CACHING;
            } else if (isDrawAreaEnable_ == DrawAreaEnableState::DRAW_AREA_DISABLE) {
                NodeCacheStateDisable();
            }
        }
    } else if (rootNodeStragyType_ == NodeStragyType::DDGR_AUTOCACHE &&
        recordState_ == NodeRecordState::RECORD_CACHING) {
        if (opincRootTotalCount < OPINC_ROOT_TOTAL_MAX && (!params.OpincGetCachedMark())) {
            opincRootTotalCount++;
            params.OpincSetCachedMark(true);
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
        " S:" + (rootNodeStragyType_ == NodeStragyType::DDGR_AUTOCACHE ? "img" : "op") +
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
    AppendFormat(ret, "%llx, rootF:%d record:%d rootS:%d opCan:%d isRD:%d",
        params->GetId(), params->OpincGetRootFlag(),
        recordState_, rootNodeStragyType_, opCanCache_, isDrawAreaEnable_);
    auto& info = opListDrawAreas_.GetOpInfo();
    AppendFormat(ret, " opNum%d", info.num, info.percent);
    auto bounds = params->GetBounds();
    AppendFormat(ret, ", rr{%.1f %.1f %.1f %.1f}",
        0.f, 0.f, bounds.GetWidth(), bounds.GetHeight());
    AppendFormat(ret, ", ur{%.1f %.1f %.1f %.1f}",
        unionRect.GetLeft(), unionRect.GetTop(), unionRect.GetWidth(), unionRect.GetHeight(),);
#endif
    return ret;
}
} // namespace OHOS::Rosen::DrawableV2
