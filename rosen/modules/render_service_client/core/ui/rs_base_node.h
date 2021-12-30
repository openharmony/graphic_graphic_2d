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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_BASE_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_BASE_NODE_H

#include <memory>
#include <unistd.h>
#include <vector>

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSTransitionEffect;

class RS_EXPORT RSBaseNode {
public:
    using WeakPtr = std::weak_ptr<RSBaseNode>;
    using SharedPtr = std::shared_ptr<RSBaseNode>;
    virtual ~RSBaseNode();

    void AddChild(SharedPtr child, int index);
    void RemoveChild(SharedPtr child);
    void RemoveFromTree();
    void ClearChildren();

    NodeId GetId() const
    {
        return id_;
    }

    const std::vector<NodeId>& GetChildren() const
    {
        return children_;
    }

protected:
    RSBaseNode();
    RSBaseNode(const RSBaseNode&) = delete;
    RSBaseNode(const RSBaseNode&&) = delete;
    RSBaseNode& operator=(const RSBaseNode&) = delete;
    RSBaseNode& operator=(const RSBaseNode&&) = delete;

    virtual void OnAddChildren() {}
    virtual void OnRemoveChildren() {}
    void SetParent(SharedPtr parent);
    SharedPtr GetParent();

    void SetId(const NodeId& id)
    {
        id_ = id;
    }

    void DumpTree(std::string& out);

private:
    static inline pid_t pid_ = getpid();
    static inline std::atomic<uint32_t> currentId_ = 1;
    NodeId id_;
    NodeId parent_ = 0;
    std::vector<NodeId> children_;
    NodeId GenerateNodeId();

    friend class RSUIDirector;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_BASE_NODE_H
