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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_NODE_MAP_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_NODE_MAP_H

#include <mutex>
#include <unordered_map>

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSBaseNode;
class RSNode;

class RSNodeMap final {
public:
    static RSNodeMap& Instance();

    bool RegisterNode(const std::shared_ptr<RSBaseNode>& nodePtr);
    void UnregisterNode(NodeId id);

    std::weak_ptr<RSBaseNode> GetNode(NodeId id);

    std::shared_ptr<RSNode> GetAnimationFallbackNode();
private:
    explicit RSNodeMap();
    ~RSNodeMap() = default;
    RSNodeMap(const RSNodeMap&) = delete;
    RSNodeMap(const RSNodeMap&&) = delete;
    RSNodeMap& operator=(const RSNodeMap&) = delete;
    RSNodeMap& operator=(const RSNodeMap&&) = delete;

private:
    std::unordered_map<NodeId, std::shared_ptr<RSBaseNode>> nodeMap_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_NODE_MAP_H
