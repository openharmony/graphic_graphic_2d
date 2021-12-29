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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_BASE_RENDER_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_BASE_RENDER_NODE_H

#include <memory>
#include <vector>

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSNodeVisitor;

class RSBaseRenderNode : public std::enable_shared_from_this<RSBaseRenderNode> {
public:
    using WeakPtr = std::weak_ptr<RSBaseRenderNode>;
    using SharedPtr = std::shared_ptr<RSBaseRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::BASE_NODE;

    explicit RSBaseRenderNode(NodeId id);
    virtual ~RSBaseRenderNode();

    void AddChild(SharedPtr child, int index = -1);
    void RemoveChild(SharedPtr child);
    void ClearChildren();
    void RemoveFromTree();

    virtual void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor);
    virtual void Process(const std::shared_ptr<RSNodeVisitor>& visitor);

    void SetParent(WeakPtr parent);
    WeakPtr GetParent();
    void ResetParent();

    NodeId GetId() const
    {
        return id_;
    }

    std::vector<WeakPtr>& GetChildren()
    {
        return children_;
    }

    const std::vector<WeakPtr>& GetChildren() const
    {
        return children_;
    }

    void DumpTree(std::string& out) const;

    void SetPendingRemoval(bool pendingRemoval)
    {
        pendingRemoval_ = pendingRemoval;
    }
    bool IsPendingRemoval()
    {
        return pendingRemoval_;
    }

    virtual void OnAddChild(RSBaseRenderNode::SharedPtr& child) {};
    virtual void OnRemoveChild(RSBaseRenderNode::SharedPtr& child) {};
    virtual bool OnUnregister()
    {
        return true;
    }

    virtual RSRenderNodeType GetType()
    {
        return RSRenderNodeType::BASE_NODE;
    }

    template<typename T>
    bool IsInstanceOf();

    // type-safe reinterpret_cast
    template<typename T>
    static std::shared_ptr<T> ReinterpretCast(const std::shared_ptr<RSBaseRenderNode>& node)
    {
        return (node && node->IsInstanceOf<T>()) ? std::static_pointer_cast<T>(node) : nullptr;
    }
    template<typename T>
    std::shared_ptr<T> ReinterpretCastTo()
    {
        return (IsInstanceOf<T>()) ? std::static_pointer_cast<T>(shared_from_this()) : nullptr;
    }

protected:
    enum class NodeDirty {
        CLEAN = 0,
        DIRTY,
    };
    virtual bool IsDirty() const;
    void SetDirty();
    void SetClean();

private:
    NodeId id_;
    WeakPtr parent_;
    std::vector<WeakPtr> children_;

    NodeDirty dirtyStatus_ = NodeDirty::DIRTY;
    bool pendingRemoval_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_BASE_RENDER_NODE_H
