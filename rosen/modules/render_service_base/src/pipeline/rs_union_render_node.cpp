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

#include "pipeline/rs_union_render_node.h"

#include "common/rs_optional_trace.h"
#include "memory/rs_memory_track.h"
#include "params/rs_render_params.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr float SCALE_EPSILON = 0.0001f;
} // namespace

RSUnionRenderNode::RSUnionRenderNode(NodeId id, const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
    : RSCanvasRenderNode(id, context, isTextureExportNode)
{
#ifndef ROSEN_ARKUI_X
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info);
    MemoryTrack::Instance().RegisterNodeMem(ExtractPid(id),
        sizeof(*this), MEMORY_TYPE::MEM_RENDER_NODE);
#endif
    MemorySnapshot::Instance().AddCpuMemory(ExtractPid(id), sizeof(*this));
}

RSUnionRenderNode::~RSUnionRenderNode()
{
#ifndef ROSEN_ARKUI_X
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
    MemoryTrack::Instance().UnRegisterNodeMem(ExtractPid(GetId()),
        sizeof(*this), MEMORY_TYPE::MEM_RENDER_NODE);
#endif
    MemorySnapshot::Instance().RemoveCpuMemory(ExtractPid(GetId()), sizeof(*this));
}

void RSUnionRenderNode::QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    ApplyModifiers();
    visitor->QuickPrepareUnionRenderNode(*this);
}

void RSUnionRenderNode::AddUnionChild(NodeId id)
{
    unionChildren_.emplace(id);
}

void RSUnionRenderNode::RemoveUnionChild(NodeId id)
{
    unionChildren_.erase(id);
}

void RSUnionRenderNode::ResetUnionChildren()
{
    unionChildren_.clear();
}

void RSUnionRenderNode::ProcessSDFShape(RSDirtyRegionManager& dirtyManager)
{
    unionMode_ = GetRenderProperties().GetSDFUnionMode();
    RS_TRACE_NAME_FMT("RSUnionRenderNode::ProcessSDFShape, unionChildren_[%lu], UnionSpacing[%f] unimode[%d]",
        unionChildren_.size(), GetRenderProperties().GetUnionSpacing(), unionMode_);
    std::shared_ptr<RSNGRenderShapeBase> root;
    if (unionChildren_.empty()) {
        if (GetRenderProperties().GetSDFShape() &&
            GetRenderProperties().GetSDFShape()->GetType() == RSNGEffectType::SDF_EMPTY_SHAPE) {
            return;
        }
        root = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_EMPTY_SHAPE);
    } else {
        std::queue<std::shared_ptr<RSNGRenderShapeBase>> shapeQueue;
        if (ROSEN_NE(GetRenderProperties().GetUnionSpacing(), 0.f) && unionMode_ == 1) {
            gravityCenter_ =  GetGravityCenter();
            root = GenerateSDFNonLeaf<RSNGEffectType::SDF_UNION_OP_SHAPE, RSNGRenderSDFUnionOpShape,
                SDFUnionOpShapeShapeXRenderTag, SDFUnionOpShapeShapeYRenderTag>(shapeQueue);
            GenerateSDFLeaf<RSNGRenderSDFUnionOpShape, SDFUnionOpShapeShapeXRenderTag,
                SDFUnionOpShapeShapeYRenderTag>(shapeQueue);
        } else if (ROSEN_NE(GetRenderProperties().GetUnionSpacing(), 0.f) && unionMode_ == 0) {
            root = GenerateSDFNonLeaf<RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE, RSNGRenderSDFSmoothUnionOpShape,
                SDFSmoothUnionOpShapeShapeXRenderTag, SDFSmoothUnionOpShapeShapeYRenderTag>(shapeQueue);
            GenerateSDFLeaf<RSNGRenderSDFSmoothUnionOpShape, SDFSmoothUnionOpShapeShapeXRenderTag,
                SDFSmoothUnionOpShapeShapeYRenderTag>(shapeQueue);
        } else {
            root = GenerateSDFNonLeaf<RSNGEffectType::SDF_UNION_OP_SHAPE, RSNGRenderSDFUnionOpShape,
                SDFUnionOpShapeShapeXRenderTag, SDFUnionOpShapeShapeYRenderTag>(shapeQueue);
            GenerateSDFLeaf<RSNGRenderSDFUnionOpShape, SDFUnionOpShapeShapeXRenderTag,
                SDFUnionOpShapeShapeYRenderTag>(shapeQueue);
        }
    }
    GetMutableRenderProperties().InternalSetSDFShape(root);
    dirtyManager.MergeDirtyRect(GetAbsDrawRect());
    UpdateDrawableAfterPostPrepare(ModifierNG::RSModifierType::BOUNDS);
}

