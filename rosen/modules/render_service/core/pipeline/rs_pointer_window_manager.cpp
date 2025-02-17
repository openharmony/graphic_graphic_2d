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
#include "common/rs_optional_trace.h"
#include "pipeline/rs_main_thread.h"
#ifdef RS_ENABLE_GPU
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "screen_manager/screen_types.h"
#endif
namespace OHOS {
namespace Rosen {
RSPointerWindowManager& RSPointerWindowManager::Instance()
{
    static RSPointerWindowManager instance;
    return instance;
}

void RSPointerWindowManager::UpdatePointerDirtyToGlobalDirty(std::shared_ptr<RSSurfaceRenderNode>& pointWindow,
    std::shared_ptr<RSDisplayRenderNode>& curDisplayNode)
{
    if (pointWindow == nullptr || curDisplayNode == nullptr) {
        return;
    }
    auto dirtyManager = pointWindow->GetDirtyManager();
    if (dirtyManager && pointWindow->GetHardCursorStatus()) {
        if (!pointWindow->GetHardCursorLastStatus()) {
            RectI lastFrameSurfacePos = curDisplayNode->GetLastFrameSurfacePos(pointWindow->GetId());
            curDisplayNode->GetDirtyManager()->MergeDirtyRect(lastFrameSurfacePos);
        }
        auto pointerWindowDirtyRegion = dirtyManager->GetCurrentFrameDirtyRegion();
        if (!pointerWindowDirtyRegion.IsEmpty()) {
            curDisplayNode->GetDirtyManager()->MergeHwcDirtyRect(pointerWindowDirtyRegion);
            dirtyManager->SetCurrentFrameDirtyRect(RectI());
            isNeedForceCommitByPointer_ = true;
        } else {
            isNeedForceCommitByPointer_ = false;
        }
    }
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
    RSUniRenderUtil::UpdateHwcNodeProperty(surfaceNode);

    // 2.update (layerInfo.matrix = TotalMatrix)
    if (surfaceNode->GetScreenId() != INVALID_SCREEN_ID) {
        auto screenManager = CreateOrGetScreenManager();
        if (!screenManager) {
            RS_LOGE("RSPointerWindowManager::UpdatePointerInfo screenManager is null!");
            return;
        }
        auto screenInfo = screenManager->QueryScreenInfo(surfaceNode->GetScreenId());
        auto transform = RSUniRenderUtil::GetLayerTransform(*surfaceNode, screenInfo);
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
    auto nodeId = hardCursorNode->GetId();
    hardCursorNodeMap_.emplace(nodeId, hardCursorNode);
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
            RS_OPTIONAL_TRACE_NAME("HardCursorCreateLayerForDirect create layer");
            processor->CreateLayer(*hardCursorNode, *params);
        }
    }
#endif
}

bool RSPointerWindowManager::CheckHardCursorSupport(std::shared_ptr<RSDisplayRenderNode>& curDisplayNode)
{
    if (curDisplayNode == nullptr) {
        return false;
    }
    auto screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        return false;
    }
    return screenManager->GetDisplayPropertyForHardCursor(curDisplayNode->GetScreenId());
}

bool RSPointerWindowManager::HasMirrorDisplay() const
{
    const std::shared_ptr<RSBaseRenderNode> rootNode =
        RSMainThread::Instance()->GetContext().GetGlobalRootRenderNode();
    if (rootNode == nullptr || rootNode->GetChildrenCount() <= 1) {
        return false;
    }
    for (auto& child : *rootNode->GetSortedChildren()) {
        if (!child || !child->IsInstanceOf<RSDisplayRenderNode>()) {
            continue;
        }
        auto displayNode = child->ReinterpretCastTo<RSDisplayRenderNode>();
        if (!displayNode) {
            continue;
        }
        if (displayNode->IsMirrorDisplay()) {
            return true;
        }
    }
    return false;
}
} // namespace Rosen
} // namespace OHOS