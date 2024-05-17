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

#include <cstdint>
#include <sys/types.h>

#include "rs_dirty_rects_dfx.h"
#include "rs_trace.h"

#include "drawable/rs_render_node_drawable.h"
#include "params/rs_display_render_params.h"
#include "params/rs_surface_render_params.h"
#include "platform/common/rs_log.h"

// fresh rate
#include "hgm_core.h"

#include "pipeline/rs_realtime_refresh_rate_manager.h"

namespace OHOS::Rosen {
namespace {
// DFX drawing alpha
constexpr float DFXFillAlpha = 0.2f;
constexpr float DFXFontSize = 24.f;
}

static const std::map<DirtyRegionType, std::string> DIRTY_REGION_TYPE_MAP {
    { DirtyRegionType::UPDATE_DIRTY_REGION, "UPDATE_DIRTY_REGION" },
    { DirtyRegionType::OVERLAY_RECT, "OVERLAY_RECT" },
    { DirtyRegionType::FILTER_RECT, "FILTER_RECT" },
    { DirtyRegionType::SHADOW_RECT, "SHADOW_RECT" },
    { DirtyRegionType::PREPARE_CLIP_RECT, "PREPARE_CLIP_RECT" },
    { DirtyRegionType::REMOVE_CHILD_RECT, "REMOVE_CHILD_RECT" },
    { DirtyRegionType::RENDER_PROPERTIES_RECT, "RENDER_PROPERTIES_RECT" },
    { DirtyRegionType::CANVAS_NODE_SKIP_RECT, "CANVAS_NODE_SKIP_RECT" },
    { DirtyRegionType::OUTLINE_RECT, "OUTLINE_RECT" },
};

void RSDirtyRectsDfx::OnDraw(std::shared_ptr<RSPaintFilterCanvas> canvas)
{
    if (!targetNode_) {
        RS_LOGE("RSDirtyRectsDfx::OnDraw target node is nullptr!");
        return;
    }
    auto& renderThreadParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!renderThreadParams) {
        RS_LOGE("RSDirtyRectsDfx::OnDraw render thread params is nullptr!");
        return;
    }
    if (!canvas) {
        RS_LOGE("RSDirtyRectsDfx::OnDraw canvas is nullptr!");
        return;
    }
    canvas_ = canvas;
    // the following code makes DirtyRegion visible, enable this method by turning on the dirtyregiondebug property
    if (renderThreadParams->isPartialRenderEnabled_) {
        if (renderThreadParams->isDirtyRegionDfxEnabled_) {
            DrawAllSurfaceDirtyRegionForDFX();
        }
        if (renderThreadParams->isTargetDirtyRegionDfxEnabled_) {
            DrawTargetSurfaceDirtyRegionForDFX();
        }
        if (renderThreadParams->isDisplayDirtyDfxEnabled_) {
            DrawDirtyRegionForDFX(targetNode_->GetSyncDirtyManager()->GetMergedDirtyRegions());
        }
    }

    if (renderThreadParams->isOpaqueRegionDfxEnabled_) {
        DrawAllSurfaceOpaqueRegionForDFX();
    }
    if (renderThreadParams->isVisibleRegionDfxEnabled_) {
        DrawTargetSurfaceVisibleRegionForDFX();
    }

    if (RSRealtimeRefreshRateManager::Instance().GetShowRefreshRateEnabled()) {
        DrawCurrentRefreshRate();
    }

    DrawableV2::RSRenderNodeDrawable::DrawDfxForCacheInfo(*canvas_);
}

