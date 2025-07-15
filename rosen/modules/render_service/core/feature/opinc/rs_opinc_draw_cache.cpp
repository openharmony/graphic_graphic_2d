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

#include "rs_opinc_draw_cache.h"
#ifdef DDGR_ENABLE_FEATURE_OPINC_DFX
#include "string_utils.h"
#endif
#include "common/rs_optional_trace.h"
#include "feature_cfg/feature_param/performance_feature/opinc_param.h"
#include "feature/opinc/rs_opinc_manager.h"
#include "params/rs_render_params.h"
#include "string_utils.h"
namespace OHOS::Rosen::DrawableV2 {

namespace {
constexpr int32_t BITMAP_CACHE_SIZE_MIN = 50;
constexpr int32_t REALDRAW_WIDTH_EX = 200;
constexpr int32_t OPINC_ROOT_TOTAL_MAX = 1;
constexpr float PERCENT = 100.f;
constexpr int32_t BORDER_WIDTH = 6;
constexpr int32_t MARGIN = 20;
constexpr float RECT_PEN_ALPHA = 0.2f;
constexpr float DFX_FONT_SIZE = 30.f;
}

bool RSOpincDrawCache::IsAutoCacheEnable()
{
    return RSOpincManager::Instance().GetOPIncSwitch();
}

bool RSOpincDrawCache::IsAutoCacheDebugEnable()
{
    return RSSystemProperties::GetAutoCacheDebugEnabled() && IsAutoCacheEnable();
}

int32_t RSOpincDrawCache::GetOpincCacheMaxWidth() const
{
    return static_cast<int32_t>(std::ceil(static_cast<float>(screenRectInfo_.GetWidth()) *
        OPIncParam::GetCacheWidthThresholdPercentValue() / PERCENT));
}

int32_t RSOpincDrawCache::GetOpincCacheMaxHeight() const
{
    return screenRectInfo_.GetHeight();
}

void RSOpincDrawCache::OpincCalculateBefore(Drawing::Canvas& canvas,
    const RSRenderParams& params, bool& isOpincDropNodeExt)
{
#ifdef RS_ENABLE_GPU
    isOpincDropNodeExtTemp_ = isOpincDropNodeExt;
    isOpincCaculateStart_ = false;
    if (IsAutoCacheEnable() && IsOpListDrawAreaEnable()) {
        isOpincCaculateStart_ = canvas.OpCalculateBefore(params.GetMatrix());
        isOpincDropNodeExt = false;
    }
#endif
}

void RSOpincDrawCache::OpincCalculateAfter(Drawing::Canvas& canvas, bool& isOpincDropNodeExt)
{
    if (isOpincCaculateStart_) {
        isOpincCaculateStart_ = false;
        auto localBound = Drawing::Rect(0.f, 0.f, static_cast<float>(GetOpincCacheMaxWidth()),
            static_cast<float>(GetOpincCacheMaxHeight()));
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

bool RSOpincDrawCache::PreDrawableCacheState(RSRenderParams& params, bool& isOpincDropNodeExt)
{
#ifdef RS_ENABLE_GPU
    if (params.OpincGetCacheChangeState()) {
        RS_OPTIONAL_TRACE_NAME_FMT("OpincGetCacheChangeState Changed %llx", params.GetId());
        DrawableCacheStateReset(params);
    }
    return isOpincDropNodeExt && (!IsOpincRootNode());
#else
    return false;
#endif
}

void RSOpincDrawCache::OpincCanvasUnionTranslate(RSPaintFilterCanvas& canvas)
{
    if (!IsComputeDrawAreaSucc()) {
        return;
    }
    auto& unionRect = GetOpListUnionArea();
    canvas.Translate(-unionRect.GetLeft(), -unionRect.GetTop());
}

void RSOpincDrawCache::ResumeOpincCanvasTranslate(RSPaintFilterCanvas& canvas)
{
    if (!IsComputeDrawAreaSucc()) {
        return;
    }
    auto& unionRect = GetOpListUnionArea();
    canvas.Translate(unionRect.GetLeft(), unionRect.GetTop());
}

void RSOpincDrawCache::DrawableCacheStateReset(RSRenderParams& params)
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

bool RSOpincDrawCache::IsOpListDrawAreaEnable()
{
    return (rootNodeStragyType_ == NodeStrategyType::OPINC_AUTOCACHE) &&
        (recordState_ == NodeRecordState::RECORD_CALCULATE);
}

bool RSOpincDrawCache::IsTranslate(Drawing::Matrix& mat)
{
    return (mat.Get(Drawing::Matrix::SCALE_X) == 1.0f) && (mat.Get(Drawing::Matrix::SCALE_Y) == 1.0f) &&
        (mat.Get(Drawing::Matrix::SKEW_X) == 0.0f) && (mat.Get(Drawing::Matrix::SKEW_Y) == 0.0f);
}

void RSOpincDrawCache::NodeCacheStateDisable()
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

bool RSOpincDrawCache::BeforeDrawCacheProcessChildNode(RSRenderParams& params)
{
#ifdef RS_ENABLE_GPU
#ifdef DDGR_ENABLE_FEATURE_OPINC_DFX
    RS_TRACE_NAME_FMT("BeforeDrawCacheProcessChildNode cs:%d rs:%d csBak:%d",
        nodeCacheType_, recordState_, temNodeStragyType_);
#endif
    // find root node
    if (nodeCacheType_ != NodeStrategyType::CACHE_NONE || !params.OpincGetRootFlag()) {
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

void RSOpincDrawCache::BeforeDrawCacheFindRootNode(Drawing::Canvas& canvas,
    const RSRenderParams& params, bool& isOpincDropNodeExt)
{
#ifdef RS_ENABLE_GPU
    if (IsAutoCacheEnable() && !params.OpincGetRootFlag()) {
        return;
    }
    auto size = params.GetCacheSize();
    if (size.x_ > GetOpincCacheMaxWidth() || size.y_ > GetOpincCacheMaxHeight()) {
        RS_TRACE_NAME_FMT("opinc oversize: width:%d, height:%d", size.x_, size.y_);
        return;
    }
    auto isOffscreen = (canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::OFFSCREEN);
    if (!isOffscreen &&
        size.y_ > BITMAP_CACHE_SIZE_MIN && size.x_ > BITMAP_CACHE_SIZE_MIN) {
        recordState_ = NodeRecordState::RECORD_CALCULATE;
        rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    }
#ifdef DDGR_ENABLE_FEATURE_OPINC_DFX
    RS_TRACE_NAME_FMT("BeforeDrawCacheFindRootNode rootS:%d xy:%d", rootNodeStragyType_,
        (size.y_ > BITMAP_CACHE_SIZE_MIN && size.x_ > BITMAP_CACHE_SIZE_MIN));
#endif
#endif
}

void RSOpincDrawCache::BeforeDrawCache(Drawing::Canvas& canvas, RSRenderParams& params, bool& isOpincDropNodeExt)
{
    if (!IsAutoCacheEnable()) {
        return;
    }
    temNodeStragyType_ = nodeCacheType_;
    if (!BeforeDrawCacheProcessChildNode(params)) {
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
    nodeCacheType_ = rootNodeStragyType_;
    OpincCalculateBefore(canvas, params, isOpincDropNodeExt);
}

bool RSOpincDrawCache::IsOpincNodeInScreenRect(RSRenderParams& params)
{
    auto nodeAbsRect = params.GetAbsDrawRect();
    RS_OPTIONAL_TRACE_NAME_FMT("opincNodeAbsRect{%d %d %d %d}, screenRect{%d %d %d %d}",
        nodeAbsRect.GetLeft(), nodeAbsRect.GetTop(), nodeAbsRect.GetRight(), nodeAbsRect.GetBottom(),
        screenRectInfo_.GetLeft(), screenRectInfo_.GetTop(), screenRectInfo_.GetRight(), screenRectInfo_.GetBottom());
    if (!nodeAbsRect.IsEmpty() && nodeAbsRect.Intersect(screenRectInfo_)) {
        return true;
    }
    return false;
}

void RSOpincDrawCache::AfterDrawCache(Drawing::Canvas& canvas, RSRenderParams& params, bool& isOpincDropNodeExt,
    int& opincRootTotalCount)
{
    if (!IsAutoCacheEnable()) {
        return;
    }
    OpincCalculateAfter(canvas, isOpincDropNodeExt);
    if (rootNodeStragyType_ == NodeStrategyType::OPINC_AUTOCACHE && recordState_ == NodeRecordState::RECORD_CALCULATE) {
        bool isOnlyTranslate = false;
        auto totalMatrix = canvas.GetTotalMatrix();
        auto rootAlpha = canvas.GetAlpha();
        if (IsTranslate(totalMatrix) && (ROSEN_EQ(rootAlpha, 0.0f) || ROSEN_EQ(rootAlpha, 1.0f))) {
            isOnlyTranslate = true;
        }
        if (isDrawAreaEnable_ == DrawAreaEnableState::DRAW_AREA_ENABLE && isOnlyTranslate) {
            recordState_ = NodeRecordState::RECORD_CACHING;
        } else if (isDrawAreaEnable_ == DrawAreaEnableState::DRAW_AREA_DISABLE) {
            NodeCacheStateDisable();
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
    nodeCacheType_ = temNodeStragyType_;
}

bool RSOpincDrawCache::DrawAutoCache(RSPaintFilterCanvas& canvas, Drawing::Image& image,
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

void RSOpincDrawCache::DrawAutoCacheDfx(RSPaintFilterCanvas& canvas,
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

void RSOpincDrawCache::DrawOpincDisabledDfx(Drawing::Canvas& canvas, RSRenderParams& params)
{
    if (!IsAutoCacheDebugEnable() || !params.OpincIsSuggest() || opCanCache_) {
        return;
    }

    auto size = params.GetCacheSize();
    Drawing::Pen rectPen;
    rectPen.SetColor(Drawing::Color::COLOR_RED);
    rectPen.SetAntiAlias(true);
    rectPen.SetAlphaF(RECT_PEN_ALPHA);
    rectPen.SetWidth(BORDER_WIDTH);
    rectPen.SetJoinStyle(Drawing::Pen::JoinStyle::ROUND_JOIN);
    canvas.AttachPen(rectPen);
    canvas.DrawRect(Drawing::Rect(MARGIN, MARGIN, size.x_ - MARGIN, size.y_ - MARGIN));
    canvas.DetachPen();

    std::string info = "";
    AppendFormat(info, "support:%d rootF:%d rootStrategy:%d recordState:%d", params.OpincGetSupportFlag(),
        params.OpincGetRootFlag(), rootNodeStragyType_, recordState_);

    Drawing::Font font;
    font.SetSize(DFX_FONT_SIZE);
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromString(info.c_str(), font);
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color::COLOR_RED);
    canvas.AttachBrush(brush);
    canvas.DrawTextBlob(textBlob.get(), 10.f, 20.f);
    canvas.DetachBrush();
}
} // namespace OHOS::Rosen::DrawableV2
