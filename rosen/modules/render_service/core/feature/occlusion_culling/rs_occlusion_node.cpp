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

#include "feature/occlusion_culling/rs_occlusion_node.h"

#include <sstream>
#include "platform/common/rs_log.h"
#include "pipeline/rs_render_node.h"
#include "rs_trace.h"

namespace OHOS::Rosen {
// When a node has a modifier that is not in the OPAQUE_MODIFIERS, it is judged as a non opaque node.
const std::unordered_set<RSModifierType> OPAQUE_MODIFIERS = {
    RSModifierType::BOUNDS,
    RSModifierType::FRAME,
    RSModifierType::POSITION_Z,
    RSModifierType::PIVOT,
    RSModifierType::PIVOT_Z,
    RSModifierType::SCALE,
    RSModifierType::SCALE_Z,
    RSModifierType::TRANSLATE,
    RSModifierType::TRANSLATE_Z,
    RSModifierType::CORNER_RADIUS,
    RSModifierType::FOREGROUND_COLOR,
    RSModifierType::BACKGROUND_COLOR,
    RSModifierType::CLIP_RRECT,
    RSModifierType::CLIP_BOUNDS,
    RSModifierType::CLIP_TO_BOUNDS,
    RSModifierType::CLIP_TO_FRAME,
    RSModifierType::VISIBLE,
    RSModifierType::ALPHA,
    RSModifierType::ALPHA_OFFSCREEN,
    RSModifierType::BORDER_COLOR,
};
// When a node has a modifier that is not in the OPAQUE_MODIFIERS and OCCLUDER_MODIFIERS,
// it is determined that the subtree should not participate in occlusion culling.
const std::unordered_set<RSModifierType> OCCLUDER_MODIFIERS = {
    RSModifierType::BG_IMAGE,
    RSModifierType::BG_IMAGE_INNER_RECT,
    RSModifierType::BG_IMAGE_WIDTH,
    RSModifierType::BG_IMAGE_HEIGHT,
    RSModifierType::BG_IMAGE_POSITION_X,
    RSModifierType::BG_IMAGE_POSITION_Y,
    RSModifierType::BRIGHTNESS,
    RSModifierType::BORDER_WIDTH,
    RSModifierType::BORDER_STYLE,
    RSModifierType::BORDER_DASH_WIDTH,
    RSModifierType::BORDER_DASH_GAP,
    RSModifierType::CAMERA_DISTANCE,
    RSModifierType::SATURATE,
    RSModifierType::BACKGROUND_SHADER,
    RSModifierType::BACKGROUND_BLUR_RADIUS,
    RSModifierType::BACKGROUND_BLUR_SATURATION,
    RSModifierType::BACKGROUND_BLUR_BRIGHTNESS,
    RSModifierType::BACKGROUND_BLUR_MASK_COLOR,
    RSModifierType::BACKGROUND_BLUR_COLOR_MODE,
    RSModifierType::FOREGROUND_BLUR_RADIUS_X,
    RSModifierType::FOREGROUND_BLUR_RADIUS_Y,
    RSModifierType::ROTATION,
    RSModifierType::ROTATION_X,
    RSModifierType::ROTATION_Y,
    RSModifierType::FRAME_GRAVITY,
    RSModifierType::POSITION_Z_APPLICABLE_CAMERA3D,
    RSModifierType::GRAY_SCALE,
};

void OcclusionNode::ForwardOrderInsert(std::shared_ptr<OcclusionNode> newNode)
{
    if (!newNode) {
        return;
    }
    if (auto oldParent = newNode->parentOcNode_.lock()) {
        oldParent->RemoveChild(newNode);
    }
    newNode->parentOcNode_ = weak_from_this();
    if (!lastChild_) {
        firstChild_ = newNode;
    } else {
        lastChild_->rightSibling_ = newNode;
        newNode->leftSibling_ = lastChild_;
    }
    lastChild_ = newNode;
    newNode->isValidInCurrentFrame_ = true;
}

bool OcclusionNode::RemoveChild(const std::shared_ptr<OcclusionNode>& child)
{
    if (!child) {
        return false;
    }
    auto parentShared = child->parentOcNode_.lock();
    if (parentShared == nullptr || parentShared.get() != this) {
        return false;
    }
    auto left = child->leftSibling_.lock();
    if (left) {
        left->rightSibling_ = child->rightSibling_;
    }
    auto right = child->rightSibling_.lock();
    if (right) {
        right->leftSibling_ = child->leftSibling_;
    }
    if (child == firstChild_) {
        firstChild_ = right;
    }
    if (child == lastChild_) {
        lastChild_ = left;
    }
    child->parentOcNode_.reset();
    child->leftSibling_.reset();
    child->rightSibling_.reset();
    return true;
}

void OcclusionNode::RemoveSubTree(std::unordered_map<NodeId, std::shared_ptr<OcclusionNode>>& occlusionNodes)
{
    auto parentShared = parentOcNode_.lock();
    if (parentShared == nullptr) {
        return;
    }
    std::shared_ptr<OcclusionNode> child = lastChild_;
    parentShared->RemoveChild(shared_from_this());
    while (child) {
        auto childLeft = child->leftSibling_.lock();
        child->RemoveSubTree(occlusionNodes);
        child = childLeft;
    }
    occlusionNodes.erase(id_);
}

void OcclusionNode::CollectNodeProperties(const RSRenderNode& node)
{
    auto parentShared = parentOcNode_.lock();
    if (parentShared == nullptr || node.GetNodeGroupType() != RSRenderNode::NodeGroupType::NONE ||
        node.GetIsTextureExportNode() || node.GetSharedTransitionParam() != nullptr) {
        isSubTreeIgnored_ = true;
        return;
    }

    if (!isSubTreeIgnored_) {
        node.GetOcclusionInfo(OPAQUE_MODIFIERS, OCCLUDER_MODIFIERS, isOpaque_, isSubTreeIgnored_);
    }
    SetOcNodeOpaqueInfo(node);
    if (isSubTreeIgnored_) {
        return;
    }
    const auto& renderProperties = node.GetRenderProperties();
    const auto& skew = renderProperties.GetSkew();
    const auto& perspective = renderProperties.GetPersp();
    const auto& degree = renderProperties.GetRotation();
    const auto& degreeX = renderProperties.GetRotationX();
    const auto& degreeY = renderProperties.GetRotationY();

    // Skip this subtree if transformations (rotation/projection/skew/clip) make bounds non-rectangular.
    if (!ROSEN_EQ(skew[0], 0.f) || !ROSEN_EQ(skew[1], 0.f) ||
        !ROSEN_EQ(perspective[0], 0.f) || !ROSEN_EQ(perspective[1], 0.f) ||
        !ROSEN_EQ(degree, 0.f) || !ROSEN_EQ(degreeX, 0.f) || !ROSEN_EQ(degreeY, 0.f) ||
        renderProperties.GetClipBounds()) {
        isSubTreeIgnored_ = true;
        return;
    }

    isSubTreeIgnored_ = false;
    rootOcclusionNode_ = parentShared->rootOcclusionNode_;
    localScale_ = renderProperties.GetScale();
    localAlpha_ = renderProperties.GetAlpha();
    isNeedClip_ = renderProperties.GetClipToBounds() || renderProperties.GetClipToFrame() ||
        renderProperties.GetClipToRRect();
    cornerRadius_ = renderProperties.GetCornerRadius();

    CalculateDrawRect(node, renderProperties);
}

void OcclusionNode::SetOcNodeOpaqueInfo(const RSRenderNode& node)
{
    auto& renderProperties = node.GetRenderProperties();
    if (node.IsPureContainer() // Pure container node, no drawing content
        || node.GetDrawCmdModifiers().size() != 0 // Node has commands to draw
        || !(ROSEN_EQ(renderProperties.GetAlpha(), 1.0f)) // Node has alpha attribute
        || renderProperties.NeedFilter() // Node has filter attribute
        // Node's background color has alpha attribute and brightness is not valid
        || !(static_cast<uint8_t>(renderProperties.GetBackgroundColor().GetAlpha())
             >= UINT8_MAX && !renderProperties.IsBgBrightnessValid())) { // Node has commands to draw
        isOpaque_ &= false;
    }
}

void OcclusionNode::CalculateDrawRect(const RSRenderNode& node, const RSProperties& renderProperties)
{
    const auto& originBounds = renderProperties.GetBounds();
    const auto& translate = renderProperties.GetTranslate();
    const auto& center = renderProperties.GetPivot();
    auto clipRRect = renderProperties.GetClipRRect().rect_;

    // calculate new pos.
    auto centOffset = center * Vector2f(originBounds.z_, originBounds.w_);
    localPosition_ = Vector2f(originBounds.x_, originBounds.y_);
    localPosition_ += centOffset;
    localPosition_ -= centOffset * localScale_;
    localPosition_ += translate;

    // calculate drawRect.
    drawRect_ = RectF(localPosition_.x_, localPosition_.y_,
        originBounds.z_ * localScale_.x_, originBounds.w_ * localScale_.y_);
    if (isNeedClip_ && !clipRRect.IsEmpty()) {
        clipRRect.Move(translate.x_, translate.y_);
        drawRect_ = drawRect_.IntersectRect(clipRRect);
    }
}

static int16_t inline SafeCast(float value)
{
    return static_cast<int16_t>(std::clamp(value, static_cast<float>(INT16_MIN), static_cast<float>(INT16_MAX)));
}

static RectI16 inline ComputeOuter(RectF drawRect)
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

static RectI16 inline ComputeInner(RectF drawRect, Vector4f cornerRadius)
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

void OcclusionNode::CalculateNodeAllBounds()
{
    auto parentShared = parentOcNode_.lock();
    if (parentShared == nullptr || isSubTreeIgnored_) {
        return;
    }

    accumulatedScale_ = parentShared->localScale_ * parentShared->accumulatedScale_;
    accumulatedAlpha_ = parentShared->localAlpha_ * parentShared->accumulatedAlpha_;
    auto alpha = localAlpha_ * accumulatedAlpha_;
    isAlphaNeed_ = alpha < 1.f;
    isOpaque_ &= !isAlphaNeed_;
    absPositions_ = parentShared->absPositions_ + localPosition_ * accumulatedScale_;
    auto scaleDrawRect = RectF(drawRect_.GetLeft() * accumulatedScale_.x_,
        drawRect_.GetTop() * accumulatedScale_.y_,
        drawRect_.GetWidth() * accumulatedScale_.x_,
        drawRect_.GetHeight() * accumulatedScale_.y_);
    auto absDrawRect = scaleDrawRect.Offset(parentShared->absPositions_.x_, parentShared->absPositions_.y_);
    outerRect_ = ComputeOuter(absDrawRect);
    innerRect_ = ComputeInner(absDrawRect, cornerRadius_);
    clipOuterRect_ = parentShared->clipOuterRect_;
    clipInnerRect_ = parentShared->clipInnerRect_;
    outerRect_ = outerRect_.IntersectRect(clipOuterRect_);
    innerRect_ = innerRect_.IntersectRect(clipInnerRect_);

    if (isNeedClip_) {
        clipOuterRect_ = outerRect_;
        clipInnerRect_ = innerRect_;
    }

    if (innerRect_.IsEmpty()) {
        isOpaque_ = false;
    }
    isOutOfRootRect_ = IsOutOfRootRect(outerRect_);
}

bool OcclusionNode::IsOutOfRootRect(const RectI16 &absDrawRect)
{
    auto shared_root = rootOcclusionNode_.lock();
    if (!shared_root) {
        return false;
    }
    return absDrawRect.GetLeft() >= shared_root->clipOuterRect_.GetRight() ||
        absDrawRect.GetRight() < 0 ||
        absDrawRect.GetTop() >= shared_root->clipOuterRect_.GetBottom() ||
        absDrawRect.GetBottom() < 0 ||
        absDrawRect.IsEmpty();
}

void OcclusionNode::UpdateClipRect(const RSRenderNode& node)
{
    auto& renderProperties = node.GetRenderProperties();
    int16_t boundsWidth = renderProperties.GetBoundsWidth();
    int16_t boundsHeight = renderProperties.GetBoundsHeight();
    clipOuterRect_.SetAll(0, 0, boundsWidth, boundsHeight);
    clipInnerRect_.SetAll(0, 0, boundsWidth, boundsHeight);
}

void OcclusionNode::UpdateSubTreeProp()
{
    isValidInCurrentFrame_ = true;
    if (isSubTreeIgnored_) {
        return;
    }
    auto parentShared = parentOcNode_.lock();
    if (parentShared) {
        CalculateNodeAllBounds();
    }
    std::shared_ptr<OcclusionNode> child = lastChild_;
    while (child) {
        child->UpdateSubTreeProp();
        child = child->leftSibling_.lock();
    }
}

void OcclusionNode::DetectOcclusion(std::unordered_set<NodeId>& culledNodes, std::unordered_set<NodeId>& offTreeNodes)
{
    RS_TRACE_NAME_FMT("OcclusionNode::DetectOcclusion, root node id is : %lu", id_);
    OcclusionCoverageInfo globalCoverage;
    DetectOcclusionInner(globalCoverage, culledNodes, offTreeNodes);
}

OcclusionCoverageInfo OcclusionNode::DetectOcclusionInner(OcclusionCoverageInfo& globalCoverage,
    std::unordered_set<NodeId>& culledNodes, std::unordered_set<NodeId>& offTreeNodes)
{
    isValidInCurrentFrame_ = false;
    occludedById_ = 0;
    // Process nodes with high z-order first
    std::shared_ptr<OcclusionNode> child = lastChild_;
    OcclusionCoverageInfo maxChildCoverage;
    OcclusionCoverageInfo selfCoverage;
    // Before traversing child nodes, check if this node is already fully covered by the global occlusion region.
    // This check can be optimized to cull the entire subtree in the future.
    CheckNodeOcclusion(globalCoverage, culledNodes);
    while (child) {
        if (!child->isSubTreeIgnored_ && child->isValidInCurrentFrame_) {
            auto childCoverInfo = child->DetectOcclusionInner(globalCoverage, culledNodes, offTreeNodes);
            if (childCoverInfo.area_ > maxChildCoverage.area_) {
                maxChildCoverage = childCoverInfo;
            }
        } else if (!child->isValidInCurrentFrame_) {
            offTreeNodes.insert(child->id_);
        } else {
            child->isValidInCurrentFrame_ = false;
        }
        child = child->leftSibling_.lock();
    }
    CheckNodeOcclusion(maxChildCoverage, culledNodes);
    UpdateCoverageInfo(globalCoverage, selfCoverage);
    // Return the maximum coverage info of the subtree
    if (maxChildCoverage.area_ > selfCoverage.area_) {
        return maxChildCoverage;
    }
    return selfCoverage;
}

void OcclusionNode::CheckNodeOcclusion(OcclusionCoverageInfo& coverageInfo, std::unordered_set<NodeId>& culledNodes)
{
    // Currently, only canvas nodes without clipping attributes can be culled
    if (type_ == RSRenderNodeType::CANVAS_NODE && !isNeedClip_) {
        if (isOutOfRootRect_ || outerRect_.IsInsideOf(coverageInfo.rect_)) {
            culledNodes.insert(id_);
            occludedById_ = coverageInfo.id_;
        }
    }
}

void OcclusionNode::UpdateCoverageInfo(OcclusionCoverageInfo& globalCoverage, OcclusionCoverageInfo& selfCoverage)
{
    // When the node is a opaque node, update the global coverage info
    if (isOpaque_) {
        selfCoverage.area_ = innerRect_.GetWidth() * innerRect_.GetHeight();
        selfCoverage.rect_ = innerRect_;
        selfCoverage.id_ = id_;
        if (selfCoverage.area_ > globalCoverage.area_) {
            globalCoverage = selfCoverage;
        }
    }
}

// Used for debugging, comparing rs node tree and occlusion node tree
void OcclusionNode::PreorderTraversal(std::vector<std::shared_ptr<OcclusionNode>>& result)
{
    if (isSubTreeIgnored_) {
        return;
    }
    RS_TRACE_NAME_FMT("%s node id %llu", __func__, id_);
    result.push_back(shared_from_this());
    std::shared_ptr<OcclusionNode> child = firstChild_;
    while (child) {
        child->PreorderTraversal(result);
        child = child->rightSibling_.lock();
    }
}

std::string OcclusionNode::GetOcclusionNodeInfoString()
{
    std::stringstream ss;
    ss << "OcclusionNode id: " << std::to_string(id_)
       << ", occludedBy id: " << std::to_string(occludedById_)
       << ", outerRectCoords left: " << outerRect_.GetLeft()
       << ", right: " << outerRect_.GetRight()
       << ", top: " << outerRect_.GetTop()
       << ", bottom: " << outerRect_.GetBottom()
       << ", innerRectCoords left: " << innerRect_.GetLeft()
       << ", right: " << innerRect_.GetRight()
       << ", top: " << innerRect_.GetTop()
       << ", bottom: " << innerRect_.GetBottom()
       << ", subTreeIgnore: " << isSubTreeIgnored_
       << ", outOfScreen: " << isOutOfRootRect_
       << ", opaque: " << isOpaque_
       << ", alpha_: " << localAlpha_ * accumulatedAlpha_
       << ", isNeedClip_: " << isNeedClip_
       << ", hasChildrenOutOfRect_: " << hasChildrenOutOfRect_;
    return ss.str();
}
} // namespace OHOS::Rosen