void RSDirtyRectsDfx::DrawCurrentRefreshRate()
{
    RS_TRACE_NAME("RSUniRender::DrawCurrentRefreshRate");
    auto displayParams = static_cast<RSDisplayRenderParams*>(targetNode_->GetRenderParams().get());
    if (!displayParams) {
        return;
    }
    auto screenId = displayParams->GetScreenId();
    uint32_t currentRefreshRate = OHOS::Rosen::HgmCore::Instance().GetScreenCurrentRefreshRate(screenId);
    uint32_t realtimeRefreshRate = RSRealtimeRefreshRateManager::Instance().GetRealtimeRefreshRate();
    if (realtimeRefreshRate > currentRefreshRate) {
        realtimeRefreshRate = currentRefreshRate;
    }

    std::string info = std::to_string(currentRefreshRate) + " " + std::to_string(realtimeRefreshRate);
    std::shared_ptr<Drawing::Typeface> tf = Drawing::Typeface::MakeFromName("HarmonyOS Sans SC", Drawing::FontStyle());
    Drawing::Font font;
    font.SetSize(100); // 100:Scalar of setting font size
    font.SetTypeface(tf);
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromString(info.c_str(), font);

    Drawing::Brush brush;
    brush.SetColor(currentRefreshRate <= 60 ? SK_ColorRED : SK_ColorGREEN); // low refresh rate 60
    brush.SetAntiAlias(true);
    RSAutoCanvasRestore acr(canvas_);
    canvas_->AttachBrush(brush);
    auto rotation = displayParams->GetScreenRotation();
    if (RSSystemProperties::IsFoldScreenFlag() && screenId == 0) {
        rotation =
            (rotation == ScreenRotation::ROTATION_270 ? ScreenRotation::ROTATION_0
                                                      : static_cast<ScreenRotation>(static_cast<int>(rotation) + 1));
    }
    auto saveCount = canvas_->Save();
    if (rotation != ScreenRotation::ROTATION_0) {
        auto screenManager = CreateOrGetScreenManager();
        auto mainScreenInfo = screenManager->QueryScreenInfo(screenId);
        if (rotation == ScreenRotation::ROTATION_90) {
            canvas_->Rotate(-90, 0, 0); // 90 degree for text draw
            canvas_->Translate(-(static_cast<float>(mainScreenInfo.height)), 0);
        } else if (rotation == ScreenRotation::ROTATION_180) {
            // 180 degree for text draw
            canvas_->Rotate(-180, static_cast<float>(mainScreenInfo.width) / 2, // 2 half of screen width
                static_cast<float>(mainScreenInfo.height) / 2);                 // 2 half of screen height
        } else if (rotation == ScreenRotation::ROTATION_270) {
            canvas_->Rotate(-270, 0, 0); // 270 degree for text draw
            canvas_->Translate(0, -(static_cast<float>(mainScreenInfo.width)));
        } else {
            return;
        }
    }
    // 100.f:Scalar x of drawing TextBlob; 200.f:Scalar y of drawing TextBlob
    canvas_->DrawTextBlob(textBlob.get(), 100.f, 200.f);
    canvas_->RestoreToCount(saveCount);
    canvas_->DetachBrush();
}

void RSDirtyRectsDfx::DrawDirtyRectForDFX(
    const RectI& dirtyRect, const Drawing::Color color, const RSPaintStyle fillType, float alpha, int edgeWidth) const
{
    if (dirtyRect.width_ <= 0 || dirtyRect.height_ <= 0) {
        ROSEN_LOGD("DrawDirtyRectForDFX dirty rect is invalid.");
        return;
    }
    ROSEN_LOGD("DrawDirtyRectForDFX current dirtyRect = %{public}s", dirtyRect.ToString().c_str());
    auto rect = Drawing::Rect(
        dirtyRect.left_, dirtyRect.top_, dirtyRect.left_ + dirtyRect.width_, dirtyRect.top_ + dirtyRect.height_);
    std::string position = std::to_string(dirtyRect.left_) + ',' + std::to_string(dirtyRect.top_) + ',' +
                           std::to_string(dirtyRect.width_) + ',' + std::to_string(dirtyRect.height_);
    const int defaultTextOffsetX = edgeWidth;
    const int defaultTextOffsetY = 30; // text position has 30 pixelSize under the Rect
    Drawing::Pen rectPen;
    Drawing::Brush rectBrush;
    // font size: 24
    std::shared_ptr<Drawing::TextBlob> textBlob =
        Drawing::TextBlob::MakeFromString(position.c_str(), Drawing::Font(nullptr, DFXFontSize, 1.0f, 0.0f));
    if (fillType == RSPaintStyle::STROKE) {
        rectPen.SetColor(color);
        rectPen.SetAntiAlias(true);
        rectPen.SetAlphaF(alpha);
        rectPen.SetWidth(edgeWidth);
        rectPen.SetJoinStyle(Drawing::Pen::JoinStyle::ROUND_JOIN);
        canvas_->AttachPen(rectPen);
    } else {
        rectBrush.SetColor(color);
        rectBrush.SetAntiAlias(true);
        rectBrush.SetAlphaF(alpha);
        canvas_->AttachBrush(rectBrush);
    }
    canvas_->DrawRect(rect);
    canvas_->DetachPen();
    canvas_->DetachBrush();
    canvas_->AttachBrush(Drawing::Brush());
    canvas_->DrawTextBlob(textBlob.get(), dirtyRect.left_ + defaultTextOffsetX, dirtyRect.top_ + defaultTextOffsetY);
    canvas_->DetachBrush();
}

