/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rs_uni_render_util.h"
#include <cstdint>
#include <unordered_set>

#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_base_render_util.h"
#include "platform/common/rs_log.h"
#include "render/rs_path.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr const char* ENTRY_VIEW = "EntryView";
constexpr const char* WALLPAPER_VIEW = "WallpaperView";
constexpr const char* SCREENLOCK_WINDOW = "ScreenLockWindow";
constexpr const char* SYSUI_DROPDOWN = "SysUI_Dropdown";
constexpr const char* SYSUI_STATUS_BAR = "SysUI_StatusBar";
constexpr const char* PRIVACY_INDICATOR = "PrivacyIndicator";
};
void RSUniRenderUtil::MergeDirtyHistory(std::shared_ptr<RSDisplayRenderNode>& node, int32_t bufferAge,
    bool useAlignedDirtyRegion)
{
    // update all child surfacenode history
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow()) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNode->GetDirtyManager();
        if (!surfaceDirtyManager->SetBufferAge(bufferAge)) {
            ROSEN_LOGE("RSUniRenderUtil::MergeVisibleDirtyRegion with invalid buffer age %d", bufferAge);
        }
        surfaceDirtyManager->IntersectDirtyRect(surfaceNode->GetOldDirtyInSurface());
        surfaceDirtyManager->UpdateDirty(useAlignedDirtyRegion);
    }
    // update display dirtymanager
    node->UpdateDisplayDirtyManager(bufferAge, useAlignedDirtyRegion);
}

Occlusion::Region RSUniRenderUtil::MergeVisibleDirtyRegion(std::shared_ptr<RSDisplayRenderNode>& node,
    bool useAlignedDirtyRegion)
{
    Occlusion::Region allSurfaceVisibleDirtyRegion;
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow() || surfaceNode->GetDstRect().IsEmpty()) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNode->GetDirtyManager();
        auto surfaceDirtyRect = surfaceDirtyManager->GetDirtyRegion();
        Occlusion::Rect dirtyRect { surfaceDirtyRect.left_, surfaceDirtyRect.top_,
            surfaceDirtyRect.GetRight(), surfaceDirtyRect.GetBottom() };
        auto visibleRegion = surfaceNode->GetVisibleRegion();
        Occlusion::Region surfaceDirtyRegion { dirtyRect };
        Occlusion::Region surfaceVisibleDirtyRegion = surfaceDirtyRegion.And(visibleRegion);
        surfaceNode->SetVisibleDirtyRegion(surfaceVisibleDirtyRegion);
        if (useAlignedDirtyRegion) {
            Occlusion::Region alignedRegion = AlignedDirtyRegion(surfaceVisibleDirtyRegion);
            surfaceNode->SetAlignedVisibleDirtyRegion(alignedRegion);
            allSurfaceVisibleDirtyRegion.OrSelf(alignedRegion);
        } else {
            allSurfaceVisibleDirtyRegion = allSurfaceVisibleDirtyRegion.Or(surfaceVisibleDirtyRegion);
        }
    }
    return allSurfaceVisibleDirtyRegion;
}

