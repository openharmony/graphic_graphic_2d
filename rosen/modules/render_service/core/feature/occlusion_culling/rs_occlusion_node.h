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

protected:
    bool Contains3dTransformation(const RSProperties& renderProperties) const
    {
        return !ROSEN_EQ(renderProperties.GetTranslateZ(), 0.f) || !ROSEN_EQ(renderProperties.GetRotationX(), 0.f) ||
            !ROSEN_EQ(renderProperties.GetRotationY(), 0.f) || !renderProperties.GetQuaternion().IsIdentity() ||
            !ROSEN_EQ(renderProperties.GetScaleZ(), 1.f) || !ROSEN_EQ(renderProperties.GetSkewZ(), 0.f) ||
            !ROSEN_EQ(renderProperties.GetPivotZ(), 0.f) || !ROSEN_EQ(renderProperties.GetCameraDistance(), 0.f) ||
            !ROSEN_EQ(renderProperties.GetPerspZ(), 0.f) || !ROSEN_EQ(renderProperties.GetPerspW(), 1.f);
    }

    int16_t SafeCast(float value) const
    {
        return static_cast<int16_t>(std::clamp(value, static_cast<float>(INT16_MIN), static_cast<float>(INT16_MAX)));
    }

    RectI16 ComputeOuter(const RectF& drawRect) const
    {
        if (drawRect.IsEmpty()) {
            return RectI16();
        }
        auto left = SafeCast(std::floor(drawRect.GetLeft()));
        auto right = SafeCast(std::ceil(drawRect.GetRight()));
        auto top = SafeCast(std::floor(drawRect.GetTop()));
        auto bottom = SafeCast(std::ceil(drawRect.GetBottom()));
        return (left >= right || top >= bottom) ? RectI16() : RectI16(left, top, right - left, bottom - top);
    }

    RectI16 ComputeInner(const RectF& drawRect, const Vector4f& cornerRadius) const
    {
        if (drawRect.IsEmpty()) {
            return RectI16();
        }
        auto left = SafeCast(std::ceil(drawRect.GetLeft()));
        auto right = SafeCast(std::floor(drawRect.GetRight()));
        auto top = SafeCast(std::ceil(drawRect.GetTop() + std::max(cornerRadius.x_, cornerRadius.y_)));
        auto bottom = SafeCast(std::floor(drawRect.GetBottom() - std::max(cornerRadius.w_, cornerRadius.z_)));
        return (left >= right || top >= bottom) ? RectI16() : RectI16(left, top, right - left, bottom - top);
    }

    RectI16 ComputeInner(const RRect& clipRRect) const
    {
        Vector4f cornerRadius {
            std::max(clipRRect.radius_[0].x_, clipRRect.radius_[0].y_),
            std::max(clipRRect.radius_[1].x_, clipRRect.radius_[1].y_),
            std::max(clipRRect.radius_[2].x_, clipRRect.radius_[2].y_),
            std::max(clipRRect.radius_[3].x_, clipRRect.radius_[3].y_)
        };
        return ComputeInner(clipRRect.rect_, cornerRadius);
    }

    void TransformByCenterScalingAndTranslation(RectF& rect, const Vector2f& centerPoint,
        const Vector2f& scale, const Vector2f& translate) const
    {
        rect.left_ = centerPoint.x_ + (rect.left_ - centerPoint.x_) * scale.x_ + translate.x_;
        rect.top_ = centerPoint.y_ + (rect.top_ - centerPoint.y_) * scale.y_ + translate.y_;
        rect.width_ = rect.width_ * scale.x_;
        rect.height_ = rect.height_ * scale.y_;
    }

    void TransformToAbsoluteCoord(RectF& rect, const Vector2f& totalScale, const Vector2f& parentAbsPos) const
    {
        rect.left_ *= totalScale.x_;
        rect.top_ *= totalScale.y_;
        rect.width_ *= totalScale.x_;
        rect.height_ *= totalScale.y_;
        rect.Move(parentAbsPos.x_, parentAbsPos.y_);
    }

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
    RectI16 FindMaxDisjointSubRect(const RectI16& baseRect, const RectI16& clipRect) const;

    uint64_t id_ = INVALID_NODEID;
    uint64_t occludedById_ = INVALID_NODEID;
    RSRenderNodeType type_ = RSRenderNodeType::UNKNOW;
    bool isSubTreeIgnored_ = false;
    bool hasChildrenOutOfRect_ = false;
    bool isNeedClip_ = false;
    bool isNeedClipFrame_ = false;
    bool isOutOfRootRect_ = false;
    bool isAlphaNeed_ = false;
    bool isBgOpaque_ = false;
    bool needFilter_ = false;
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
    std::unique_ptr<RRect> clipRRect_;

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