void RSDirtyRectsDfx::DrawDirtyRegionForDFX(const std::vector<RectI>& dirtyRects) const
{
    for (const auto& subRect : dirtyRects) {
        DrawDirtyRectForDFX(subRect, Drawing::Color::COLOR_BLUE, RSPaintStyle::STROKE, DFXFillAlpha);
    }
}

void RSDirtyRectsDfx::DrawAndTraceSingleDirtyRegionTypeForDFX(
    RSSurfaceRenderNode& node, DirtyRegionType dirtyType, bool isDrawn) const
{
    auto dirtyManager = node.GetSyncDirtyManager();
    auto matchType = DIRTY_REGION_TYPE_MAP.find(dirtyType);
    if (dirtyManager == nullptr || matchType == DIRTY_REGION_TYPE_MAP.end()) {
        return;
    }
    std::map<NodeId, RectI> dirtyInfo;
    std::map<RSRenderNodeType, std::pair<std::string, SkColor>> nodeConfig = {
        { RSRenderNodeType::CANVAS_NODE, std::make_pair("canvas", SK_ColorRED) },
        { RSRenderNodeType::SURFACE_NODE, std::make_pair("surface", SK_ColorGREEN) },
    };

    std::string subInfo;
    for (const auto& [nodeType, info] : nodeConfig) {
        dirtyManager->GetDirtyRegionInfo(dirtyInfo, nodeType, dirtyType);
        subInfo += (" " + info.first + "node amount: " + std::to_string(dirtyInfo.size()));
        for (const auto& [nid, rect] : dirtyInfo) {
            if (isDrawn) {
                DrawDirtyRectForDFX(rect, info.second, RSPaintStyle::STROKE, DFXFillAlpha);
            }
        }
    }
    RS_TRACE_NAME("DrawAndTraceSingleDirtyRegionTypeForDFX target surface node " + node.GetName() + " - id[" +
                  std::to_string(node.GetId()) + "] has dirtytype " + matchType->second + subInfo);
    ROSEN_LOGD("DrawAndTraceSingleDirtyRegionTypeForDFX target surface node %{public}s, id[%{public}" PRIu64 "]"
               "has dirtytype %{public}s%{public}s",
        node.GetName().c_str(), node.GetId(), matchType->second.c_str(), subInfo.c_str());
}

bool RSDirtyRectsDfx::DrawDetailedTypesOfDirtyRegionForDFX(RSSurfaceRenderNode& node) const
{
    auto dirtyRegionDebugType = RSUniRenderThread::Instance().GetRSRenderThreadParams()->dirtyRegionDebugType_;
    if (dirtyRegionDebugType < DirtyRegionDebugType::CUR_DIRTY_DETAIL_ONLY_TRACE) {
        return false;
    }
    if (dirtyRegionDebugType == DirtyRegionDebugType::CUR_DIRTY_DETAIL_ONLY_TRACE) {
        auto i = DirtyRegionType::UPDATE_DIRTY_REGION;
        for (; i < DirtyRegionType::TYPE_AMOUNT; i = static_cast<DirtyRegionType>(i + 1)) {
            DrawAndTraceSingleDirtyRegionTypeForDFX(node, i, false);
        }
        return true;
    }
    static const std::map<DirtyRegionDebugType, DirtyRegionType> DIRTY_REGION_DEBUG_TYPE_MAP {
        { DirtyRegionDebugType::UPDATE_DIRTY_REGION, DirtyRegionType::UPDATE_DIRTY_REGION },
        { DirtyRegionDebugType::OVERLAY_RECT, DirtyRegionType::OVERLAY_RECT },
        { DirtyRegionDebugType::FILTER_RECT, DirtyRegionType::FILTER_RECT },
        { DirtyRegionDebugType::SHADOW_RECT, DirtyRegionType::SHADOW_RECT },
        { DirtyRegionDebugType::PREPARE_CLIP_RECT, DirtyRegionType::PREPARE_CLIP_RECT },
        { DirtyRegionDebugType::REMOVE_CHILD_RECT, DirtyRegionType::REMOVE_CHILD_RECT },
        { DirtyRegionDebugType::RENDER_PROPERTIES_RECT, DirtyRegionType::RENDER_PROPERTIES_RECT },
        { DirtyRegionDebugType::CANVAS_NODE_SKIP_RECT, DirtyRegionType::CANVAS_NODE_SKIP_RECT },
        { DirtyRegionDebugType::OUTLINE_RECT, DirtyRegionType::OUTLINE_RECT },
    };
    auto matchType = DIRTY_REGION_DEBUG_TYPE_MAP.find(dirtyRegionDebugType);
    if (matchType != DIRTY_REGION_DEBUG_TYPE_MAP.end()) {
        DrawAndTraceSingleDirtyRegionTypeForDFX(node, matchType->second);
    }
    return true;
}