void RSUniRenderUtil::SrcRectScaleDown(BufferDrawParam& params, const RSSurfaceRenderNode& node)
{
    ScalingMode scalingMode = ScalingMode::SCALING_MODE_SCALE_TO_WINDOW;
    const auto& buffer = node.GetBuffer();
    const auto& surface = node.GetConsumer();
    if (buffer == nullptr || surface == nullptr) {
        return;
    }

    if (surface->GetScalingMode(buffer->GetSeqNum(), scalingMode) == GSERROR_OK &&
        scalingMode == ScalingMode::SCALING_MODE_SCALE_CROP) {
        const RSProperties& property = node.GetRenderProperties();
#ifndef USE_ROSEN_DRAWING
        uint32_t newWidth = static_cast<uint32_t>(params.srcRect.width());
        uint32_t newHeight = static_cast<uint32_t>(params.srcRect.height());
#else
        uint32_t newWidth = static_cast<uint32_t>(params.srcRect.GetWidth());
        uint32_t newHeight = static_cast<uint32_t>(params.srcRect.GetHeight());
#endif
        // Canvas is able to handle the situation when the window is out of screen, using bounds instead of dst.
        uint32_t boundsWidth = static_cast<uint32_t>(property.GetBoundsWidth());
        uint32_t boundsHeight = static_cast<uint32_t>(property.GetBoundsHeight());

        // If transformType is not a multiple of 180, need to change the correspondence between width & height.
        GraphicTransformType transformType = RSBaseRenderUtil::GetRotateTransform(surface->GetTransform());
        if (transformType == GraphicTransformType::GRAPHIC_ROTATE_270 ||
            transformType == GraphicTransformType::GRAPHIC_ROTATE_90) {
            std::swap(boundsWidth, boundsHeight);
        }

        if (newWidth * boundsHeight > newHeight * boundsWidth) {
            // too wide
            newWidth = boundsWidth * newHeight / boundsHeight;
        } else if (newWidth * boundsHeight < newHeight * boundsWidth) {
            // too tall
            newHeight = boundsHeight * newWidth / boundsWidth;
        } else {
            return;
        }

#ifndef USE_ROSEN_DRAWING
        uint32_t currentWidth = static_cast<uint32_t>(params.srcRect.width());
        uint32_t currentHeight = static_cast<uint32_t>(params.srcRect.height());
#else
        uint32_t currentWidth = static_cast<uint32_t>(params.srcRect.GetWidth());
        uint32_t currentHeight = static_cast<uint32_t>(params.srcRect.GetHeight());
#endif
        if (newWidth < currentWidth) {
            // the crop is too wide
            uint32_t dw = currentWidth - newWidth;
            auto halfdw = dw / 2;
#ifndef USE_ROSEN_DRAWING
            params.srcRect = SkRect::MakeXYWH(params.srcRect.left() + static_cast<int32_t>(halfdw),
                                              params.srcRect.top(),
                                              static_cast<int32_t>(newWidth),
                                              params.srcRect.height());
#else
            params.srcRect =
                Drawing::Rect(params.srcRect.GetLeft() + static_cast<int32_t>(halfdw), params.srcRect.GetTop(),
                    params.srcRect.GetLeft() + static_cast<int32_t>(halfdw) + static_cast<int32_t>(newWidth),
                    params.srcRect.GetTop() + params.srcRect.GetHeight());
#endif
        } else {
            // thr crop is too tall
            uint32_t dh = currentHeight - newHeight;
            auto halfdh = dh / 2;
#ifndef USE_ROSEN_DRAWING
            params.srcRect = SkRect::MakeXYWH(params.srcRect.left(),
                                              params.srcRect.top() + static_cast<int32_t>(halfdh),
                                              params.srcRect.width(),
                                              static_cast<int32_t>(newHeight));
#else
            params.srcRect =
                Drawing::Rect(params.srcRect.GetLeft(), params.srcRect.GetTop() + static_cast<int32_t>(halfdh),
                    params.srcRect.GetLeft() + params.srcRect.GetWidth(),
                    params.srcRect.GetTop() + static_cast<int32_t>(halfdh) + static_cast<int32_t>(newHeight));
#endif
        }
#ifndef USE_ROSEN_DRAWING
        RS_LOGD("RsDebug RSUniRenderUtil::SrcRectScaleDown surfaceNode id:%" PRIu64 ", srcRect [%f %f %f %f]",
            node.GetId(), params.srcRect.left(), params.srcRect.top(),
            params.srcRect.width(), params.srcRect.height());
#else
        RS_LOGD("RsDebug RSUniRenderUtil::SrcRectScaleDown surfaceNode id:%" PRIu64 ", srcRect [%f %f %f %f]",
            node.GetId(), params.srcRect.GetLeft(), params.srcRect.GetTop(),
            params.srcRect.GetWidth(), params.srcRect.GetHeight());
#endif
    }
}

