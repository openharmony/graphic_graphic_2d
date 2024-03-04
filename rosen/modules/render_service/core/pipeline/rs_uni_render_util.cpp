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

#include "common/rs_optional_trace.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_base_render_util.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties.h"
#include "render/rs_material_filter.h"
#include "render/rs_path.h"
#include "rs_trace.h"
#include "common/rs_optional_trace.h"
#include "scene_board_judgement.h"
#include <parameter.h>
#include <parameters.h>

namespace OHOS {
namespace Rosen {
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
            ROSEN_LOGE("RSUniRenderUtil::MergeDirtyHistory with invalid buffer age %{public}d", bufferAge);
        }
        surfaceDirtyManager->IntersectDirtyRect(surfaceNode->GetOldDirtyInSurface());
        surfaceDirtyManager->UpdateDirty(useAlignedDirtyRegion);
    }
    // update display dirtymanager
    node->UpdateDisplayDirtyManager(bufferAge, useAlignedDirtyRegion);
}

Occlusion::Region RSUniRenderUtil::MergeVisibleDirtyRegion(std::shared_ptr<RSDisplayRenderNode>& node,
    std::vector<NodeId>& hasVisibleDirtyRegionSurfaceVec, bool useAlignedDirtyRegion)
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
        if (!surfaceVisibleDirtyRegion.IsEmpty()) {
            hasVisibleDirtyRegionSurfaceVec.emplace_back(surfaceNode->GetId());
        }
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
        uint32_t newWidth = static_cast<uint32_t>(params.srcRect.GetWidth());
        uint32_t newHeight = static_cast<uint32_t>(params.srcRect.GetHeight());
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
            if (boundsHeight == 0) {
                return;
            }
            newWidth = boundsWidth * newHeight / boundsHeight;
        } else if (newWidth * boundsHeight < newHeight * boundsWidth) {
            // too tall
            if (boundsWidth == 0) {
                return;
            }
            newHeight = boundsHeight * newWidth / boundsWidth;
        } else {
            return;
        }

        uint32_t currentWidth = static_cast<uint32_t>(params.srcRect.GetWidth());
        uint32_t currentHeight = static_cast<uint32_t>(params.srcRect.GetHeight());
        if (newWidth < currentWidth) {
            // the crop is too wide
            uint32_t dw = currentWidth - newWidth;
            auto halfdw = dw / 2;
            params.srcRect =
                Drawing::Rect(params.srcRect.GetLeft() + static_cast<int32_t>(halfdw), params.srcRect.GetTop(),
                    params.srcRect.GetLeft() + static_cast<int32_t>(halfdw) + static_cast<int32_t>(newWidth),
                    params.srcRect.GetTop() + params.srcRect.GetHeight());
        } else {
            // thr crop is too tall
            uint32_t dh = currentHeight - newHeight;
            auto halfdh = dh / 2;
            params.srcRect =
                Drawing::Rect(params.srcRect.GetLeft(), params.srcRect.GetTop() + static_cast<int32_t>(halfdh),
                    params.srcRect.GetLeft() + params.srcRect.GetWidth(),
                    params.srcRect.GetTop() + static_cast<int32_t>(halfdh) + static_cast<int32_t>(newHeight));
        }
        RS_LOGD("RsDebug RSUniRenderUtil::SrcRectScaleDown surfaceNode id:%{public}" PRIu64 ","
            " srcRect [%{public}f %{public}f %{public}f %{public}f]",
            node.GetId(), params.srcRect.GetLeft(), params.srcRect.GetTop(),
            params.srcRect.GetWidth(), params.srcRect.GetHeight());
    }
}