void RSDirtyRectsDfx::DrawSurfaceOpaqueRegionForDFX(RSSurfaceRenderNode& node) const
{
    const auto& opaqueRegionRects = node.GetOpaqueRegion().GetRegionRects();
    for (const auto& subRect : opaqueRegionRects) {
        DrawDirtyRectForDFX(subRect.ToRectI(), Drawing::Color::COLOR_GREEN, RSPaintStyle::FILL, DFXFillAlpha, 0);
    }
}

void RSDirtyRectsDfx::DrawAllSurfaceDirtyRegionForDFX() const
{
    const auto& visibleDirtyRects = dirtyRegion_.GetRegionRects();
    std::vector<RectI> rects;
    for (auto& rect : visibleDirtyRects) {
        rects.emplace_back(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_);
    }
    DrawDirtyRegionForDFX(rects);

    // draw display dirtyregion with red color
    RectI dirtySurfaceRect = targetNode_->GetSyncDirtyManager()->GetDirtyRegion();
    DrawDirtyRectForDFX(dirtySurfaceRect, Drawing::Color::COLOR_RED, RSPaintStyle::STROKE, DFXFillAlpha);
}

void RSDirtyRectsDfx::DrawAllSurfaceOpaqueRegionForDFX() const
{
    auto params = static_cast<RSDisplayRenderParams*>(targetNode_->GetRenderParams().get());
    auto& curAllSurfaces = params->GetAllMainAndLeashSurfaces();
    for (auto it = curAllSurfaces.rbegin(); it != curAllSurfaces.rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode->IsMainWindowType()) {
            DrawSurfaceOpaqueRegionForDFX(*surfaceNode);
        }
    }
}

void RSDirtyRectsDfx::DrawTargetSurfaceDirtyRegionForDFX() const
{
    auto params = static_cast<RSDisplayRenderParams*>(targetNode_->GetRenderParams().get());
    auto& curAllSurfaces = params->GetAllMainAndLeashSurfaces();
    for (auto it = curAllSurfaces.rbegin(); it != curAllSurfaces.rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow()) {
            continue;
        }
        if (CheckIfSurfaceTargetedForDFX(surfaceNode->GetName())) {
            if (DrawDetailedTypesOfDirtyRegionForDFX(*surfaceNode)) {
                continue;
            }
            const auto& visibleDirtyRegions = surfaceNode->GetVisibleDirtyRegion().GetRegionRects();
            std::vector<RectI> rects;
            for (auto& rect : visibleDirtyRegions) {
                rects.emplace_back(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_);
            }
            const auto& visibleRegions = surfaceNode->GetVisibleRegion().GetRegionRects();
            auto displayDirtyRegion = targetNode_->GetSyncDirtyManager()->GetDirtyRegion();
            for (auto& rect : visibleRegions) {
                auto visibleRect = RectI(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_);
                auto intersectRegion = displayDirtyRegion.IntersectRect(visibleRect);
                rects.emplace_back(intersectRegion);
            }
            DrawDirtyRegionForDFX(rects);
        }
    }
}

void RSDirtyRectsDfx::DrawTargetSurfaceVisibleRegionForDFX() const
{
    auto params = static_cast<RSDisplayRenderParams*>(targetNode_->GetRenderParams().get());
    auto& curAllSurfaces = params->GetAllMainAndLeashSurfaces();
    for (auto it = curAllSurfaces.rbegin(); it != curAllSurfaces.rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow()) {
            continue;
        }
        if (CheckIfSurfaceTargetedForDFX(surfaceNode->GetName())) {
            auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetRenderParams().get());
            const auto& visibleRegions = surfaceParams->GetVisibleRegion().GetRegionRects();
            std::vector<RectI> rects;
            for (auto& rect : visibleRegions) {
                rects.emplace_back(rect.left_, rect.top_, rect.right_ - rect.left_, rect.bottom_ - rect.top_);
            }
            DrawDirtyRegionForDFX(rects);
        }
    }
}

} // namespace OHOS::Rosen