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

#ifndef RENDER_SERVICE_WINDOW_KEYFRAME_NODE_INFO_H
#define RENDER_SERVICE_WINDOW_KEYFRAME_NODE_INFO_H

#include <unordered_map>
#include "common/rs_common_def.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS::Rosen {

class RSWindowKeyframeNodeInfo {
public:
    RSWindowKeyframeNodeInfo() = default;
    ~RSWindowKeyframeNodeInfo() = default;

    void UpdateLinkedNodeId(NodeId srcNodeId, NodeId linkedNodeId);
    size_t GetLinkedNodeCount() const;
    void ClearLinkedNodeInfo();

    bool PrepareRootNodeOffscreen(RSSurfaceRenderNode& surfaceNode);

private:
    RSWindowKeyframeNodeInfo(const RSWindowKeyframeNodeInfo&) = delete;
    RSWindowKeyframeNodeInfo(const RSWindowKeyframeNodeInfo&&) = delete;
    RSWindowKeyframeNodeInfo& operator=(const RSWindowKeyframeNodeInfo&) = delete;
    RSWindowKeyframeNodeInfo& operator=(const RSWindowKeyframeNodeInfo&&) = delete;

    std::unordered_map<NodeId, NodeId> linkedNodeMap_;
};

} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_WINDOW_KEYFRAME_NODE_INFO_H