bool RSUnionRenderNode::GetChildRelativeMatrixToUnionNode(Drawing::Matrix& relativeMatrix,
    std::shared_ptr<RSRenderNode>& child) const
{
    auto geoPtr = GetRenderProperties().GetBoundsGeometry();
    if (!geoPtr) {
        RS_LOGE("RSUnionRenderNode::GetChildRelativeMatrixToUnionNode GetBoundsGeometry fail");
        return false;
    }
    auto unionAbsMatrix = geoPtr->GetAbsMatrix();
    Drawing::Matrix invertUnionAbsMatrix;
    if (!unionAbsMatrix.Invert(invertUnionAbsMatrix)) {
        RS_LOGE("RSUnionRenderNode::GetChildRelativeMatrixToUnionNode unionAbsMatrix Invert fail");
        return false;
    }
    if (!child->GetAbsMatrixReverse(*this, relativeMatrix)) {
        RS_LOGE("RSUnionRenderNode::GetChildRelativeMatrixToUnionNode get child absMatrix fail");
        return false;
    }
    relativeMatrix.PostConcat(invertUnionAbsMatrix);
    return true;
}

std::shared_ptr<RSNGRenderShapeBase> RSUnionRenderNode::CreateSDFOpShapeWithBaseInitialization(RSNGEffectType type)
{
    auto opShape = RSNGRenderShapeBase::Create(type);
    if (!opShape) {
        return nullptr;
    }
    switch (opShape->GetType()) {
        case RSNGEffectType::SDF_UNION_OP_SHAPE: {
            break;
        }
        case RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE: {
            std::static_pointer_cast<RSNGRenderSDFSmoothUnionOpShape>(opShape)->
                Setter<SDFSmoothUnionOpShapeSpacingRenderTag>(GetRenderProperties().GetUnionSpacing());
            break;
        }
        default: {
            RS_LOGE("RSUnionRenderNode::CreateSDFOpShapeWithBaseInitialization childGeoPtr no matching type found");
            opShape = nullptr;
            break;
        }
    }
    return opShape;
}

std::shared_ptr<RSNGRenderShapeBase> RSUnionRenderNode::CreateChildToContainerSDFTransformShape(
    std::shared_ptr<RSRenderNode>& child, std::shared_ptr<RSNGRenderShapeBase>& childShape)
{
    Drawing::Matrix relativeMatrix;
    if (!GetChildRelativeMatrixToUnionNode(relativeMatrix, child)) {
        return childShape;
    }
    Matrix3f matrix(relativeMatrix.Get(Drawing::Matrix::Index::SCALE_X),
        relativeMatrix.Get(Drawing::Matrix::Index::SKEW_X), relativeMatrix.Get(Drawing::Matrix::Index::TRANS_X),
        relativeMatrix.Get(Drawing::Matrix::Index::SKEW_Y), relativeMatrix.Get(Drawing::Matrix::Index::SCALE_Y),
        relativeMatrix.Get(Drawing::Matrix::Index::TRANS_Y), relativeMatrix.Get(Drawing::Matrix::Index::PERSP_0),
        relativeMatrix.Get(Drawing::Matrix::Index::PERSP_1), relativeMatrix.Get(Drawing::Matrix::Index::PERSP_2));
    auto transformShape = std::static_pointer_cast<RSNGRenderSDFTransformShape>(
        RSNGRenderShapeBase::Create(RSNGEffectType::SDF_TRANSFORM_SHAPE));
    transformShape->Setter<SDFTransformShapeMatrixRenderTag>(matrix);
    transformShape->Setter<SDFTransformShapeShapeRenderTag>(childShape);

    transformShape->Setter<SDFTransformShapeUnionModeRenderTag>(0);
    if (ROSEN_NE(GetRenderProperties().GetUnionSpacing(), 0.f) && unionMode_ == 1) {
        transformShape->Setter<SDFTransformShapeUnionModeRenderTag>(1);
        transformShape->Setter<SDFTransformShapeGravityCenterRenderTag>(gravityCenter_);
        transformShape->Setter<SDFTransformShapeGravityStrengthRenderTag>(
            GetRenderProperties().GetGravityPullStrength());
        transformShape->Setter<SDFTransformShapeGravitySpacingRenderTag>(GetRenderProperties().GetUnionSpacing());
    }
    return transformShape;
}

