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

void RSUnionRenderNode::UpdateVisibleUnionChildren(RSRenderNode& childNode)
{
    if (childNode.GetRenderProperties().GetUseUnion() && !childNode.GetOldDirtyInSurface().IsEmpty()) {
        visibleUnionChildren_.emplace(childNode.GetId());
    }
}

void RSUnionRenderNode::ResetVisibleUnionChildren()
{
    visibleUnionChildren_.clear();
}

void RSUnionRenderNode::ProcessSDFShape()
{
    RS_TRACE_NAME_FMT("RSUnionRenderNode::ProcessSDFShape, visibleUnionChildren_[%lu], UnionSpacing[%f]",
        visibleUnionChildren_.size(), GetRenderProperties().GetUnionSpacing());
    if (visibleUnionChildren_.empty()) {
        return;
    }
    std::queue<std::shared_ptr<RSNGRenderShapeBase>> shapeQueue;
    std::shared_ptr<RSNGRenderShapeBase> root;
    if (ROSEN_NE(GetRenderProperties().GetUnionSpacing(), 0.f)) {
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
    GetMutableRenderProperties().SetSDFShape(root);
    GetMutableRenderProperties().OnSDFShapeChange();
    if (GetRenderProperties().GetForegroundFilterCache()) {
        SetDrawingCacheType(RSDrawingCacheType::FOREGROUND_FILTER_CACHE);
        stagingRenderParams_->SetDrawingCacheType(GetDrawingCacheType());
        stagingRenderParams_->SetDrawingCacheIncludeProperty(true);
        stagingRenderParams_->SetForegroundFilterCache(GetRenderProperties().GetForegroundFilterCache());
    }
    UpdateDrawableAfterPostPrepare(ModifierNG::RSModifierType::BOUNDS);
}

bool RSUnionRenderNode::GetChildRelativeMatrixToUnionNode(Drawing::Matrix& relativeMatrix,
    std::shared_ptr<RSRenderNode>& child)
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
    auto childGeoPtr = child->GetRenderProperties().GetBoundsGeometry();
    if (!childGeoPtr) {
        RS_LOGE("RSUnionRenderNode::GetChildRelativeMatrixToUnionNode childGeoPtr GetBoundsGeometry fail");
        return false;
    }
    relativeMatrix = childGeoPtr->GetAbsMatrix();
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

std::shared_ptr<RSNGRenderShapeBase> RSUnionRenderNode::GetOrCreateChildSDFShape(Drawing::Matrix& relativeMatrix,
    std::shared_ptr<RSRenderNode>& child)
{
    if (child->GetRenderProperties().GetSDFShape()) {
        return child->GetRenderProperties().GetSDFShape();
    }
    auto geoPtr = GetRenderProperties().GetBoundsGeometry();
    if (!geoPtr) {
        RS_LOGE("RSUnionRenderNode::GetOrCreateChildSDFShape GetBoundsGeometry fail");
        return nullptr;
    }
    RRect sdfRRect{};
    if (child->GetRenderProperties().GetClipToRRect()) {
        auto childRRect = child->GetRenderProperties().GetClipRRect();
        sdfRRect = RRect(childRRect.rect_, childRRect.radius_[0].x_, childRRect.radius_[0].y_);
    } else if (!child->GetRenderProperties().GetCornerRadius().IsZero()) {
        auto childRRect = child->GetRenderProperties().GetRRect();
        sdfRRect = RRect(childRRect.rect_, childRRect.radius_[0].x_, childRRect.radius_[0].y_);
    } else {
        sdfRRect.rect_ = child->GetRenderProperties().GetBoundsRect();
    }
    sdfRRect.rect_ = geoPtr->MapRectWithoutRounding(sdfRRect.rect_, relativeMatrix);
    auto childShape = std::static_pointer_cast<RSNGRenderSDFRRectShape>(
        RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE));
    childShape->Setter<SDFRRectShapeRRectRenderTag>(sdfRRect);
    return childShape;
}

} // namespace Rosen
} // namespace OHOS
