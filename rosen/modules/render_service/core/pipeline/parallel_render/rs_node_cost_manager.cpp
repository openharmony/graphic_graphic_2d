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

#include "rs_node_cost_manager.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_system_properties.h"
#include "rs_parallel_render_manager.h"
#include "rs_trace.h"
namespace OHOS::Rosen {

RSNodeCostManager::RSNodeCostManager(bool doAnimate, bool opDropped, bool isSecurityDisplay)
{
    doAnimate_ = doAnimate;
    isOpDropped_ = opDropped;
    isSecurityDisplay_ = isSecurityDisplay;
    isOcclusionEnabled_ = RSSystemProperties::GetOcclusionEnabled();
    partialRenderType_ = RSSystemProperties::GetPartialRenderEnabled();
}

void RSNodeCostManager::AddNodeCost(int32_t cost)
{
    dirtyNodeCost_ += cost;
}

int32_t RSNodeCostManager::GetDirtyNodeCost() const
{
    return dirtyNodeCost_;
}

void RSNodeCostManager::CalcNodeCost(RSSurfaceRenderNode& node)
{
    dirtyNodeCost_ = 0;
    CalcSurfaceRenderNodeCost(node);
}

bool RSNodeCostManager::IsSkipProcessing(RSSurfaceRenderNode& node) const
{
    if (isSecurityDisplay_ && node.GetSecurityLayer()) {
        return true;
    }
    if (!node.ShouldPaint()) {
        return true;
    }
    if (!node.GetOcclusionVisible() && !doAnimate_ && isOcclusionEnabled_ && !isSecurityDisplay_) {
        return true;
    }
    if (node.IsAbilityComponent() && node.GetDstRect().IsEmpty()) {
        return true;
    }
#ifdef RS_ENABLE_EGLQUERYSURFACE
    // skip clean surface node
    if (isOpDropped_ && node.IsAppWindow()) {
        if (!node.SubNodeNeedDraw(node.GetOldDirtyInSurface(), partialRenderType_)) {
            return true;
        }
    }
#endif
    return false;
}

void RSNodeCostManager::CalcBaseRenderNodeCost(RSBaseRenderNode& node)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::OPENGL) {
        return;
    }

    for (auto& child : *node.GetChildren()) {
        switch (child->GetType()) {
            case RSRenderNodeType::SURFACE_NODE: {
                auto surfaceNodePtr = child->ReinterpretCastTo<RSSurfaceRenderNode>();
                CalcSurfaceRenderNodeCost(*surfaceNodePtr);
                break;
            }
            case RSRenderNodeType::CANVAS_NODE: {
                auto canvasNodePtr = child->ReinterpretCastTo<RSCanvasRenderNode>();
                CalcCanvasRenderNodeCost(*canvasNodePtr);
                break;
            }
            case RSRenderNodeType::RS_NODE:
            case RSRenderNodeType::DISPLAY_NODE:
            case RSRenderNodeType::PROXY_NODE:
            case RSRenderNodeType::ROOT_NODE: {
                CalcBaseRenderNodeCost(*child);
                break;
            }
            default: {
                break;
            }
        }
    }
#endif
}

void RSNodeCostManager::CalcCanvasRenderNodeCost(RSCanvasRenderNode& node)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::OPENGL) {
        return;
    }
    if (!node.ShouldPaint()) {
        return;
    }
#ifdef RS_ENABLE_EGLQUERYSURFACE
    if (isOpDropped_ && costSurfaceNode_ &&
        !costSurfaceNode_->SubNodeNeedDraw(node.GetOldDirtyInSurface(), partialRenderType_) &&
        !node.HasChildrenOutOfRect()) {
        return;
    }
#endif
    if ((costSurfaceNode_ == nullptr) ||
        (!costSurfaceNode_->SubNodeNeedDraw(node.GetOldDirtyInSurface(), partialRenderType_) &&
        !node.HasChildrenOutOfRect())) {
        return;
    }
    auto parallelRenderManager = RSParallelRenderManager::Instance();
    AddNodeCost(parallelRenderManager->GetCost(node));
    CalcBaseRenderNodeCost(node);
#endif
}

void RSNodeCostManager::CalcSurfaceRenderNodeCost(RSSurfaceRenderNode& node)
{
#if defined(RS_ENABLE_PARALLEL_RENDER) && defined(RS_ENABLE_GL)
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::OPENGL) {
        return;
    }
    if (IsSkipProcessing(node)) {
        RS_TRACE_NAME(node.GetName() + " QuickReject Skip");
        return;
    }
    switch (node.GetSurfaceNodeType()) {
        case RSSurfaceNodeType::SELF_DRAWING_NODE:
        case RSSurfaceNodeType::ABILITY_COMPONENT_NODE: {
            auto parallelRenderManager = RSParallelRenderManager::Instance();
            AddNodeCost(parallelRenderManager->GetSelfDrawNodeCost());
            return;
        }
        case RSSurfaceNodeType::LEASH_WINDOW_NODE: {
            CalcBaseRenderNodeCost(node);
            return;
        }
        case RSSurfaceNodeType::APP_WINDOW_NODE: {
            RS_TRACE_NAME_FMT("CalcSurfaceRenderNodeCost %s",
                node.GetName().c_str());
            costSurfaceNode_ = node.ReinterpretCastTo<RSSurfaceRenderNode>();
            CalcBaseRenderNodeCost(node);
            RS_TRACE_NAME_FMT("CalcSurfaceRenderNodeCost  %s cost %d", node.GetName().c_str(),
                GetDirtyNodeCost());
            break;
        }
        case RSSurfaceNodeType::STARTING_WINDOW_NODE:
        case RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE:
        default: {
            return;
        }
    }
#endif
}
} // OHOS::Rosen