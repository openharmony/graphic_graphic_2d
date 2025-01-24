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

#ifndef RENDER_SERVICE_CORE_RS_RCD_RENDER_MANAGER_H
#define RENDER_SERVICE_CORE_RS_RCD_RENDER_MANAGER_H

#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "pipeline/rs_context.h"
#include "pipeline/rs_processor.h"
#include "rs_rcd_surface_render_node.h"
#include "feature/round_corner_display/rs_round_corner_config.h"

namespace OHOS {
namespace Rosen {
using RSRcdSurfaceRenderNodePtr = RSRcdSurfaceRenderNode::SharedPtr;
using RSRcdSurfaceRenderNodePtrMap = std::unordered_map<NodeId, RSRcdSurfaceRenderNode::SharedPtr>;
using RSRcdSurfaceRenderNodePtrVec = std::vector<RSRcdSurfaceRenderNodePtr>;
struct RcdPrepareInfo {
    RSBaseRenderNode::SharedPtr bottomNode = nullptr;
    RSBaseRenderNode::SharedPtr topNode = nullptr;
    bool hasInvalidScene = false;
};

struct RcdProcessInfo {
    std::shared_ptr<RSProcessor> uniProcessor = nullptr;
    std::shared_ptr<rs_rcd::RoundCornerLayer> topLayer = nullptr;
    std::shared_ptr<rs_rcd::RoundCornerLayer> bottomLayer = nullptr;
    RectT<uint32_t> displayRect;
    bool resourceChanged = false;
};

struct RcdInfo {
    RcdPrepareInfo prepareInfo;
    RcdProcessInfo processInfo;
};

class RSRcdRenderManager {
public:
    static RSRcdRenderManager& GetInstance();
    static void InitInstance();
    bool GetRcdRenderEnabled() const;

    void CheckRenderTargetNode(const RSContext& context);
    RSRcdSurfaceRenderNodePtr GetTopSurfaceNode(NodeId id);
    RSRcdSurfaceRenderNodePtr GetBottomSurfaceNode(NodeId id);

    void DoProcessRenderTask(NodeId id, const RcdProcessInfo& info);

    void DoProcessRenderMainThreadTask(NodeId id, const RcdProcessInfo& info);

    static bool IsRcdProcessInfoValid(const RcdProcessInfo& info);

    RSRcdRenderManager() = default;
    virtual ~RSRcdRenderManager() = default;

private:
    static bool CheckExist(NodeId id, const RSRenderNodeMap& map);
    RSRcdSurfaceRenderNodePtr GetTopRenderNode(NodeId id);
    RSRcdSurfaceRenderNodePtr GetBottomRenderNode(NodeId id);
    void RemoveRcdResource(NodeId id);

    bool rcdRenderEnabled_ = false;
    constexpr static NodeId TOP_RCD_NODE_ID = 1;
    constexpr static NodeId BACKGROUND_RCD_NODE_ID = 2;

    std::mutex topNodeMapMut_;
    std::mutex bottomNodeMapMut_;
    // key nodeId of display screen, rcd rendernode
    RSRcdSurfaceRenderNodePtrMap topSurfaceNodeMap_;
    RSRcdSurfaceRenderNodePtrMap bottomSurfaceNodeMap_;

    bool isBufferCacheClear_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_RS_RCD_RENDER_MANAGER_H