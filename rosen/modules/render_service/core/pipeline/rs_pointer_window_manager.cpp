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

#include "pipeline/rs_pointer_window_manager.h"
#include "feature/hwc/rs_uni_hwc_compute_util.h"
#include "common/rs_optional_trace.h"
#include "feature_cfg/graphic_feature_param_manager.h"
#include "pipeline/main_thread/rs_main_thread.h"
#ifdef RS_ENABLE_GPU
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "screen_manager/screen_types.h"
#endif
namespace OHOS {
namespace Rosen {
namespace {
constexpr int MIN_LAYER_WIDTH = 2;
constexpr int MIN_LAYER_HEIGHT = 2;
}
RSPointerWindowManager& RSPointerWindowManager::Instance()
{
    static RSPointerWindowManager instance;
    return instance;
}

RSPointerWindowManager::RSPointerWindowManager()
{
    isHardCursorEnable_ = RSPointerWindowManager::GetHardCursorEnabledPass();
}

void RSPointerWindowManager::UpdatePointerDirtyToGlobalDirty(std::shared_ptr<RSSurfaceRenderNode>& pointWindow,
    std::shared_ptr<RSScreenRenderNode>& curScreenNode)
{
    if (pointWindow == nullptr || curScreenNode == nullptr) {
        return;
    }
    auto dirtyManager = pointWindow->GetDirtyManager();
    if (dirtyManager && pointWindow->GetHardCursorStatus()) {
        if (!pointWindow->GetHardCursorLastStatus()) {
            RectI lastFrameSurfacePos = curScreenNode->GetLastFrameSurfacePos(pointWindow->GetId());
            curScreenNode->GetDirtyManager()->MergeDirtyRect(lastFrameSurfacePos);
        }
        auto pointerWindowDirtyRegion = dirtyManager->GetCurrentFrameDirtyRegion();
        if (!pointerWindowDirtyRegion.IsEmpty()) {
            curScreenNode->GetDirtyManager()->MergeHwcDirtyRect(pointerWindowDirtyRegion);
            dirtyManager->SetCurrentFrameDirtyRect(RectI());
            isNeedForceCommitByPointer_ = true;
        } else {
            isNeedForceCommitByPointer_ = false;
        }
    }
}

bool RSPointerWindowManager::GetHardCursorEnabledPass()
{
    auto hardCursorFeatureParam = GraphicFeatureParamManager::GetInstance().GetFeatureParam("HardCursorConfig");
    auto hardCursorFeature = std::static_pointer_cast<HardCursorParam>(hardCursorFeatureParam);
    if (hardCursorFeature != nullptr) {
        return hardCursorFeature->IsHardCursorEnable();
    }
    return false;
}

void RSPointerWindowManager::UpdatePointerInfo()
{
#ifdef RS_ENABLE_GPU
    int64_t rsNodeId = 0;
    BoundParam boundTemp = {0.0f, 0.0f, 0.0f, 0.0f};
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (!BoundHasUpdateCompareChange(true, false)) {
            return;
        }
        rsNodeId = GetRsNodeId();
        boundTemp = GetBound();
    }

    if (rsNodeId <= 0) {
        return;
    }

    // prepare
    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(rsNodeId);
    if (node == nullptr) {
        return;
    }
    auto& properties = node->GetMutableRenderProperties();
    properties.SetBounds({boundTemp.x, boundTemp.y, boundTemp.z, boundTemp.w});
    node->SetDirty();
    properties.OnApplyModifiers();
    node->OnApplyModifiers();
    node->AddToPendingSyncList();

    // 1.Set TotalMatrix
    auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>();
    if (surfaceNode == nullptr) {
        return;
    }
    RSUniHwcComputeUtil::UpdateHwcNodeProperty(surfaceNode);

    // 2.update (layerInfo.matrix = TotalMatrix)
    if (surfaceNode->GetScreenId() != INVALID_SCREEN_ID) {
        auto screenManager = CreateOrGetScreenManager();
        if (!screenManager) {
            RS_LOGE("RSPointerWindowManager::UpdatePointerInfo screenManager is null!");
            return;
        }
        auto screenInfo = screenManager->QueryScreenInfo(surfaceNode->GetScreenId());
        auto transform = RSUniHwcComputeUtil::GetLayerTransform(*surfaceNode, screenInfo);
        surfaceNode->UpdateHwcNodeLayerInfo(transform, isPointerEnableHwc_);
    }
#endif
}

void RSPointerWindowManager::SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
    float positionZ, float positionW)
{
#ifdef RS_ENABLE_GPU
    // record status here
    {
        std::lock_guard<std::mutex> lock(mtx_);
        SetBound({positionX, positionY, positionZ, positionW});
        SetRsNodeId(rsNodeId);
    }
    SetBoundHasUpdate(true);
#endif
}

void RSPointerWindowManager::SetHardCursorNodeInfo(std::shared_ptr<RSSurfaceRenderNode> hardCursorNode)
{
    if (!hardCursorNode) {
        return;
    }
    if (!hardCursorNode->IsHardwareEnabledTopSurface() ||
        (!hardCursorNode->ShouldPaint() && !hardCursorNode->GetHardCursorStatus())) {
        return;
    }
    if (hardCursorNode->IsOnTheTree()) {
        auto nodeId = hardCursorNode->GetId();
        hardCursorNodeMap_.emplace(nodeId, hardCursorNode);
    }
}