BufferDrawParam RSUniRenderUtil::CreateBufferDrawParam(const RSSurfaceRenderNode& node,
    bool forceCPU, uint32_t threadIndex)
{
    BufferDrawParam params;
    params.threadIndex = threadIndex;
    params.useBilinearInterpolation = node.NeedBilinearInterpolation();
    params.useCPU = forceCPU;
    params.paint.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetFilterQuality(Drawing::Filter::FilterQuality::LOW);
    params.paint.SetFilter(filter);

    const RSProperties& property = node.GetRenderProperties();
    params.dstRect = Drawing::Rect(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());

    const sptr<SurfaceBuffer> buffer = node.GetBuffer();
    if (buffer == nullptr) {
        return params;
    }
    params.buffer = buffer;
    params.acquireFence = node.GetAcquireFence();
    params.srcRect = Drawing::Rect(0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());

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
    params.useCPU = forceCPU;
    params.paint.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetFilterQuality(Drawing::Filter::FilterQuality::LOW);
    params.paint.SetFilter(filter);

    const sptr<SurfaceBuffer>& buffer = node.GetBuffer();
    params.buffer = buffer;
    params.acquireFence = node.GetAcquireFence();
    params.srcRect = Drawing::Rect(0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    params.dstRect = Drawing::Rect(0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    return params;
}

BufferDrawParam RSUniRenderUtil::CreateLayerBufferDrawParam(const LayerInfoPtr& layer, bool forceCPU)
{
    BufferDrawParam params;
    params.useCPU = forceCPU;
    params.paint.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetFilterQuality(Drawing::Filter::FilterQuality::LOW);
    params.paint.SetFilter(filter);
    params.paint.SetAlphaF(layer->GetAlpha().gAlpha);

    sptr<SurfaceBuffer> buffer = layer->GetBuffer();
    if (buffer == nullptr) {
        return params;
    }
    params.acquireFence = layer->GetAcquireFence();
    params.buffer = buffer;
    params.srcRect = Drawing::Rect(0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    auto boundRect = layer->GetBoundSize();
    params.dstRect = Drawing::Rect(0, 0, boundRect.w, boundRect.h);

    auto layerMatrix = layer->GetMatrix();
    params.matrix = Drawing::Matrix();
    params.matrix.SetMatrix(layerMatrix.scaleX, layerMatrix.skewX, layerMatrix.transX, layerMatrix.skewY,
        layerMatrix.scaleY, layerMatrix.transY, layerMatrix.pers0, layerMatrix.pers1, layerMatrix.pers2);
    int nodeRotation = RSUniRenderUtil::GetRotationFromMatrix(params.matrix); // rotation degree anti-clockwise
    auto layerTransform = layer->GetTransformType();
    // calculate clockwise rotation degree excluded rotation in total matrix
    int realRotation = (nodeRotation +
        RSBaseRenderUtil::RotateEnumToInt(RSBaseRenderUtil::GetRotateTransform(layerTransform))) % 360;
    auto flip = RSBaseRenderUtil::GetFlipTransform(layerTransform);
    // calculate transform in anti-clockwise
    auto transform = RSBaseRenderUtil::RotateEnumToInt(realRotation, flip);

    RectF localBounds = { 0.0f, 0.0f, static_cast<float>(boundRect.w), static_cast<float>(boundRect.h) };
    RSBaseRenderUtil::DealWithSurfaceRotationAndGravity(transform, static_cast<Gravity>(layer->GetGravity()),
        localBounds, params);
    RSBaseRenderUtil::FlipMatrix(transform, params);
    return params;
}

bool RSUniRenderUtil::IsNeedClient(RSSurfaceRenderNode& node, const ComposeInfo& info)
{
    if (RSBaseRenderUtil::IsForceClient()) {
        RS_LOGD("RSUniRenderUtil::IsNeedClient: force client.");
        return true;
    }
    const auto& property = node.GetRenderProperties();
    if (property.GetRotation() != 0 || property.GetRotationX() != 0 || property.GetRotationY() != 0 ||
        property.GetQuaternion() != Quaternion()) {
        RS_LOGD("RSUniRenderUtil::IsNeedClient need client with RSSurfaceRenderNode rotation");
        return true;
    }
    return false;
}

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

bool RSUniRenderUtil::HandleSubThreadNode(RSSurfaceRenderNode& node, RSPaintFilterCanvas& canvas)
{
    if (node.IsMainThreadNode()) {
        return false;
    } else if (RSMainThread::Instance()->GetDeviceType() == DeviceType::PC &&
        !node.QueryIfAllHwcChildrenForceDisabledByFilter()) {
        return false; // this node should do DSS composition in mainThread although it is assigned to subThread
    }
    if (!node.HasCachedTexture()) {
        RS_TRACE_NAME_FMT("HandleSubThreadNode wait %" PRIu64 "", node.GetId());
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
        RSSubThreadManager::Instance()->WaitNodeTask(node.GetId());
        node.UpdateCompletedCacheSurface();
#endif
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderUtil::HandleSubThreadNode %" PRIu64 "", node.GetId());
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
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
            if (curNode->GetCacheSurfaceProcessedStatus() == CacheProcessStatus::DOING) {
                RSSubThreadManager::Instance()->WaitNodeTask(curNode->GetId());
            }
#endif
            return false;
        }
    }
    return false;
}

int RSUniRenderUtil::GetRotationFromMatrix(Drawing::Matrix matrix)
{
    Drawing::Matrix::Buffer value;
    matrix.GetAll(value);

    int rAngle = static_cast<int>(-round(atan2(value[Drawing::Matrix::Index::SKEW_X],
        value[Drawing::Matrix::Index::SCALE_X]) * (180 / PI)));
    // transfer the result to anti-clockwise degrees
    // only rotation with 90°, 180°, 270° are composed through hardware,
    // in which situation the transformation of the layer needs to be set.
    static const std::map<int, int> supportedDegrees = {{90, 270}, {180, 180}, {-90, 90}, {-180, 180}};
    auto iter = supportedDegrees.find(rAngle);
    return iter != supportedDegrees.end() ? iter->second : 0;
}

int RSUniRenderUtil::GetRotationDegreeFromMatrix(Drawing::Matrix matrix)
{
    Drawing::Matrix::Buffer value;
    matrix.GetAll(value);
    return static_cast<int>(-round(atan2(value[Drawing::Matrix::Index::SKEW_X],
        value[Drawing::Matrix::Index::SCALE_X]) * (180 / PI)));
}

bool RSUniRenderUtil::Is3DRotation(Drawing::Matrix matrix)
{
    return !ROSEN_EQ(matrix.Get(Drawing::Matrix::Index::SKEW_X), 0.f) ||
        matrix.Get(Drawing::Matrix::Index::SCALE_X) < 0 ||
        !ROSEN_EQ(matrix.Get(Drawing::Matrix::Index::SKEW_Y), 0.f) ||
        matrix.Get(Drawing::Matrix::Index::SCALE_Y) < 0;
}


void RSUniRenderUtil::ReleaseColorPickerFilter(std::shared_ptr<RSFilter> RSFilter)
{
    auto materialFilter = std::static_pointer_cast<RSMaterialFilter>(RSFilter);
    if (materialFilter->GetColorPickerCacheTask() == nullptr) {
        return;
    }
    materialFilter->ReleaseColorPickerFilter();
}

void RSUniRenderUtil::ReleaseColorPickerResource(std::shared_ptr<RSRenderNode>& node)
{
    if (node == nullptr) {
        return;
    }
    auto& properties = node->GetRenderProperties();
    if (properties.GetColorPickerCacheTaskShadow() != nullptr) {
        properties.ReleaseColorPickerTaskShadow();
    }
    if ((properties.GetFilter() != nullptr &&
        properties.GetFilter()->GetFilterType() == RSFilter::MATERIAL)) {
        ReleaseColorPickerFilter(properties.GetFilter());
    }
    if (properties.GetBackgroundFilter() != nullptr &&
        properties.GetBackgroundFilter()->GetFilterType() == RSFilter::MATERIAL) {
        ReleaseColorPickerFilter(properties.GetBackgroundFilter());
    }
    // Recursive to release color picker resource
    for (auto& child : *node->GetChildren()) {
        if (auto canvasChild = RSBaseRenderNode::ReinterpretCast<RSRenderNode>(child)) {
            ReleaseColorPickerResource(canvasChild);
        }
    }
}

bool RSUniRenderUtil::IsNodeAssignSubThread(std::shared_ptr<RSSurfaceRenderNode> node, bool isDisplayRotation)
{
    bool isPhoneType = RSMainThread::Instance()->GetDeviceType() == DeviceType::PHONE;
    bool isNeedAssignToSubThread = false;
    if (isPhoneType && node->IsLeashWindow()) {
        isNeedAssignToSubThread = (node->IsScale() || ROSEN_EQ(node->GetGlobalAlpha(), 0.0f) ||
            node->GetForceUIFirst()) && !node->HasFilter();
        RS_TRACE_NAME_FMT("Assign info: name[%s] id[%lu]"
            " status:%d filter:%d isScale:%d forceUIFirst:%d isNeedAssign:%d",
            node->GetName().c_str(), node->GetId(), node->GetCacheSurfaceProcessedStatus(),
            node->HasFilter(), node->IsScale(), node->GetForceUIFirst(), isNeedAssignToSubThread);
    }
    std::string surfaceName = node->GetName();
    bool needFilterSCB = surfaceName.substr(0, 3) == "SCB" ||
        surfaceName.substr(0, 13) == "BlurComponent"; // filter BlurComponent, 13 is string len
    if (needFilterSCB || node->IsSelfDrawingType()) {
        return false;
    }
    if (node->GetCacheSurfaceProcessedStatus() == CacheProcessStatus::DOING) { // node exceed one vsync
        return true;
    }
    if (isPhoneType) {
        return isNeedAssignToSubThread;
    } else { // PC or TABLET
        if ((node->IsFocusedNode(RSMainThread::Instance()->GetFocusNodeId()) ||
            node->IsFocusedNode(RSMainThread::Instance()->GetFocusLeashWindowId())) &&
            node->GetHasSharedTransitionNode()) {
            return false;
        }
        return node->QuerySubAssignable(isDisplayRotation);
    }
}

void RSUniRenderUtil::AssignWindowNodes(const std::shared_ptr<RSDisplayRenderNode>& displayNode,
    std::list<std::shared_ptr<RSSurfaceRenderNode>>& mainThreadNodes,
    std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes)
{
    if (displayNode == nullptr) {
        ROSEN_LOGE("RSUniRenderUtil::AssignWindowNodes display node is null");
        return;
    }
    bool isRotation = displayNode->IsRotationChanged();
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    if (Rosen::SceneBoardJudgement::IsSceneBoardEnabled()) {
        displayNode->CollectSurface(displayNode, curAllSurfaces, true, true);
    } else {
        curAllSurfaces = *displayNode->GetSortedChildren();
    }
    for (auto iter = curAllSurfaces.begin(); iter != curAllSurfaces.end(); iter++) {
        auto node = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*iter);
        if (node == nullptr) {
            ROSEN_LOGE("RSUniRenderUtil::AssignWindowNodes nullptr found in sortedChildren, this should not happen");
            continue;
        }

        // release color picker resource when thread-switching between RS and subthread
        bool lastIsNeedAssignToSubThread = node->GetLastIsNeedAssignToSubThread();
        bool isNodeAssignSubThread = IsNodeAssignSubThread(node, isRotation);
        if (isNodeAssignSubThread != lastIsNeedAssignToSubThread) {
            auto renderNode = RSBaseRenderNode::ReinterpretCast<RSRenderNode>(node);
            ReleaseColorPickerResource(renderNode);
            node->SetLastIsNeedAssignToSubThread(isNodeAssignSubThread);
        }
        if (isNodeAssignSubThread) {
            AssignSubThreadNode(subThreadNodes, node);
        } else {
            AssignMainThreadNode(mainThreadNodes, node);
        }
    }
    SortSubThreadNodes(subThreadNodes);
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
        RS_LOGD("RSUniRenderUtil::AssignMainThreadNode clear cache surface:[%{public}s, %{public}" PRIu64 "]",
            node->GetName().c_str(), node->GetId());
        ClearCacheSurface(*node, UNI_MAIN_THREAD_INDEX);
        node->SetIsMainThreadNode(true);
        node->SetTextureValidFlag(false);
    }
    if (RSMainThread::Instance()->GetDeviceType() == DeviceType::PC) {
        RS_TRACE_NAME_FMT("AssignMainThread: name: %s, id: %lu, [HasTransparentSurface: %d, ChildHasVisibleFilter: %d,"
            "HasFilter: %d, HasAbilityComponent: %d, QueryIfAllHwcChildrenForceDisabledByFilter: %d]",
            node->GetName().c_str(), node->GetId(), node->GetHasTransparentSurface(),
            node->ChildHasVisibleFilter(), node->HasFilter(), node->HasAbilityComponent(),
            node->QueryIfAllHwcChildrenForceDisabledByFilter());
    }
}

