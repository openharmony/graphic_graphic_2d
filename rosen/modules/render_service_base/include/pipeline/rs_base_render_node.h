/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <list>
#include <memory>

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSContext;
class RSNodeVisitor;

class RSBaseRenderNode : public std::enable_shared_from_this<RSBaseRenderNode> {
public:
    using WeakPtr = std::weak_ptr<RSBaseRenderNode>;
    using SharedPtr = std::shared_ptr<RSBaseRenderNode>;
    static inline constexpr RSRenderNodeType Type = RSRenderNodeType::BASE_NODE;

    explicit RSBaseRenderNode(NodeId id, std::weak_ptr<RSContext> context = {}) : id_(id), context_(context) {};
    explicit RSBaseRenderNode(NodeId id, bool isOnTheTree = false, std::weak_ptr<RSContext> context = {}) : id_(id),
        isOnTheTree_(isOnTheTree), context_(context) {};
    virtual ~RSBaseRenderNode() = default;

    void AddChild(SharedPtr child, int index = -1);
    void RemoveChild(SharedPtr child);
    // Add/RemoveCrossParentChild only used as: the child is under multiple parents(e.g. a window cross multi-screens)
    void AddCrossParentChild(const SharedPtr& child, int32_t index = -1);
    void RemoveCrossParentChild(const SharedPtr& child, const WeakPtr& newParent);
    void ClearChildren();
    void RemoveFromTree();
    virtual void CollectSurface(const std::shared_ptr<RSBaseRenderNode>& node,
                                std::vector<RSBaseRenderNode::SharedPtr>& vec);
    virtual void Prepare(const std::shared_ptr<RSNodeVisitor>& visitor);
    virtual void Process(const std::shared_ptr<RSNodeVisitor>& visitor);

    // return if any animation is running
    virtual bool Animate(int64_t timestamp)
    {
        return false;
    }

    WeakPtr GetParent() const;
    void ResetParent();

    NodeId GetId() const
    {
        return id_;
    }

    void SetIsOnTheTree(bool flag);
    bool IsOnTheTree() const
    {
        return isOnTheTree_;
    }

    const std::list<SharedPtr>& GetSortedChildren();

    void ResetSortedChildren()
    {
        sortedChildren_.clear();
    }

    uint32_t GetChildrenCount() const
    {
        return children_.size();
    }

    void DumpTree(int32_t depth, std::string& ou) const;

    virtual bool HasDisappearingTransition(bool recursive = true) const
    {
        if (recursive == false) {
            return false;
        } else {
            auto parent = GetParent().lock();
            return parent ? parent->HasDisappearingTransition(true) : false;
        }
    }

    virtual RSRenderNodeType GetType() const
    {
        return RSRenderNodeType::BASE_NODE;
    }

    template<typename T>
    bool IsInstanceOf();

    // type-safe reinterpret_cast
    template<typename T>
    static std::shared_ptr<T> ReinterpretCast(std::shared_ptr<RSBaseRenderNode> node)
    {
        return node ? node->ReinterpretCastTo<T>() : nullptr;
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

    void DumpNodeType(std::string& out) const;

    const std::weak_ptr<RSContext> GetContext() const
    {
        return context_;
    }

private:
    NodeId id_;

    WeakPtr parent_;
    void SetParent(WeakPtr parent);
    bool isOnTheTree_ = false;

    std::list<WeakPtr> children_;
    std::list<std::pair<SharedPtr, uint32_t>> disappearingChildren_;

    std::list<SharedPtr> sortedChildren_;
    void GenerateSortedChildren();

    const std::weak_ptr<RSContext> context_;
    NodeDirty dirtyStatus_ = NodeDirty::DIRTY;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_BASE_RENDER_NODE_H
