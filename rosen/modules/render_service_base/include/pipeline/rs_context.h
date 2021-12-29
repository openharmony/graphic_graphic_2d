/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_PIPELINE_RS_CONTEXT_H
#define ROSEN_RENDER_SERVICE_BASE_PIPELINE_RS_CONTEXT_H

#include "pipeline/rs_render_node_map.h"

namespace OHOS {
namespace Rosen {

class RSContext {
public:
    RSContext() = default;
    ~RSContext() = default;

    RSRenderNodeMap& GetNodeMap()
    {
        return nodeMap;
    }
    const RSRenderNodeMap& GetNodeMap() const
    {
        return nodeMap;
    }

    void SetGlobalRootRenderNode(const std::shared_ptr<RSBaseRenderNode>& rootNode)
    {
        globalRootRenderNode_ = rootNode;
    }

    const std::shared_ptr<RSBaseRenderNode>& GetGlobalRootRenderNode() const
    {
        return globalRootRenderNode_;
    }

private:
    static inline RSRenderNodeMap& nodeMap = RSRenderNodeMap::Instance();
    std::shared_ptr<RSBaseRenderNode> globalRootRenderNode_ = nullptr;

    RSContext(const RSContext&) = delete;
    RSContext(const RSContext&&) = delete;
    RSContext& operator=(const RSContext&) = delete;
    RSContext& operator=(const RSContext&&) = delete;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_PIPELINE_RS_CONTEXT_H