void RSUniRenderUtil::AssignSubThreadNode(
    std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes, const std::shared_ptr<RSSurfaceRenderNode>& node)
{
    if (node == nullptr) {
        ROSEN_LOGW("RSUniRenderUtil::AssignSubThreadNode node is nullptr");
        return;
    }
    node->SetNeedSubmitSubThread(true);
    node->SetCacheType(CacheType::CONTENT);
    node->SetIsMainThreadNode(false);
    auto deviceType = RSMainThread::Instance()->GetDeviceType();
    // skip complete static window, DO NOT assign it to subthread.
    if (node->GetCacheSurfaceProcessedStatus() == CacheProcessStatus::DONE &&
        node->HasCachedTexture() && node->IsUIFirstSelfDrawCheck() &&
        (node->IsCurFrameStatic(deviceType) || node->IsVisibleDirtyEmpty(deviceType))) {
        node->SetNeedSubmitSubThread(false);
        RS_OPTIONAL_TRACE_NAME_FMT("subThreadNodes : static skip %s", node->GetName().c_str());
    } else {
        node->UpdateCacheSurfaceDirtyManager(2); // 2 means buffer age
    }
    node->SetLastFrameChildrenCnt(node->GetChildren()->size());
    subThreadNodes.emplace_back(node);
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (node->GetCacheSurfaceProcessedStatus() == CacheProcessStatus::DONE &&
        node->GetCacheSurface(UNI_MAIN_THREAD_INDEX, false) && node->GetCacheSurfaceNeedUpdated()) {
        node->UpdateCompletedCacheSurface();
        if (node->IsAppWindow() &&
            !RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node->GetParent().lock())) {
            node->GetDirtyManager()->MergeDirtyRect(node->GetOldDirty());
        } else {
            for (auto& child : *node->GetSortedChildren()) {
                auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
                if (surfaceNode && surfaceNode->IsAppWindow()) {
                    surfaceNode->GetDirtyManager()->MergeDirtyRect(surfaceNode->GetOldDirty());
                    break;
                }
            }
        }
        node->SetCacheSurfaceNeedUpdated(false);
    }
