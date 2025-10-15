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
#include "feature/watermark/rs_surface_watermark.h"
#include "pipeline/rs_screen_render_node.h"
namespace OHOS {
namespace Rosen {
constexpr uint32_t MAX_LIMIT_SURFACE_WATER_MARK_IMG = 1000;
uint32_t RSSurfaceWatermarkHelper::SetSurfaceWatermark(pid_t pid,
    const std::string& name, std::shared_ptr<Media::PixelMap> pixelMap, const std::vector<NodeId>& nodeIdList,
    SurfaceWatermarkType watermarkType,
    RSContext& mainContext, bool isSystemCalling)
{
    auto iter = surfaceWatermarks_.find(name);
    if (iter == surfaceWatermarks_.end() && pixelMap == nullptr) {
        return WATER_MARK_PIXELMAP_INVALID;
    }
    // When SCB is restored, the pixelMap is passed as a null pointer.
    std::shared_ptr<Drawing::Image> tmpImagePtr = (pixelMap == nullptr) ? iter->second.first :
        RSPixelMapUtil::ExtractDrawingImage(pixelMap);

    if (tmpImagePtr == nullptr) {
        RS_LOGE("RSSurfaceWatermarkHelper::SetSurfaceWatermark tmpImagePtr is nullptr");
        return WATER_MARK_RS_CONNECTION_ERROR;
    }

    auto func = [&tmpImagePtr, &watermarkType, &mainContext,
        &isSystemCalling, this](const std::string& name, std::shared_ptr<RSSurfaceRenderNode> node) {
        node->SetWatermarkEnabled(name, true, watermarkType);
        AddWatermarkNameMapNodeId(name, node->GetId(), watermarkType);
        // CUSTOM_WATER_MARK need limit screen size and surface size
        bool needCompareWindowSize = (watermarkType == CUSTOM_WATER_MARK);
        if (!hasNodeOperationSuccess_ && needCompareWindowSize) {
            auto imgHeight = tmpImagePtr->GetHeight();
            auto imgWidth = tmpImagePtr->GetWidth();
            auto screenInfo = GetScreenInfo(node, mainContext);
            auto pixelMapSize = imgHeight * imgWidth;

            bool upperNodeSize = (pixelMapSize > (node->GetRenderProperties().GetBoundsHeight() *
                node->GetRenderProperties().GetBoundsWidth()));
            bool upperscreenSize = (pixelMapSize > (static_cast<int32_t>(screenInfo.height) *
                static_cast<int32_t>(screenInfo.width)));
            if (upperNodeSize && upperscreenSize) {
                RS_LOGE("RSSurfaceWatermarkHelper::SetSurfaceWatermark watermark size error imgWidth:[%{public}d],"
                    "imgHeight:[%{public}d] nodeWidth:[%{public}d], nodeHeight:[%{public}d],"
                    "screenWidth:[%{public}d], scrrenHeight:[%{public}d], nodeId:[%{public}" PRIu64 "]",
                    imgWidth, imgHeight,
                    static_cast<int32_t>(node->GetRenderProperties().GetBoundsWidth()),
                    static_cast<int32_t>(node->GetRenderProperties().GetBoundsHeight()),
                    static_cast<int32_t>(screenInfo.width),
                    static_cast<int32_t>(screenInfo.height),
                    node->GetId()
                );
                return WATER_MARK_IMG_SIZE_ERROR;
            }
        }
        return WATER_MARK_SUCCESS;
    };
    TraverseAndOperateNodeList(pid, name, nodeIdList, mainContext, func, isSystemCalling);
    // As long as one watermark is successfully applied to the node, the watermark image can be stored.
    if (watermarkType == CUSTOM_WATER_MARK && !hasNodeOperationSuccess_) {
        return globalErrorCode_;
    }
    // Adding a new watermark image requires verification to check if the limit has been reached.
    if (iter == surfaceWatermarks_.end()) {
        if (registerSurfaceWatermarkCount_ >= MAX_LIMIT_SURFACE_WATER_MARK_IMG) {
            RS_LOGE("RSSurfaceWatermarkHelper::SetSurfaceWatermark max limit surface water mark img");
            return WATER_MARK_RS_CONNECTION_ERROR;
        }
        registerSurfaceWatermarkCount_++;
    }

    globalErrorCode_ &= ~(static_cast<uint32_t>(WATER_MARK_IMG_ASTC_ERROR));
    surfaceWatermarks_[name] = {tmpImagePtr, pid};
    return globalErrorCode_;
}

template<typename Container>
void RSSurfaceWatermarkHelper::TraverseAndOperateNodeList(const pid_t& pid, const std::string& name,
    const Container& nodeIdList, RSContext& mainContext,
    std::function<uint32_t(const std::string& name, std::shared_ptr<RSSurfaceRenderNode>)> func,
    const bool& isSystemCalling)
{
    globalErrorCode_ = 0;
    hasNodeOperationSuccess_ = false;
    for (const auto& nodeId : nodeIdList) {
        // Not System calls must have the same pid.
        bool isErrorPermission = (!isSystemCalling && pid != ExtractPid(nodeId));
        if (isErrorPermission) {
            globalErrorCode_ |= static_cast<uint32_t>(WATER_MARK_PERMISSION_ERROR);
            continue;
        }

        auto surfaceNode = mainContext.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
        if (!surfaceNode) {
            RemoveNodeIdForWatermarkNameMapNodeId(name, nodeId);
            RS_LOGE("RSSurfaceWatermarkHelper::TraverseAndOperateNodeList not find node"
                "nodeId:[%{public}" PRIu64 "]", nodeId);
            globalErrorCode_ |= WATER_MARK_NOT_SURFACE_NODE_ERROR;
            continue;
        }
        
        auto errorCode = func(name, surfaceNode);
        if (errorCode > 0) {
            globalErrorCode_ |= errorCode;
        } else {
            hasNodeOperationSuccess_ = true;
        }
    }
}

ScreenInfo RSSurfaceWatermarkHelper::GetScreenInfo(const std::shared_ptr<RSSurfaceRenderNode>& node,
    RSContext& mainContext)
{
    ScreenInfo info;
    auto screenNodeId = node->GetScreenNodeId();
    auto screenNode = mainContext.GetNodeMap().GetRenderNode<RSScreenRenderNode>(screenNodeId);
    if (!screenNode) {
        RS_LOGE("RSSurfaceWatermarkHelper::GetScreenInfo not find node"
            "nodeId:[%{public}" PRIu64 "]", node->GetId());
        return info;
    }
    info = screenNode->GetScreenInfo();
    return info;
}

void RSSurfaceWatermarkHelper::ClearSurfaceWatermarkForNodes(pid_t pid, const std::string& name,
    const std::vector<NodeId>& nodeIdList, RSContext& mainContext, bool isSystemCalling)
{
    auto watermarkType = GetWatermarkType(name);
    if (watermarkType == INVALID_WATER_MARK) {
        return;
    }
    auto func = [&watermarkType, this](const std::string& name, std::shared_ptr<RSSurfaceRenderNode> node) {
        node->ClearWatermarkEnabled(name, watermarkType);
        RemoveNodeIdForWatermarkNameMapNodeId(name, node->GetId());
        return WATER_MARK_SUCCESS;
    };
    TraverseAndOperateNodeList(pid, name, nodeIdList, mainContext, func, isSystemCalling);
}

void RSSurfaceWatermarkHelper::ClearSurfaceWatermark(pid_t pid, const std::string& name,
    RSContext& mainContext, bool isSystemCalling, bool isDeathMonitor)
{
    auto iter = surfaceWatermarks_.find(name);
    auto watermarkType = GetWatermarkType(name);
    if (watermarkType == INVALID_WATER_MARK && iter == surfaceWatermarks_.end()) {
        return;
    }
    if (!CheckClearWatermarkPermission(pid, name, isSystemCalling)) {
        return;
    }
    // currently, system watermark has not been transmitted to the node. Unified incorporation in the future
    std::unordered_set<NodeId> nodeIdList = (watermarkType != INVALID_WATER_MARK) ?
        watermarkNameMapNodeId_[name].first : std::unordered_set<NodeId>{};
    auto func = [&watermarkType](const std::string& name, std::shared_ptr<RSSurfaceRenderNode> node) {
        node->ClearWatermarkEnabled(name, watermarkType);
        return WATER_MARK_SUCCESS;
    };
    std::vector<NodeId> vecNodeIdList(nodeIdList.begin(), nodeIdList.end()); // compiler adapter
    TraverseAndOperateNodeList(pid, name, vecNodeIdList, mainContext, func, isSystemCalling);
    registerSurfaceWatermarkCount_ = (registerSurfaceWatermarkCount_ > 0) ? --registerSurfaceWatermarkCount_ : 0;
    if (!isDeathMonitor) {
        surfaceWatermarks_.erase(name);
    }
}

void RSSurfaceWatermarkHelper::ClearSurfaceWatermark(pid_t pid, RSContext& mainContext)
{
    EraseIf(surfaceWatermarks_, [pid, this, &mainContext](const auto& pair) {
        if (pair.second.second != pid) {
            return false;
        }
        auto name = pair.first;
        ClearSurfaceWatermark(pid, name, mainContext, true, true);
        return true;
    });
}

void RSSurfaceWatermarkHelper::AddWatermarkNameMapNodeId(std::string name, NodeId id,
    SurfaceWatermarkType watermarkType)
{
    auto iter = watermarkNameMapNodeId_.find(name);
    if (iter == watermarkNameMapNodeId_.end()) {
        std::unordered_set<NodeId> setNodeId = {id};
        watermarkNameMapNodeId_[name] = {setNodeId, watermarkType};
    } else {
        iter->second.first.insert(id);
    }
}
}
}