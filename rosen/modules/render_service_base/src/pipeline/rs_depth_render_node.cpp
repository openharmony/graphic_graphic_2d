/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "pipeline/rs_depth_render_node.h"

#include "memory/rs_memory_track.h"
#include "modifier_ng/appearance/rs_depth_space_render_modifier.h"
#include "params/rs_depth_render_params.h"
#include "platform/common/rs_log.h"
#include "property/rs_spatial_effect_manager.h"
#include "render/rs_pixel_map_util.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
RSDepthRenderNode::RSDepthRenderNode(NodeId id, const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
    : RSRenderNode(id, context, isTextureExportNode)
{
#ifndef ROSEN_ARKUI_X
    MemoryInfo info = { sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE };
    MemoryTrack::Instance().AddNodeRecord(id, info);
    MemoryTrack::Instance().RegisterNodeMem(ExtractPid(GetId()), sizeof(*this), MEMORY_TYPE::MEM_RENDER_NODE);
#endif
    MemorySnapshot::Instance().AddCpuMemory(ExtractPid(id), sizeof(*this));
}

RSDepthRenderNode::~RSDepthRenderNode()
{
#ifndef ROSEN_ARKUI_X
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
    MemoryTrack::Instance().UnRegisterNodeMem(ExtractPid(GetId()), sizeof(*this), MEMORY_TYPE::MEM_RENDER_NODE);
#endif
    MemorySnapshot::Instance().RemoveCpuMemory(ExtractPid(GetId()), sizeof(*this));
}

void RSDepthRenderNode::SetDepthSpaceType(DepthSpaceType depthSpaceType)
{
    RS_LOGD("RSDepthRenderNode::SetDepthSpaceType type=%{public}d", static_cast<int32_t>(depthSpaceType));
    depthSpaceType_ = depthSpaceType;
    RSSpatialEffectManager::Instance()->RegisterDepthSpace(shared_from_this());
}

DepthSpaceType RSDepthRenderNode::GetDepthSpaceType() const
{
    return depthSpaceType_;
}

void RSDepthRenderNode::InitRenderParams()
{
    stagingRenderParams_ = std::make_unique<RSDepthRenderParams>(GetId());
    DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(shared_from_this());
    if (renderDrawable_ == nullptr) {
        HILOG_COMM_ERROR("RSDepthRenderNode::InitRenderParams failed");
        return;
    }
}

void RSDepthRenderNode::UpdateRenderParams()
{
    auto depthParams = static_cast<RSDepthRenderParams*>(stagingRenderParams_.get());
    if (depthParams == nullptr) {
        RS_LOGE("RSDepthRenderNode::UpdateRenderParams depthParams is null");
        return;
    }

    auto& properties = GetRenderProperties();
    auto depthImage = properties.GetDepthImage();
    depthParams->SetDepthImage(depthImage ? RSPixelMapUtil::ExtractDrawingImage(depthImage->GetPixelMap()) : nullptr);
    depthParams->ResetDepthSrcSurfaceDrawable();
    depthParams->SetDepthCameraPara(properties.GetDepthCameraPara());
    depthParams->SetDepthLightPara(properties.GetDepthLightPara());
    depthParams->SetImageMatrix(properties.GetDepthImageMatrix().has_value() ?
        properties.GetDepthImageMatrix().value() : Matrix3f::IDENTITY);

    RSRenderNode::UpdateRenderParams();
}

void RSDepthRenderNode::QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor,
    bool isParentPrepareInReverseOrder)
{
    if (!visitor) {
        return;
    }
    ApplyModifiers();
    visitor->QuickPrepareDepthRenderNode(*this, isParentPrepareInReverseOrder);
}

void RSDepthRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    return;
}

void RSDepthRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    return;
}

void RSDepthRenderNode::ApplyDepthSpaceModifier()
{
    constexpr auto depthSpaceModifierTypeNG = static_cast<uint16_t>(ModifierNG::RSModifierType::DEPTH_SPACE);
    if (!dirtyTypesNG_.test(depthSpaceModifierTypeNG)) {
        return;
    }
    auto depthSpaceModifiers = GetModifiersNG(ModifierNG::RSModifierType::DEPTH_SPACE);
    if (depthSpaceModifiers.empty()) {
        return;
    }
    ModifierNG::RSDepthSpaceRenderModifier::ResetProperties(GetMutableRenderProperties());
    for (auto& modifier : depthSpaceModifiers) {
        modifier->ApplyLegacyProperty(GetMutableRenderProperties());
    }
    dirtyTypesNG_.reset(depthSpaceModifierTypeNG);
}
} // namespace Rosen
} // namespace OHOS