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

#include "rsnodecommand_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <fuzzer/FuzzedDataProvider.h>
#include <securec.h>

#include "command/rs_node_command.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_base_render_node.h"

namespace OHOS {
namespace Rosen {
bool DoNodeCommand(FuzzedDataProvider& fdp)
{
    RSContext context;
    NodeId nodeId = fdp.ConsumeIntegral<NodeId>();
    bool isFreeze = fdp.ConsumeBool();
    bool isMarkedByUi = fdp.ConsumeBool();
    bool isNodeGroup = fdp.ConsumeBool();
    bool isForced = fdp.ConsumeBool();
    bool includeProperty = fdp.ConsumeBool();
    bool isExcluded = fdp.ConsumeBool();
    bool isRepaintBoundary = fdp.ConsumeBool();
    bool isUifirstNode = fdp.ConsumeBool();
    pid_t pid = fdp.ConsumeIntegral<pid_t>();
    float brightness = fdp.ConsumeFloatingPoint<float>();
    int8_t colorSpace = fdp.ConsumeIntegral<int8_t>();
    bool isLayer = fdp.ConsumeBool();
    bool enable = fdp.ConsumeBool();
    NodeId keyOcclusionNodeId = fdp.ConsumeIntegral<NodeId>();
    uint64_t token = fdp.ConsumeIntegral<uint64_t>();
    DrawNodeType nodeType = static_cast<DrawNodeType>(fdp.ConsumeIntegral<uint32_t>());

    auto node = std::make_shared<RSBaseRenderNode>(nodeId);
    context.GetMutableNodeMap().RegisterRenderNode(node);

    RSNodeCommandHelper::SetFreeze(context, nodeId, isFreeze, isMarkedByUi);
    RSNodeCommandHelper::MarkNodeGroup(context, nodeId, isNodeGroup, isForced, includeProperty);
    RSNodeCommandHelper::ExcludedFromNodeGroup(context, nodeId, isExcluded);
    RSNodeCommandHelper::MarkRepaintBoundary(context, nodeId, isRepaintBoundary);
    RSNodeCommandHelper::MarkNodeSingleFrameComposer(context, nodeId, false, pid);
    RSNodeCommandHelper::MarkSuggestOpincNode(context, nodeId, false, false);
    RSNodeCommandHelper::MarkLayerPartRender(context, nodeId, false);
    RSNodeCommandHelper::MarkUifirstNode(context, nodeId, isUifirstNode);
    RSNodeCommandHelper::SetHDRUIBrightness(context, nodeId, brightness);
    RSNodeCommandHelper::MarkNodeColorSpace(context, nodeId, colorSpace);
    RSNodeCommandHelper::MarkLayer(context, nodeId, isLayer);
    RSNodeCommandHelper::SetDrawNodeType(context, nodeId, nodeType);
    RSNodeCommandHelper::UpdateOcclusionCullingStatus(context, nodeId, enable, keyOcclusionNodeId);
    RSNodeCommandHelper::SetUIToken(context, nodeId, token);
    RSNodeCommandHelper::ColorPickerDestroyInRender(
        context, nodeId, pid, token, fdp.ConsumeIntegral<uint8_t>());
    RSNodeCommandHelper::ReSortChildrenByZIndex(context, nodeId);
    return true;
}

bool DoNodeCommandSetProperty(FuzzedDataProvider& fdp)
{
    RSContext context;
    NodeId nodeId = fdp.ConsumeIntegral<NodeId>();
    bool value = fdp.ConsumeBool();
    PropertyId propertyId = fdp.ConsumeIntegral<PropertyId>();
    PropertyUpdateType type = static_cast<PropertyUpdateType>(fdp.ConsumeIntegral<uint8_t>());

    auto node = std::make_shared<RSBaseRenderNode>(nodeId);
    context.GetMutableNodeMap().RegisterRenderNode(node);

    RSNodeCommandHelper::UpdateProperty<bool>(context, nodeId, value, propertyId, type);
    RSNodeCommandHelper::UpdateProperty<float>(context, nodeId, fdp.ConsumeFloatingPoint<float>(), propertyId, type);
    RSNodeCommandHelper::UpdateProperty<int>(context, nodeId, fdp.ConsumeIntegral<int>(), propertyId, type);
    return true;
}
} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    OHOS::Rosen::DoNodeCommand(fdp);
    OHOS::Rosen::DoNodeCommandSetProperty(fdp);
    return 0;
}