Vector2f RSUnionRenderNode::GetGravityCenter() const
{
    auto context = GetContext().lock();
    if (!context) {
        RS_LOGE("RSUnionRenderNode::GetGravityCenter GetContext fail");
        return Vector2f(0.0f, 0.0f);
    }
    for (auto& childId : unionChildren_) {
        auto child = context->GetNodeMap().GetRenderNode<RSRenderNode>(childId);
        if (!child || !child->GetRenderProperties().GetGravityPullCenterFlag()) {
            continue;
        }
        Drawing::Matrix relativeMatrix;
        if (!GetChildRelativeMatrixToUnionNode(relativeMatrix, child)) {
            return Vector2f(0.0f, 0.0f);
        }
        Matrix3f matrix(relativeMatrix.Get(Drawing::Matrix::Index::SCALE_X),
            relativeMatrix.Get(Drawing::Matrix::Index::SKEW_X), relativeMatrix.Get(Drawing::Matrix::Index::TRANS_X),
            relativeMatrix.Get(Drawing::Matrix::Index::SKEW_Y), relativeMatrix.Get(Drawing::Matrix::Index::SCALE_Y),
            relativeMatrix.Get(Drawing::Matrix::Index::TRANS_Y), relativeMatrix.Get(Drawing::Matrix::Index::PERSP_0),
            relativeMatrix.Get(Drawing::Matrix::Index::PERSP_1), relativeMatrix.Get(Drawing::Matrix::Index::PERSP_2));
        auto sdfRect = child->GetRenderProperties().GetRRectForSDF().rect_;
        auto center = Vector3f(sdfRect.GetLeft() + sdfRect.GetWidth() / 2.0f,
            sdfRect.GetTop() + sdfRect.GetHeight() / 2.0f, 1.0f);
        auto result = matrix.Transpose() *  center;
        if (std::abs(result.z_) < SCALE_EPSILON) {
            return Vector2f(0.0f, 0.0f); // scaling too large
        }
        return Vector2f(result.x_ / result.z_, result.y_ / result.z_);
    }
    return Vector2f(0.0f, 0.0f);
}

std::shared_ptr<RSNGRenderShapeBase> RSUnionRenderNode::GetOrCreateChildSDFShape(std::shared_ptr<RSRenderNode>& child)
{
    if (auto shape = child->GetRenderProperties().GetSDFShape()) {
        return shape;
    }
    auto sdfRRect = child->GetRenderProperties().GetRRectForSDF();
    auto childShape = std::static_pointer_cast<RSNGRenderSDFRRectShape>(
        RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE));
    childShape->Setter<SDFRRectShapeRRectRenderTag>(sdfRRect);
    return childShape;
}

void RSUnionRenderNode::ProcessUnionInfoOnTreeStateChanged(const std::shared_ptr<RSRenderNode> node)
{
    if (!node->GetRenderProperties().GetUseUnion()) {
        return;
    }
    auto unionNode = FindClosestUnionAncestor(node);
    if (!unionNode) {
        ROSEN_LOGD("RSUnionRenderNode::ProcessUnionInfoOnTreeStateChanged: invalid unionNode");
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSUnionRenderNode::ProcessUnionInfoOnTreeStateChanged node[%llu], unionNode[%llu], "
        "isOnTheTree[%d]", node->GetId(), unionNode->GetId(), node->IsOnTheTree());
    node->IsOnTheTree() ? unionNode->AddUnionChild(node->GetId()) : unionNode->RemoveUnionChild(node->GetId());
}

void RSUnionRenderNode::ProcessUnionInfoAfterApplyModifiers(const std::shared_ptr<RSRenderNode> node)
{
    if (!node->IsOnTheTree()) {
        return;
    }
    auto unionNode = FindClosestUnionAncestor(node);
    if (!unionNode) {
        ROSEN_LOGD("RSUnionRenderNode::ProcessUnionInfoAfterApplyModifiers: invalid unionNode");
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSUnionRenderNode::ProcessUnionInfoAfterApplyModifiers node[%llu], unionNode[%llu], "
        "useUnion[%d]", node->GetId(), unionNode->GetId(), node->GetRenderProperties().GetUseUnion());
    node->GetRenderProperties().GetUseUnion() ?
        unionNode->AddUnionChild(node->GetId()) : unionNode->RemoveUnionChild(node->GetId());
}

std::shared_ptr<RSUnionRenderNode> RSUnionRenderNode::FindClosestUnionAncestor(
    const std::shared_ptr<RSRenderNode> node)
{
    std::shared_ptr<RSRenderNode> curNode = node;
    while (auto parent = curNode->GetParent().lock()) {
        if (parent->IsInstanceOf<RSUnionRenderNode>()) {
            return std::static_pointer_cast<RSUnionRenderNode>(parent);
        }
        curNode = parent;
    }
    return nullptr;
}
} // namespace Rosen
} // namespace OHOS