BufferDrawParam RSUniRenderUtil::CreateBufferDrawParam(const RSSurfaceRenderNode& node, bool forceCPU)
{
    BufferDrawParam params;
#ifdef RS_ENABLE_EGLIMAGE
    params.useCPU = forceCPU;
#else // RS_ENABLE_EGLIMAGE
    params.useCPU = true;
#endif // RS_ENABLE_EGLIMAGE
#ifndef USE_ROSEN_DRAWING
    params.paint.setAntiAlias(true);
#ifndef NEW_SKIA
    params.paint.setFilterQuality(SkFilterQuality::kLow_SkFilterQuality);
#endif

    const RSProperties& property = node.GetRenderProperties();
    params.dstRect = SkRect::MakeWH(property.GetBoundsWidth(), property.GetBoundsHeight());
#else
    params.paint.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetFilterQuality(Drawing::Filter::FilterQuality::LOW);
    params.paint.SetFilter(filter);

    const RSProperties& property = node.GetRenderProperties();
    params.dstRect = Drawing::Rect(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
#endif

    const sptr<SurfaceBuffer>& buffer = node.GetBuffer();
    if (buffer == nullptr) {
        return params;
    }
    params.buffer = buffer;
    params.acquireFence = node.GetAcquireFence();
#ifndef USE_ROSEN_DRAWING
    params.srcRect = SkRect::MakeWH(buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
#else
    params.srcRect = Drawing::Rect(0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
#endif

    auto& consumer = node.GetConsumer();
    if (consumer == nullptr) {
        return params;
    }
    auto transform = consumer->GetTransform();
    RectF localBounds = { 0.0f, 0.0f, property.GetBoundsWidth(), property.GetBoundsHeight() };
    RSBaseRenderUtil::DealWithSurfaceRotationAndGravity(transform, property.GetFrameGravity(), localBounds, params);
    RSBaseRenderUtil::FlipMatrix(transform, params);
    SrcRectScaleDown(params, node);
    return params;
}

BufferDrawParam RSUniRenderUtil::CreateBufferDrawParam(const RSDisplayRenderNode& node, bool forceCPU)
{
    BufferDrawParam params;
#ifdef RS_ENABLE_EGLIMAGE
    params.useCPU = forceCPU;
#else // RS_ENABLE_EGLIMAGE
    params.useCPU = true;
#endif // RS_ENABLE_EGLIMAGE
#ifndef USE_ROSEN_DRAWING
    params.paint.setAntiAlias(true);
#ifndef NEW_SKIA
    params.paint.setFilterQuality(SkFilterQuality::kLow_SkFilterQuality);
#endif

    const sptr<SurfaceBuffer>& buffer = node.GetBuffer();
    params.buffer = buffer;
    params.acquireFence = node.GetAcquireFence();
    params.srcRect = SkRect::MakeWH(buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    params.dstRect = SkRect::MakeWH(buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
#else
    params.paint.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetFilterQuality(Drawing::Filter::FilterQuality::LOW);
    params.paint.SetFilter(filter);

    const sptr<SurfaceBuffer>& buffer = node.GetBuffer();
    params.buffer = buffer;
    params.acquireFence = node.GetAcquireFence();
    params.srcRect = Drawing::Rect(0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    params.dstRect = Drawing::Rect(0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
#endif
    return params;
}

BufferDrawParam RSUniRenderUtil::CreateLayerBufferDrawParam(const LayerInfoPtr& layer, bool forceCPU)
{
    BufferDrawParam params;
#ifdef RS_ENABLE_EGLIMAGE
    params.useCPU = forceCPU;
#else // RS_ENABLE_EGLIMAGE
    params.useCPU = true;
#endif // RS_ENABLE_EGLIMAGE
#ifndef USE_ROSEN_DRAWING
    params.paint.setAntiAlias(true);
#ifndef NEW_SKIA
    params.paint.setFilterQuality(SkFilterQuality::kLow_SkFilterQuality);
#endif
    params.paint.setAlphaf(layer->GetAlpha().gAlpha);
#else
    params.paint.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetFilterQuality(Drawing::Filter::FilterQuality::LOW);
    params.paint.SetFilter(filter);
    params.paint.SetAlphaF(layer->GetAlpha().gAlpha);
#endif

    sptr<SurfaceBuffer> buffer = layer->GetBuffer();
    if (buffer == nullptr) {
        return params;
    }
    params.acquireFence = layer->GetAcquireFence();
    params.buffer = buffer;
#ifndef USE_ROSEN_DRAWING
    params.srcRect = SkRect::MakeWH(buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    auto boundRect = layer->GetBoundSize();
    params.dstRect = SkRect::MakeWH(boundRect.w, boundRect.h);

    auto layerMatrix = layer->GetMatrix();
    params.matrix = SkMatrix::MakeAll(layerMatrix.scaleX, layerMatrix.skewX, layerMatrix.transX,
                                      layerMatrix.skewY, layerMatrix.scaleY, layerMatrix.transY,
                                      layerMatrix.pers0, layerMatrix.pers1, layerMatrix.pers2);
#else
    params.srcRect = Drawing::Rect(0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    auto boundRect = layer->GetBoundSize();
    params.dstRect = Drawing::Rect(0, 0, boundRect.w, boundRect.h);

    auto layerMatrix = layer->GetMatrix();
    params.matrix = Drawing::Matrix();
    params.matrix.SetMatrix(layerMatrix.scaleX, layerMatrix.skewX, layerMatrix.transX, layerMatrix.skewY,
        layerMatrix.scaleY, layerMatrix.transY, layerMatrix.pers0, layerMatrix.pers1, layerMatrix.pers2);
#endif
    int nodeRotation = RSUniRenderUtil::GetRotationFromMatrix(params.matrix); // rotation degree anti-clockwise
    auto layerTransform = layer->GetTransformType();
    // calculate clockwise rotation degree excluded rotation in total matrix
    int realRotation = (nodeRotation +
        RSBaseRenderUtil::RotateEnumToInt(RSBaseRenderUtil::GetRotateTransform(layerTransform))) % 360;
    auto flip = RSBaseRenderUtil::GetFlipTransform(layerTransform);
    // calculate transform in anti-clockwise
    auto transform = RSBaseRenderUtil::RotateEnumToInt(realRotation, flip);

    RectF localBounds = { 0.0f, 0.0f, static_cast<float>(boundRect.w), static_cast<float>(boundRect.h) };
    RSBaseRenderUtil::DealWithSurfaceRotationAndGravity(transform, Gravity::RESIZE, localBounds, params);
    RSBaseRenderUtil::FlipMatrix(transform, params);
    return params;
}

#ifndef USE_ROSEN_DRAWING
void RSUniRenderUtil::DrawCachedImage(RSSurfaceRenderNode& node, RSPaintFilterCanvas& canvas, sk_sp<SkImage> image)
{
    if (image == nullptr) {
        return;
    }
    canvas.save();
    canvas.scale(node.GetRenderProperties().GetBoundsWidth() / image->width(),
        node.GetRenderProperties().GetBoundsHeight() / image->height());
    SkPaint paint;
#ifdef NEW_SKIA
    canvas.drawImage(image.get(), 0.0, 0.0, SkSamplingOptions(), &paint);
#else
    canvas.drawImage(image.get(), 0.0, 0.0, &paint);
#endif
    canvas.restore();
}
#else
void RSUniRenderUtil::DrawCachedImage(RSSurfaceRenderNode& node, RSPaintFilterCanvas& canvas,
    std::shared_ptr<Drawing::Image> image)
{
    if (image == nullptr) {
        return;
    }
    canvas.Save();
    canvas.Scale(node.GetRenderProperties().GetBoundsWidth() / image->GetWidth(),
        node.GetRenderProperties().GetBoundsHeight() / image->GetHeight());
    Drawing::Brush brush;
    canvas.AttachBrush(brush);
    Drawing::SamplingOptions sampling =
        Drawing::SamplingOptions(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NEAREST);
    canvas.DrawImage(*image.get(), 0.0, 0.0, sampling);
    canvas.DetachBrush();
    canvas.Restore();
}
#endif

Occlusion::Region RSUniRenderUtil::AlignedDirtyRegion(const Occlusion::Region& dirtyRegion, int32_t alignedBits)
{
    Occlusion::Region alignedRegion;
    if (alignedBits <= 1) {
        return dirtyRegion;
    }
    for (const auto& dirtyRect : dirtyRegion.GetRegionRects()) {
        int32_t left = (dirtyRect.left_ / alignedBits) * alignedBits;
        int32_t top = (dirtyRect.top_ / alignedBits) * alignedBits;
        int32_t width = ((dirtyRect.right_ + alignedBits - 1) / alignedBits) * alignedBits - left;
        int32_t height = ((dirtyRect.bottom_ + alignedBits - 1) / alignedBits) * alignedBits - top;
        Occlusion::Rect rect = { left, top, left + width, top + height };
        Occlusion::Region singleAlignedRegion(rect);
        alignedRegion.OrSelf(singleAlignedRegion);
    }
    return alignedRegion;
}

bool RSUniRenderUtil::HandleSubThreadNode(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    if (node.IsMainThreadNode()) {
        return false;
    }
    if (!node.HasCachedTexture()) {
        RS_TRACE_NAME_FMT("HandleSubThreadNode wait %" PRIu64 "", node.GetId());
#if defined(RS_ENABLE_GL)
        RSSubThreadManager::Instance()->WaitNodeTask(node.GetId());
        node.UpdateCompletedCacheSurface();
#endif
    }
    RS_TRACE_NAME_FMT("RSUniRenderUtil::HandleSubThreadNode %" PRIu64 "", node.GetId());
    node.DrawCacheSurface(canvas, UNI_MAIN_THREAD_INDEX, true);
    return true;
}

bool RSUniRenderUtil::HandleCaptureNode(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    auto surfaceNodePtr = node.ReinterpretCastTo<RSSurfaceRenderNode>();
    if (surfaceNodePtr == nullptr) {
        return false;
    }
    if (surfaceNodePtr->IsAppWindow()) {
        auto rsParent = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(surfaceNodePtr->GetParent().lock());
        auto curNode = surfaceNodePtr;
        if (rsParent && rsParent->IsLeashWindow()) {
            curNode = rsParent;
        }
        if (!curNode->ShouldPaint()) {
            return false;
        }
        if (curNode->IsOnTheTree()) {
            return HandleSubThreadNode(*curNode, canvas);
        } else {
#ifdef RS_ENABLE_GL
            if (curNode->GetCacheSurfaceProcessedStatus() == CacheProcessStatus::DOING) {
                RSSubThreadManager::Instance()->WaitNodeTask(curNode->GetId());
            }
#endif
            return false;
        }
    }
    return false;
}

#ifndef USE_ROSEN_DRAWING
int RSUniRenderUtil::GetRotationFromMatrix(SkMatrix matrix)
{
    float value[9];
    matrix.get9(value);

    int rAngle = static_cast<int>(-round(atan2(value[SkMatrix::kMSkewX], value[SkMatrix::kMScaleX]) * (180 / PI)));
    // transfer the result to anti-clockwise degrees
    // only rotation with 90°, 180°, 270° are composed through hardware,
    // in which situation the transformation of the layer needs to be set.
    static const std::map<int, int> supportedDegrees = {{90, 270}, {180, 180}, {-90, 90}, {-180, 180}};
    auto iter = supportedDegrees.find(rAngle);
    return iter != supportedDegrees.end() ? iter->second : 0;
}
#else
int RSUniRenderUtil::GetRotationFromMatrix(Drawing::Matrix matrix)
{
    Drawing::Matrix::Buffer value;
    matrix.GetAll(value);

    int rAngle = static_cast<int>(-round(atan2(value[Drawing::Matrix::Index::SKEW_X],
        value[Drawing::Matrix::Index::SKEW_Y]) * (180 / PI)));
    // transfer the result to anti-clockwise degrees
    // only rotation with 90°, 180°, 270° are composed through hardware,
    // in which situation the transformation of the layer needs to be set.
    static const std::map<int, int> supportedDegrees = {{90, 270}, {180, 180}, {-90, 90}};
    auto iter = supportedDegrees.find(rAngle);
    return iter != supportedDegrees.end() ? iter->second : 0;
}
#endif

void RSUniRenderUtil::AssignWindowNodes(const std::shared_ptr<RSDisplayRenderNode>& displayNode,
    std::list<std::shared_ptr<RSSurfaceRenderNode>>& mainThreadNodes,
    std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes)
{
    if (displayNode == nullptr) {
        ROSEN_LOGE("RSUniRenderUtil::AssignWindowNodes display node is null");
        return;
    }
    RS_TRACE_NAME("AssignWindowNodes");
    bool isRotation = displayNode->IsRotationChanged();
    bool isScale = false;
    uint32_t leashWindowCount = 0;
    for (auto iter = displayNode->GetSortedChildren().begin(); iter != displayNode->GetSortedChildren().end(); iter++) {
        auto node = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*iter);
        if (node == nullptr) {
            ROSEN_LOGE("RSUniRenderUtil::AssignWindowNodes nullptr found in sortedChildren, this should not happen");
            continue;
        }
        if (node->IsLeashWindow()) {
            leashWindowCount++;
        }
        if (node->IsLeashWindow() && node->IsScale()) {
            isScale = true;
        }
    }
    // trace info for assign window nodes
    std::string traceInfo = "{ isScale: " + std::to_string(isScale) + ", " +
        "leashWindowCount: " + std::to_string(leashWindowCount) + ", " +
        "isRotation: " + std::to_string(isRotation) + " }; ";
    for (auto iter = displayNode->GetSortedChildren().begin(); iter != displayNode->GetSortedChildren().end(); iter++) {
        auto node = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*iter);
        if (node == nullptr) {
            ROSEN_LOGE("RSUniRenderUtil::AssignWindowNodes nullptr found in sortedChildren, this should not happen");
            continue;
        }
        // trace info for assign window nodes
        traceInfo += "node:[ " + node->GetName() + ", " + std::to_string(node->GetId()) + " ]" +
            "( " + std::to_string(static_cast<uint32_t>(node->GetCacheSurfaceProcessedStatus())) + ", " +
            std::to_string(node->HasFilter()) + ", " + std::to_string(node->HasAbilityComponent()) + " ); ";
        std::string surfaceName = node->GetName();
        if (surfaceName == ENTRY_VIEW || surfaceName == WALLPAPER_VIEW || surfaceName == SYSUI_STATUS_BAR ||
            surfaceName == SCREENLOCK_WINDOW ||surfaceName == SYSUI_DROPDOWN || surfaceName == PRIVACY_INDICATOR) {
            AssignMainThreadNode(mainThreadNodes, node);
            continue;
        }
        if (node->GetCacheSurfaceProcessedStatus() == CacheProcessStatus::DOING) { // node exceed one vsync
            AssignSubThreadNode(subThreadNodes, node);
        } else if (leashWindowCount > 1) { // start app from another app
            AssignMainThreadNode(mainThreadNodes, node);
        } else if (isScale) { // app start or close scene
            if (!node->HasFilter() && !node->HasAbilityComponent() && !isRotation) {
                AssignSubThreadNode(subThreadNodes, node);
            } else {
                AssignMainThreadNode(mainThreadNodes, node);
            }
        } else { // other scene
            AssignMainThreadNode(mainThreadNodes, node);
        }
    }
    SortSubThreadNodes(subThreadNodes);
    RS_TRACE_NAME_FMT("AssignWindowNodes Infos: %s", traceInfo.c_str());
}

void RSUniRenderUtil::AssignMainThreadNode(std::list<std::shared_ptr<RSSurfaceRenderNode>>& mainThreadNodes,
    const std::shared_ptr<RSSurfaceRenderNode>& node)
{
    if (node == nullptr) {
        ROSEN_LOGW("RSUniRenderUtil::AssignMainThreadNode node is nullptr");
        return;
    }
    mainThreadNodes.emplace_back(node);
    bool changeThread = !node->IsMainThreadNode();
    node->SetIsMainThreadNode(true);
    node->SetCacheType(CacheType::NONE);
    HandleHardwareNode(node);
    if (changeThread) {
        RS_LOGD("RSUniRenderUtil::AssignMainThreadNode clear cache surface:[%s, %llu]",
            node->GetName().c_str(), node->GetId());
        ClearCacheSurface(node, UNI_MAIN_THREAD_INDEX);
        node->SetTextureValidFlag(false);
    }
}

void RSUniRenderUtil::AssignSubThreadNode(std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes,
    const std::shared_ptr<RSSurfaceRenderNode>& node)
{
    if (node == nullptr) {
        ROSEN_LOGW("RSUniRenderUtil::AssignSubThreadNode node is nullptr");
        return;
    }
    subThreadNodes.emplace_back(node);
    node->SetIsMainThreadNode(false);
    node->UpdateCacheSurfaceDirtyManager(2); // 2 means buffer age
    node->SetCacheType(CacheType::CONTENT);
#if defined(RS_ENABLE_GL)
    if (node->GetCacheSurfaceProcessedStatus() == CacheProcessStatus::DONE && node->GetCacheSurface()) {
        node->UpdateCompletedCacheSurface();
    }
#endif
    if (node->HasCachedTexture()) {
        node->SetPriority(NodePriorityType::SUB_LOW_PRIORITY);
    } else {
        node->SetPriority(NodePriorityType::SUB_HIGH_PRIORITY);
    }
}

void RSUniRenderUtil::SortSubThreadNodes(std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes)
{
    // sort subThreadNodes by priority and z-order
    subThreadNodes.sort([](const auto& first, const auto& second) -> bool {
        auto node1 = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(first);
        auto node2 = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(second);
        if (node1 == nullptr || node2 == nullptr) {
            ROSEN_LOGE(
                "RSUniRenderUtil::SortSubThreadNodes sort nullptr found in subThreadNodes, this should not happen");
            return false;
        }
        if (node1->GetPriority() == node2->GetPriority()) {
            return node2->GetRenderProperties().GetPositionZ() < node1->GetRenderProperties().GetPositionZ();
        } else {
            return node1->GetPriority() < node2->GetPriority();
        }
    });
}

void RSUniRenderUtil::CacheSubThreadNodes(std::list<std::shared_ptr<RSSurfaceRenderNode>>& oldSubThreadNodes,
    std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes)
{
    std::unordered_set<std::shared_ptr<RSSurfaceRenderNode>> nodes(subThreadNodes.begin(), subThreadNodes.end());
    for (auto node : oldSubThreadNodes) {
        if (nodes.count(node) > 0) {
            continue;
        }
        // The node being processed by sub thread may have been removed.
        if (node->GetCacheSurfaceProcessedStatus() == CacheProcessStatus::DOING) {
            subThreadNodes.emplace_back(node);
        }
    }
    oldSubThreadNodes.clear();
    oldSubThreadNodes = subThreadNodes;
}

void RSUniRenderUtil::HandleHardwareNode(const std::shared_ptr<RSSurfaceRenderNode>& node)
{
    if (!node->HasHardwareNode()) {
        return;
    }
    auto appWindow = node;
    if (node->IsLeashWindow()) {
        for (auto& child : node->GetSortedChildren()) {
            auto surfaceNodePtr = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
            if (surfaceNodePtr && surfaceNodePtr->IsAppWindow()) {
                appWindow = surfaceNodePtr;
                break;
            }
        }
    }
    auto hardwareEnabledNodes = appWindow->GetChildHardwareEnabledNodes();
    for (auto& hardwareEnabledNode : hardwareEnabledNodes) {
        auto hardwareEnabledNodePtr = hardwareEnabledNode.lock();
        if (hardwareEnabledNodePtr) {
            hardwareEnabledNodePtr->SetHardwareDisabledByCache(false);
        }
    }
}

void RSUniRenderUtil::ClearSurfaceIfNeed(const RSRenderNodeMap& map,
    const std::shared_ptr<RSDisplayRenderNode>& displayNode,
    std::set<std::shared_ptr<RSBaseRenderNode>>& oldChildren)
{
    if (displayNode == nullptr) {
        return;
    }
    auto sortedChildren = displayNode->GetSortedChildren();
    std::set<std::shared_ptr<RSBaseRenderNode>> tmpSet(sortedChildren.begin(), sortedChildren.end());
    for (auto& child : oldChildren) {
        auto surface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
        if (tmpSet.count(surface) == 0) {
            if (surface && map.GetRenderNode(surface->GetId()) != nullptr) {
                RS_LOGD("RSUniRenderUtil::ClearSurfaceIfNeed clear cache surface:[%s, %llu]",
                    surface->GetName().c_str(), surface->GetId());
                ClearCacheSurface(surface, UNI_MAIN_THREAD_INDEX);
                surface->SetTextureValidFlag(false);
            }
        }
    }
    oldChildren.swap(tmpSet);
}

void RSUniRenderUtil::ClearCacheSurface(const std::shared_ptr<RSSurfaceRenderNode>& node, uint32_t threadIndex)
{
    RS_LOGD("ClearCacheSurface node in correct thread: [%llu]", node->GetId());
    uint32_t cacheSurfaceThreadIndex = node->GetCacheSurfaceThreadIndex();
    if (cacheSurfaceThreadIndex == threadIndex) {
        node->ClearCacheSurface();
        node->SetIsMainThreadNode(true);
        return;
    }
    if (cacheSurfaceThreadIndex == UNI_MAIN_THREAD_INDEX) {
        RSMainThread::Instance()->PostTask([node]() {
            RS_LOGD("clear node cache surface in main thread");
            node->ClearCacheSurface();
            node->SetIsMainThreadNode(true);
        });
    } else {
#ifdef RS_ENABLE_GL
        RSSubThreadManager::Instance()->PostTask([node]() {
            RS_LOGD("clear node cache surface in sub thread");
            node->ClearCacheSurface();
            node->SetIsMainThreadNode(true);
        }, cacheSurfaceThreadIndex);
#endif
    }
}

void RSUniRenderUtil::ClearCacheSurface(RSRenderNode& node, uint32_t threadIndex, bool isUIFirst)
{
    RS_LOGD("ClearCacheSurface node: [%llu]", node.GetId());
    uint32_t cacheSurfaceThreadIndex = node.GetCacheSurfaceThreadIndex();
    if (cacheSurfaceThreadIndex == threadIndex) {
        node.ClearCacheSurface();
        return;
    }
    auto cacheSurface = node.GetCacheSurface();
    auto cacheCompletedSurface = node.GetCompletedCacheSurface(threadIndex, isUIFirst);
    node.ClearCacheSurface();
    ClearNodeCacheSurface(cacheSurface, cacheCompletedSurface, cacheSurfaceThreadIndex);
}

#ifndef USE_ROSEN_DRAWING
void RSUniRenderUtil::ClearNodeCacheSurface(sk_sp<SkSurface> cacheSurface, sk_sp<SkSurface> cacheCompletedSurface,
    uint32_t threadIndex)
#else
void RSUniRenderUtil::ClearNodeCacheSurface(std::shared_ptr<Drawing::Surface> cacheSurface,
    std::shared_ptr<Drawing::Surface> cacheCompletedSurface, uint32_t threadIndex)
#endif
{
    if (cacheSurface == nullptr && cacheCompletedSurface == nullptr) {
        return;
    }
    if (threadIndex == UNI_MAIN_THREAD_INDEX) {
        RSMainThread::Instance()->PostTask([cacheSurface, cacheCompletedSurface]() mutable {
            RS_LOGD("clear node cache surface in main thread");
            cacheSurface = nullptr;
            cacheCompletedSurface = nullptr;
        });
    } else {
#ifdef RS_ENABLE_GL
        RSSubThreadManager::Instance()->PostTask([cacheSurface, cacheCompletedSurface]() mutable {
            RS_LOGD("clear node cache surface in sub thread");
            cacheSurface = nullptr;
            cacheCompletedSurface = nullptr;
        }, threadIndex);
#endif
    }
}
} // namespace Rosen
} // namespace OHOS
