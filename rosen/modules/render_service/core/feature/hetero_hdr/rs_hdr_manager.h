/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RS_HDR_MANAGER_H
#define RS_HDR_MANAGER_H

#include <condition_variable>
#include <unordered_map>
#include <set>
#include <vector>

#include "drawable/rs_surface_render_node_drawable.h"
#include "ffrt.h"
#include "ffrt_inner.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_processor.h"
#include "pipeline/rs_surface_render_node.h"
#include "transaction/rs_render_service_client.h"

constexpr int GRAPH_NUM = 4;

namespace OHOS::Rosen {
#define ROTATION_90 90
#define ROTATION_270 270
#define HDR_EPSILON 1e-8f
#define HPAE_TIMEOUT 32

class RSHdrManager {
public:
    static RSHdrManager &Instance();
    void UpdateHdrNodes(RSSurfaceRenderNode &node, bool isCurrentFrameBufferConsumed);
    void PostHdrSubTasks();
    bool GetCurFrameHeterogHandleCanBeUesed();
    bool IsHeterogComputingHdrOn()
    {
        return isHeterogComputingHdrOn_;
    }
    
private:
    RSHdrManager();
    ~RSHdrManager() = default;
    RSHdrManager(const RSHdrManager &);
    RSHdrManager(const RSHdrManager &&);
    RSHdrManager &operator = (const RSHdrManager &);
    RSHdrManager &operator = (const RSHdrManager &&);
    void ReverseParentNode(std::shared_ptr<RSRenderNode> node);
    bool EquRect(RectI a, RectI b);
    RectI RectRound(RectI src);

    void GetFixDstRectStatus(bool isUifistR, RSSurfaceRenderParams *surfaceParams, RectI &boundSize,
        bool &isFixDstRect);
    int32_t BuildHDRTask(RSSurfaceRenderParams *surfaceParams, MdcRectT srcRect, uint32_t* taskId, void** taskPtr);
    bool IsHDRSurfaceNodeSkipped(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> surfaceDrawable);
    bool PrepareHapeTask(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> surfaceDrawable,
        uint64_t curFrameId);
    void FindParentLeashWindowNode();

    std::shared_ptr<RSSurfaceRenderNode> windowSceneNode_;
    std::unordered_map<NodeId, NodeId> ownedLeashWindowIdMap_;
    std::shared_ptr<RSSurfaceRenderNode> appWindowNode_;
    std::unordered_map<NodeId, NodeId> ownedAppWindowIdMap_;
    std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> pendingPostNodes_;
   
    std::unordered_map<NodeId, RectI> nodeSrcRect_;
    std::unordered_map<NodeId, bool> isCurrentFrameBufferConsumedMap_;
    void* taskPtr_ = nullptr;
    std::atomic<uint32_t> taskId_ = 0;
    uint32_t buildHDRTaskStatus = 0;
    int32_t queryTaskStatus = 0;
    std::mutex HpaeMtx_;
    std::condition_variable cond_;
    std::atomic<bool> destroyedFlag_{true};
    bool isFixedDstBuffer_ = false;
    RectI dst_ =  {0, 0, 0, 0};
    sptr<SurfaceBuffer> dstBuffer_ = nullptr;
    bool curFrameHeterogHandleCanBeUesed_ = false;
    bool isHeterogComputingHdrOn_ = false;

    void ProcessPendingNode(uint32_t nodeId, uint64_t curFrameId);
    bool CheckWindowOwnership(uint32_t nodeId);
    bool ValidateSurface(RSSurfaceRenderParams* surfaceParams);
    bool PrepareAndSubmitHdrTask(
        std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable,
        RSSurfaceRenderParams* surfaceParams, uint32_t nodeId, uint64_t curFrameId);
};
}
#endif // RS_HDR_MANAGER_H
