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

#include "rs_protective_solid_render_node.h"
#include "common/rs_singleton.h"
#include "memory/rs_memory_graphic.h"
#include "memory/rs_memory_track.h"
#include "platform/common/rs_log.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {

RSProtectiveSolidRenderNode::RSProtectiveSolidRenderNode(NodeId id, const std::weak_ptr<RSContext>& context)
    : RSSurfaceRenderNode(id, context)
{
    name_ = "protectiveSolidNode";
    RS_LOGD("RSProtectiveSolidRenderNode: Create RSProtectiveSolidRenderNode");
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info);
    MemorySnapshot::Instance().AddCpuMemory(ExtractPid(id), sizeof(*this));
}

RSProtectiveSolidRenderNode::~RSProtectiveSolidRenderNode()
{
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
    MemorySnapshot::Instance().RemoveCpuMemory(ExtractPid(GetId()), sizeof(*this));
}

void RSProtectiveSolidRenderNode::QuickPrepare(const std::shared_ptr<RSNodeVisitor>& visitor,
    bool isParentPrepareInReverseOrder)
{
    if (!visitor) {
        return;
    }
    visitor->QuickPrepareProtectiveSolidRenderNode(*this, isParentPrepareInReverseOrder);
}
    
void RSProtectiveSolidRenderNode::UpdateProtectiveSolidLayerInfo(GraphicTransformType transform)
{
    const auto& properties = GetRenderProperties();
    int32_t boundsX = static_cast<int32_t>(properties.GetBoundsPositionX());
    int32_t boundsY = static_cast<int32_t>(properties.GetBoundsPositionY());
    int32_t boundsWidth = static_cast<int32_t>(properties.GetBoundsWidth());
    int32_t boundsHeight = static_cast<int32_t>(properties.GetBoundsHeight());
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(stagingRenderParams_.get());
    if (!surfaceParams) {
        return;
    }
    auto layer = surfaceParams->GetLayerInfo();
    layer.boundRect = {boundsX, boundsY, boundsWidth, boundsHeight};
    layer.dstRect = {boundsX, boundsY, boundsWidth, boundsHeight};
    layer.srcRect = {boundsX, boundsY, boundsWidth, boundsHeight};
    layer.transformType = transform;
    layer.alpha = GetGlobalAlpha();
    layer.layerType = GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC;
    surfaceParams->SetLayerInfo(layer);
    RS_TRACE_NAME_FMT("RSProtectiveSolidRenderNode::UpdateProtectiveSolidLayerInfo nodeId[%" PRIu64 "], nodeName[%s],"
        " bounds:[%d,%d,%d,%d], alpha[%.2f]", GetId(), GetName().c_str(), boundsX, boundsY, boundsWidth, boundsHeight,
        layer.alpha);
}
} // namespace Rosen
} // namespace OHOS