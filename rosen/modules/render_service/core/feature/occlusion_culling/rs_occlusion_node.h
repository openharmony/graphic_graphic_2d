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

#ifndef ROSEN_RENDER_SERVICE_PIPELINE_RS_OCCLUSION_NODE_H
#define ROSEN_RENDER_SERVICE_PIPELINE_RS_OCCLUSION_NODE_H
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "property/rs_properties.h"

namespace OHOS {
namespace Rosen {
typedef RectT<int16_t> RectI16;

struct OcclusionCoverageInfo {
    uint64_t id_ = 0;
    RectI16 rect_ = RectI16();
    int area_ = 0;
};

class RSB_EXPORT OcclusionNode : public std::enable_shared_from_this<OcclusionNode> {
public:
    OcclusionNode(NodeId id, RSRenderNodeType type) : id_(id), type_(type){};
    OcclusionNode() = delete;
    NodeId GetId() const {
        return id_;
    }
    std::weak_ptr<OcclusionNode> GetParentOcNode() const {
        return parentOcNode_;
    }
    void UpdateChildrenOutOfRectInfo(bool hasChildrenOutOfRect) {
        hasChildrenOutOfRect_ = hasChildrenOutOfRect;
    }
    void MarkAsRootOcclusionNode() {
        isValidInCurrentFrame_ = true;
        rootOcclusionNode_ = weak_from_this();
    }
    bool IsSubTreeIgnored() const {
        return isSubTreeIgnored_;
    }
    bool IsValid(const float value)
    {
        return !std::isinf(value) && !std::isnan(value);
    }
    void ForwardOrderInsert(std::shared_ptr<OcclusionNode> newNode);
    bool RemoveChild(const std::shared_ptr<OcclusionNode>& child);
    void RemoveSubTree(std::unordered_map<NodeId, std::shared_ptr<OcclusionNode>>& occlusionNodes);
    void CollectNodeProperties(const RSRenderNode& node);
    void CalculateNodeAllBounds();
    void UpdateClipRect(const RSRenderNode& node);
    void UpdateSubTreeProp();
    void DetectOcclusion(std::unordered_set<NodeId>& culledNodes, std::unordered_set<NodeId>& culledEntireSubtree,
        std::unordered_set<NodeId>& offTreeNodes);
    void PreorderTraversal(std::vector<std::shared_ptr<OcclusionNode>>& result);
    std::string GetOcclusionNodeInfoString();
private:
    void CalculateDrawRect(const RSRenderNode& node, const RSProperties& renderProperties);
    bool IsOutOfRootRect(const RectI16 &rect);
    bool IsSubTreeShouldIgnored(const RSRenderNode& node, const RSProperties& renderProperties);
    bool IsOpaque() const {
        return isBgOpaque_ && !isAlphaNeed_;
    }
    OcclusionCoverageInfo DetectOcclusionInner(OcclusionCoverageInfo& globalCoverInfo,
        std::unordered_set<NodeId>& culledNodes, std::unordered_set<NodeId>& culledEntireSubtree,
        std::unordered_set<NodeId>& offTreeNodes);
    void CheckNodeOcclusion(OcclusionCoverageInfo& coverageInfo, std::unordered_set<NodeId>& culledNodes);
    void CheckNodeOcclusion(OcclusionCoverageInfo& coverageInfo, std::unordered_set<NodeId>& culledNodes,
        std::unordered_set<NodeId>& culledEntireSubtree);
    void UpdateCoverageInfo(OcclusionCoverageInfo& globalCoverage, OcclusionCoverageInfo& selfCoverage);

    uint64_t id_ = INVALID_NODEID;
    uint64_t occludedById_ = INVALID_NODEID;
    RSRenderNodeType type_ = RSRenderNodeType::UNKNOW;
    bool isSubTreeIgnored_ = false;
    bool hasChildrenOutOfRect_ = false;
    bool isNeedClip_ = false;
    bool isOutOfRootRect_ = false;
    bool isAlphaNeed_ = false;
    bool isBgOpaque_ = false;
    // Indicates whether the node was inserted or updated in the current frame.
    // Nodes not inserted or updated in the current frame will be removed from the occlusion tree.
    bool isValidInCurrentFrame_ = false;

    // Spatial information
    Vector2f localPosition_;
    Vector2f absPositions_;
    Vector2f localScale_ = {1.0f, 1.0f};
    Vector2f accumulatedScale_ = {1.0f, 1.0f};
    float localAlpha_ = 1.0f;
    float accumulatedAlpha_ = 1.0f;
    Vector4f cornerRadius_;
    RectF drawRect_;
    RectI16 clipOuterRect_;
    RectI16 clipInnerRect_;
    RectI16 outerRect_;
    RectI16 innerRect_;

    // Node relationship
    std::weak_ptr<OcclusionNode> rootOcclusionNode_;
    std::weak_ptr<OcclusionNode> parentOcNode_;
    std::shared_ptr<OcclusionNode> firstChild_ = nullptr;
    std::shared_ptr<OcclusionNode> lastChild_ = nullptr;
    // Same-level node relationships
    std::weak_ptr<OcclusionNode> leftSibling_;
    std::weak_ptr<OcclusionNode> rightSibling_;
};

} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_PIPELINE_RS_OCCLUSION_NODE_H