const std::map<NodeId, std::shared_ptr<RSSurfaceRenderNode>>& RSPointerWindowManager::GetHardCursorNode() const
{
    return hardCursorNodeMap_;
}

void RSPointerWindowManager::HardCursorCreateLayerForDirect(std::shared_ptr<RSProcessor> processor)
{
#ifdef RS_ENABLE_GPU
    auto& hardCursorNodeMap = GetHardCursorNode();
    for (auto& [_, hardCursorNode] :  hardCursorNodeMap) {
        if (hardCursorNode && hardCursorNode->IsHardwareEnabledTopSurface()) {
            auto surfaceHandler = hardCursorNode->GetRSSurfaceHandler();
            if (!surfaceHandler) {
                continue;
            }
            auto params = static_cast<RSSurfaceRenderParams *>(hardCursorNode->GetStagingRenderParams().get());
            if (!params) {
                continue;
            }
            if (!surfaceHandler->IsCurrentFrameBufferConsumed() && params->GetPreBuffer() != nullptr) {
                params->SetPreBuffer(nullptr);
                hardCursorNode->AddToPendingSyncList();
            }
            processor->CreateLayer(*hardCursorNode, *params);
        }
    }
#endif
}

bool RSPointerWindowManager::CheckHardCursorSupport(uint32_t screenId)
{
    if (!isHardCursorEnable_) {
        return false;
    }
    auto screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        return false;
    }
    return screenManager->GetDisplayPropertyForHardCursor(screenId);
}

bool RSPointerWindowManager::HasMirrorDisplay() const
{
    const std::shared_ptr<RSBaseRenderNode> rootNode =
        RSMainThread::Instance()->GetContext().GetGlobalRootRenderNode();
    if (rootNode == nullptr || rootNode->GetChildrenCount() <= 1) {
        return false;
    }
    for (auto& child : *rootNode->GetSortedChildren()) {
        if (!child || !child->IsInstanceOf<RSScreenRenderNode>()) {
            continue;
        }
        auto screenNode = child->ReinterpretCastTo<RSScreenRenderNode>();
        if (!screenNode) {
            continue;
        }
        if (screenNode->IsMirrorScreen()) {
            return true;
        }
    }
    return false;
}

void RSPointerWindowManager::CollectAllHardCursor(
    RSSurfaceRenderNode& hardCursorNode, std::shared_ptr<RSScreenRenderNode>& curScreenNode,
    std::shared_ptr<RSLogicalDisplayRenderNode>& curDisplayNode)
{
    if (hardCursorNode.IsHardwareEnabledTopSurface() &&
        (hardCursorNode.ShouldPaint() || hardCursorNode.GetHardCursorStatus())) {
        auto surfaceNodeDrawable =
            std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(hardCursorNode.GetRenderDrawable());
        if (surfaceNodeDrawable && hardCursorNode.IsOnTheTree()) {
            hardCursorDrawableVec_.emplace_back(curScreenNode->GetId(),
                curDisplayNode->GetId(), hardCursorNode.GetRenderDrawable());
        }
    }
}

std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> RSPointerWindowManager::GetHardCursorDrawable(NodeId id)
{
    auto& renderThreadParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!renderThreadParams) {
        return nullptr;
    }
    auto& hardCursorDrawables = renderThreadParams->GetHardCursorDrawables();
    if (hardCursorDrawables.empty()) {
        return nullptr;
    }
    auto iter = std::find_if(hardCursorDrawables.begin(), hardCursorDrawables.end(), [id](const auto& node) {
        return id == std::get<0>(node);
    });
    if (iter == hardCursorDrawables.end()) {
        return nullptr;
    }
    auto& hardCursorDrawable = std::get<2>(*iter);
    if (!hardCursorDrawable) {
        return nullptr;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(hardCursorDrawable->GetRenderParams().get());
    if (!surfaceParams) {
        return nullptr;
    }
    auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(hardCursorDrawable);
    if (surfaceDrawable && surfaceParams->GetHardCursorStatus()) {
        return std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(surfaceDrawable);
    }
    return nullptr;
}

void RSPointerWindowManager::CheckHardCursorValid(const RSSurfaceRenderNode& node)
{
    if (!node.GetHardCursorStatus()) {
        return;
    }
    // DSS Hardware don't support the synthesis of layers with length and width not larger than 2
    auto srcRect = node.GetSrcRect();
    if (srcRect.GetWidth() <= MIN_LAYER_WIDTH || srcRect.GetHeight() <= MIN_LAYER_WIDTH) {
        const auto& property = node.GetRenderProperties();
        auto buffer = node.GetRSSurfaceHandler()->GetBuffer();
        if (buffer == nullptr) {
            RS_LOGE("RSPointerWindowManager::CheckHardCursorValid: buffer is nullptr");
            return;
        }
        auto bufferWidth = buffer->GetSurfaceBufferWidth();
        auto bufferHeight = buffer->GetSurfaceBufferHeight();
        const auto boundsWidth = property.GetBoundsWidth();
        const auto boundsHeight = property.GetBoundsHeight();
        RS_LOGE("hardcursor srcRect error, %{public}d, %{public}d, %{public}f, %{public}f",
            bufferWidth, bufferHeight, boundsWidth, boundsHeight);
    }
}
} // namespace Rosen
} // namespace OHOS