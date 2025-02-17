/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "rs_rcd_render_manager.h"

#include <unordered_set>
#include "common/rs_optional_trace.h"
#include "common/rs_singleton.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "feature/round_corner_display/rs_message_bus.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "platform/common/rs_log.h"
#include "rs_rcd_render_visitor.h"
#include "rs_round_corner_display_manager.h"

namespace OHOS {
namespace Rosen {
static std::unique_ptr<RSRcdRenderManager> g_rcdRenderManagerInstance =
    std::make_unique<RSRcdRenderManager>();

RSRcdRenderManager& RSRcdRenderManager::GetInstance()
{
    return *g_rcdRenderManagerInstance;
}

void RSRcdRenderManager::InitInstance()
{
    g_rcdRenderManagerInstance->rcdRenderEnabled_ = true;
}

bool RSRcdRenderManager::GetRcdRenderEnabled() const
{
    return rcdRenderEnabled_;
}

bool RSRcdRenderManager::IsRcdProcessInfoValid(const RcdProcessInfo& info)
{
    if (info.uniProcessor == nullptr) {
        RS_LOGE("info uniProcessor is nullptr");
        return false;
    } else if (info.topLayer == nullptr || info.bottomLayer == nullptr) {
        RS_LOGE("info toplayer or bottomlayer resource is nullptr");
        return false;
    }
    return true;
}

bool RSRcdRenderManager::CheckExist(NodeId id, const RSRenderNodeMap& map)
{
    const auto& node = map.GetRenderNode<RSDisplayRenderNode>(id);
    if (node == nullptr) {
        return false;
    }
    return true;
}

void RSRcdRenderManager::RemoveRcdResource(NodeId id)
{
    // to remove the resource which rendertarget display node not exist
    auto rmTask = [id, this]() {
        {
            std::lock_guard<std::mutex> lock(topNodeMapMut_);
            topSurfaceNodeMap_.erase(id);
        }
        {
            std::lock_guard<std::mutex> lock(bottomNodeMapMut_);
            bottomSurfaceNodeMap_.erase(id);
        }
        RSSingleton<RoundCornerDisplayManager>::GetInstance().RemoveRCDResource(id);
        RS_LOGI_IF(DEBUG_PIPELINE,
            "[%{public}s] nodeId:%{public}" PRIu64 " not exist in nodeMap remove the rcd module \n",
            __func__, id);
    };
    RSUniRenderThread::Instance().PostTask(rmTask);
}

void RSRcdRenderManager::CheckRenderTargetNode(const RSContext& context)
{
    const auto& nodeMap = context.GetNodeMap();
    std::unordered_set<NodeId> idSets;
    {
        std::lock_guard<std::mutex> lock(topNodeMapMut_);
        for (const auto& p : topSurfaceNodeMap_) {
            idSets.insert(p.first);
        }
    }
    {
        std::lock_guard<std::mutex> lock(bottomNodeMapMut_);
        for (const auto& p : bottomSurfaceNodeMap_) {
            idSets.insert(p.first);
        }
    }
    for (auto it = idSets.begin(); it != idSets.end(); it++) {
        if (!CheckExist(*it, nodeMap)) {
            // to remove the resource which rendertarget display node not exist
            RemoveRcdResource(*it);
        } else {
            RS_LOGD("[%{public}s] nodeId:%{public}" PRIu64 " detected in nodeMap \n", __func__, *it);
        }
    }
}

RSRcdSurfaceRenderNodePtr RSRcdRenderManager::GetTopSurfaceNode(NodeId id)
{
    std::lock_guard<std::mutex> lock(topNodeMapMut_);
    auto it = topSurfaceNodeMap_.find(id);
    if (it != topSurfaceNodeMap_.end() && it->second != nullptr) {
        return it->second;
    }
    return nullptr;
}

RSRcdSurfaceRenderNodePtr RSRcdRenderManager::GetBottomSurfaceNode(NodeId id)
{
    std::lock_guard<std::mutex> lock(bottomNodeMapMut_);
    auto it = bottomSurfaceNodeMap_.find(id);
    if (it != bottomSurfaceNodeMap_.end() && it->second != nullptr) {
        return it->second;
    }
    return nullptr;
}

RSRcdSurfaceRenderNodePtr RSRcdRenderManager::GetTopRenderNode(NodeId id)
{
    std::lock_guard<std::mutex> lock(topNodeMapMut_);
    auto it = topSurfaceNodeMap_.find(id);
    if (it != topSurfaceNodeMap_.end() && it->second != nullptr) {
        return it->second;
    }
    // create and insert
    auto topRcdNode = RSRcdSurfaceRenderNode::Create(TOP_RCD_NODE_ID, RCDSurfaceType::TOP);
    topRcdNode->SetRenderTargetId(id);
    topSurfaceNodeMap_[id] = topRcdNode;
    RS_LOGI_IF(DEBUG_PIPELINE, "RCD: insert a top rendernode");
    return topRcdNode;
}

RSRcdSurfaceRenderNodePtr RSRcdRenderManager::GetBottomRenderNode(NodeId id)
{
    std::lock_guard<std::mutex> lock(bottomNodeMapMut_);
    auto it = bottomSurfaceNodeMap_.find(id);
    if (it != bottomSurfaceNodeMap_.end() && it->second != nullptr) {
        return it->second;
    }
    // create and insert
    auto bottomRcdNode = RSRcdSurfaceRenderNode::Create(BACKGROUND_RCD_NODE_ID, RCDSurfaceType::BOTTOM);
    bottomRcdNode->SetRenderTargetId(id);
    bottomSurfaceNodeMap_[id] = bottomRcdNode;
    RS_LOGI_IF(DEBUG_PIPELINE, "RCD: insert a bottom rendernode");
    return bottomRcdNode;
}

void RSRcdRenderManager::DoProcessRenderTask(NodeId id, const RcdProcessInfo& info)
{
    RS_TRACE_BEGIN("RSUniRender:DoRCDProcessTask");
    if (!IsRcdProcessInfoValid(info)) {
        RS_LOGE("RCD: RcdProcessInfo is incorrect");
        RS_TRACE_END();
        return;
    }
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    visitor->SetUniProcessor(info.uniProcessor);
    RSRcdSurfaceRenderNodePtr bottomPtr = GetBottomRenderNode(id);
    RSRcdSurfaceRenderNodePtr topPtr = GetTopRenderNode(id);
    bottomPtr->SetRenderDisplayRect(info.displayRect);
    topPtr->SetRenderDisplayRect(info.displayRect);
    auto bottomRes = visitor->ProcessRcdSurfaceRenderNode(*bottomPtr, info.bottomLayer,
        info.resourceChanged);
    auto topRes = visitor->ProcessRcdSurfaceRenderNode(*topPtr, info.topLayer, info.resourceChanged);
    if (info.resourceChanged && bottomRes && topRes) {
        RSSingleton<RsMessageBus>::GetInstance().SendMsg<NodeId, bool>(TOPIC_RCD_DISPLAY_HWRESOURCE, id, true);
    }
    RS_TRACE_END();
}

void RSRcdRenderManager::DoProcessRenderMainThreadTask(NodeId id, const RcdProcessInfo& info)
{
    RS_TRACE_BEGIN("RSUniRender:DoRCDProcessMainThreadTask");
    if (!IsRcdProcessInfoValid(info)) {
        RS_LOGE("RCD: RcdProcessInfo in MainThread is incorrect");
        RS_TRACE_END();
        return;
    }
    auto visitor = std::make_shared<RSRcdRenderVisitor>();
    visitor->SetUniProcessor(info.uniProcessor);
    visitor->ProcessRcdSurfaceRenderNodeMainThread(*GetBottomRenderNode(id), info.resourceChanged);
    visitor->ProcessRcdSurfaceRenderNodeMainThread(*GetTopRenderNode(id), info.resourceChanged);
    RS_TRACE_END();
}

} // namespace Rosen
} // namespace OHOS