#endif
    bool isFocus = node->IsFocusedNode(RSMainThread::Instance()->GetFocusNodeId()) ||
        (node->IsFocusedNode(RSMainThread::Instance()->GetFocusLeashWindowId()));
    if ((deviceType == DeviceType::PC || deviceType == DeviceType::TABLET) && isFocus) {
        node->SetPriority(NodePriorityType::SUB_FOCUSNODE_PRIORITY); // for resolving response latency
        return;
    }
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
        for (auto& child : *node->GetSortedChildren()) {
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
    std::set<std::shared_ptr<RSBaseRenderNode>>& oldChildren,
    DeviceType deviceType)
{
    if (displayNode == nullptr) {
        return;
    }
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    if (Rosen::SceneBoardJudgement::IsSceneBoardEnabled()) {
        displayNode->CollectSurface(displayNode, curAllSurfaces, true, true);
    } else {
        curAllSurfaces = *displayNode->GetSortedChildren();
    }
    std::set<std::shared_ptr<RSBaseRenderNode>> tmpSet(curAllSurfaces.begin(), curAllSurfaces.end());
    for (auto& child : oldChildren) {
        auto surface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
        if (tmpSet.count(surface) == 0) {
            if (surface && surface->GetCacheSurfaceProcessedStatus() == CacheProcessStatus::DOING) {
                tmpSet.emplace(surface);
                continue;
            }
            if (surface && map.GetRenderNode(surface->GetId()) != nullptr) {
                RS_LOGD("RSUniRenderUtil::ClearSurfaceIfNeed clear cache surface:[%{public}s, %{public}" PRIu64 "]",
                    surface->GetName().c_str(), surface->GetId());
                if (deviceType == DeviceType::PHONE) {
                    ClearCacheSurface(*surface, UNI_MAIN_THREAD_INDEX);
                    surface->SetIsMainThreadNode(true);
                    surface->SetTextureValidFlag(false);
                } else {
                    if (RSMainThread::Instance()->IsPCThreeFingerScenesListScene()) {
                        ClearCacheSurface(*surface, UNI_MAIN_THREAD_INDEX, false);
                    } else {
                        ClearCacheSurface(*surface, UNI_MAIN_THREAD_INDEX);
                    }
                }
            }
        }
    }
    oldChildren.swap(tmpSet);
}

