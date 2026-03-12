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

#include "pipeline/rs_context.h"
#include "platform/common/rs_system_properties.h"
#include "feature/uifirst/rs_frame_control.h"

namespace OHOS {
namespace Rosen {

RSFrameControlTool& RSFrameControlTool::Instance()
{
    static RSFrameControlTool instance;
    return instance;
}

void RSFrameControlTool::SetAppWindowNodeId(NodeId id)
{
    refreshAppWindowNodeId_ = id;
}

void RSFrameControlTool::SetNodeIdForFrameControl(RSSurfaceRenderNode& node)
{
    if (!RSSystemProperties::GetSubThreadControlFrameRate()) {
        return;
    }
    auto context = node.GetContext().lock();
    if (!context) {
        RS_LOGE("RSSurfaceRenderNode::SetClonedNodeInfo invalid context");
        return;
    }
    auto surfaceNode = context->GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(node.GetFirstLevelNodeId());
    if (surfaceNode) {
        SetAppWindowNodeId(node.GetFirstLevelNodeId());
    }
}

bool RSFrameControlTool::CheckAppWindowNodeId(NodeId id)
{
    if (refreshAppWindowNodeId_ == id) {
        return true;
    }
    return false;
}

}
}