void RSUniRenderUtil::ClearCacheSurface(RSRenderNode& node, uint32_t threadIndex, bool isClearCompletedCacheSurface)
{
    RS_LOGD("ClearCacheSurface node: [%{public}" PRIu64 "]", node.GetId());
    uint32_t cacheSurfaceThreadIndex = node.GetCacheSurfaceThreadIndex();
    uint32_t completedSurfaceThreadIndex = node.GetCompletedSurfaceThreadIndex();
    if (cacheSurfaceThreadIndex == threadIndex && completedSurfaceThreadIndex == threadIndex) {
        node.ClearCacheSurface(isClearCompletedCacheSurface);
        return;
    }
    std::shared_ptr<Drawing::Surface> completedCacheSurface = isClearCompletedCacheSurface ?
        node.GetCompletedCacheSurface(threadIndex, false, true) : nullptr;
    ClearNodeCacheSurface(node.GetCacheSurface(threadIndex, false, true),
        std::move(completedCacheSurface), cacheSurfaceThreadIndex, completedSurfaceThreadIndex);
    node.ClearCacheSurface(isClearCompletedCacheSurface);
}

void RSUniRenderUtil::ClearNodeCacheSurface(std::shared_ptr<Drawing::Surface>&& cacheSurface,
    std::shared_ptr<Drawing::Surface>&& cacheCompletedSurface,
    uint32_t cacheSurfaceThreadIndex, uint32_t completedSurfaceThreadIndex)
{
    PostReleaseSurfaceTask(std::move(cacheSurface), cacheSurfaceThreadIndex);
    PostReleaseSurfaceTask(std::move(cacheCompletedSurface), completedSurfaceThreadIndex);
}

void RSUniRenderUtil::PostReleaseSurfaceTask(std::shared_ptr<Drawing::Surface>&& surface, uint32_t threadIndex)
{
    if (surface == nullptr) {
        return;
    }

    if (threadIndex == UNI_MAIN_THREAD_INDEX) {
        auto instance = RSMainThread::Instance();
        instance->AddToReleaseQueue(std::move(surface));
        instance->PostTask([instance] () {
            instance->ReleaseSurface();
        });
    } else {
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
        auto instance = RSSubThreadManager::Instance();
        instance->AddToReleaseQueue(std::move(surface), threadIndex);
        instance->ReleaseSurface(threadIndex);
#endif
    }
}

void RSUniRenderUtil::FloorTransXYInCanvasMatrix(RSPaintFilterCanvas& canvas)
{
    auto matrix = canvas.GetTotalMatrix();
    matrix.Set(Drawing::Matrix::TRANS_X, std::floor(matrix.Get(Drawing::Matrix::TRANS_X)));
    matrix.Set(Drawing::Matrix::TRANS_Y, std::floor(matrix.Get(Drawing::Matrix::TRANS_Y)));
    canvas.SetMatrix(matrix);
}
} // namespace Rosen
} // namespace OHOS
