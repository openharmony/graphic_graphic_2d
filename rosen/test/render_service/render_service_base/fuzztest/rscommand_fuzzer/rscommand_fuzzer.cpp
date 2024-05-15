/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rscommand_fuzzer.h"

#include <securec.h>

#include "command/rs_base_node_command.h"
#include "command/rs_canvas_node_command.h"
#include "command/rs_message_processor.h"
#include "command/rs_proxy_node_command.h"
#include "command/rs_root_node_command.h"
#include "command/rs_surface_node_command.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool RSSurfaceNodeCommandFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    uint64_t id = GetData<uint64_t>();
    float contextAlpha = GetData<float>();
    bool isSecurityLayer = GetData<bool>();
    GraphicColorGamut colorSpace = GetData<GraphicColorGamut>();
    float width = GetData<float>();
    float height = GetData<float>();
    uint8_t alpha = GetData<uint8_t>();
    bool available = GetData<bool>();
    bool isRefresh = GetData<bool>();
    RSSurfaceNodeType type = GetData<RSSurfaceNodeType>();
    bool hasContainerWindow = GetData<bool>();
    float density = GetData<float>();
    bool flag = GetData<bool>();

    // test
    RSContext context;
    SurfaceNodeCommandHelper::Create(context, static_cast<NodeId>(id));
    SurfaceNodeCommandHelper::SetContextAlpha(context, static_cast<NodeId>(id), contextAlpha);
    SurfaceNodeCommandHelper::SetSecurityLayer(context, static_cast<NodeId>(id), isSecurityLayer);
    SurfaceNodeCommandHelper::SetColorSpace(context, static_cast<NodeId>(id), colorSpace);
    SurfaceNodeCommandHelper::UpdateSurfaceDefaultSize(context, static_cast<NodeId>(id), width, height);
    SurfaceNodeCommandHelper::ConnectToNodeInRenderService(context, static_cast<NodeId>(id));
    SurfaceNodeCommandHelper::SetCallbackForRenderThreadRefresh(context, static_cast<NodeId>(id), isRefresh);
    Vector4f bounds = {GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>()};
    SurfaceNodeCommandHelper::SetContextBounds(context, static_cast<NodeId>(id), bounds);
    SurfaceNodeCommandHelper::SetAbilityBGAlpha(context, static_cast<NodeId>(id), alpha);
    SurfaceNodeCommandHelper::SetIsNotifyUIBufferAvailable(context, static_cast<NodeId>(id), available);
    SurfaceNodeCommandHelper::SetSurfaceNodeType(context, static_cast<NodeId>(id), static_cast<uint8_t>(type));
    SurfaceNodeCommandHelper::SetContainerWindow(context, static_cast<NodeId>(id), hasContainerWindow, density);
    SurfaceNodeCommandHelper::SetForceHardwareAndFixRotation(context, static_cast<NodeId>(id), flag);

    return true;
}

bool RSBaseNodeCommandFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    uint64_t nodeId = GetData<uint64_t>();
    uint64_t childNodeId = GetData<uint64_t>();
    uint64_t newParentId = GetData<uint64_t>();
    int32_t index = GetData<int32_t>();

    // test
    RSContext context;
    BaseNodeCommandHelper::Destroy(context, static_cast<NodeId>(nodeId));
    BaseNodeCommandHelper::AddChild(context, static_cast<NodeId>(nodeId), static_cast<NodeId>(childNodeId), index);
    BaseNodeCommandHelper::MoveChild(context, static_cast<NodeId>(nodeId), static_cast<NodeId>(childNodeId), index);
    BaseNodeCommandHelper::RemoveChild(context, static_cast<NodeId>(nodeId), static_cast<NodeId>(childNodeId));
    BaseNodeCommandHelper::AddCrossParentChild(context, static_cast<NodeId>(nodeId), static_cast<NodeId>(childNodeId),
                                               index);
    BaseNodeCommandHelper::RemoveCrossParentChild(context, static_cast<NodeId>(nodeId),
                                                  static_cast<NodeId>(childNodeId),
                                                  static_cast<NodeId>(newParentId));
    BaseNodeCommandHelper::RemoveFromTree(context, static_cast<NodeId>(nodeId));
    BaseNodeCommandHelper::ClearChildren(context, static_cast<NodeId>(nodeId));

    return true;
}

bool RSRootNodeCommandFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    uint64_t id = GetData<uint64_t>();
    uint64_t surfaceNodeId = GetData<uint64_t>();
    bool flag = GetData<bool>();
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();

    // test
    RSContext context;
    RootNodeCommandHelper::Create(context, static_cast<NodeId>(id), false);
    RootNodeCommandHelper::AttachRSSurfaceNode(context, static_cast<NodeId>(id), static_cast<NodeId>(surfaceNodeId));
    RootNodeCommandHelper::AttachToUniSurfaceNode(context, static_cast<NodeId>(id), static_cast<NodeId>(surfaceNodeId));
    RootNodeCommandHelper::SetEnableRender(context, static_cast<NodeId>(id), flag);
    RootNodeCommandHelper::UpdateSuggestedBufferSize(context, static_cast<NodeId>(id), width, height);
    
    return true;
}

bool RSCanvasNodeCommandFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    uint64_t id = GetData<uint64_t>();
    RSModifierType type = GetData<RSModifierType>();

    // test
    RSContext context;
    RSCanvasNodeCommandHelper::Create(context, static_cast<NodeId>(id), false);
    std::shared_ptr<Drawing::DrawCmdList> drawCmds;
    RSCanvasNodeCommandHelper::UpdateRecording(context, static_cast<NodeId>(id), drawCmds, static_cast<uint16_t>(type));
    RSCanvasNodeCommandHelper::ClearRecording(context, static_cast<NodeId>(id));
    
    return true;
}

bool RSProxyNodeCommandFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    uint64_t id = GetData<uint64_t>();
    uint64_t target = GetData<uint64_t>();

    // test
    RSContext context;
    ProxyNodeCommandHelper::Create(context, static_cast<NodeId>(id), static_cast<NodeId>(target));
    ProxyNodeCommandHelper::ResetContextVariableCache(context, static_cast<NodeId>(id));
    
    return true;
}

bool RSMessageProcessFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    uint32_t pid = GetData<uint32_t>();

    // test
    std::unique_ptr<RSCommand> command;
    RSMessageProcessor::Instance().AddUIMessage(pid, command);
    RSMessageProcessor::Instance().AddUIMessage(pid, command);
    RSMessageProcessor::Instance().HasTransaction(pid);
    RSMessageProcessor::Instance().GetTransaction(pid);
    
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSSurfaceNodeCommandFuzzTest(data, size);
    OHOS::Rosen::RSBaseNodeCommandFuzzTest(data, size);
    OHOS::Rosen::RSRootNodeCommandFuzzTest(data, size);
    OHOS::Rosen::RSCanvasNodeCommandFuzzTest(data, size);
    OHOS::Rosen::RSProxyNodeCommandFuzzTest(data, size);
    OHOS::Rosen::RSMessageProcessFuzzTest(data, size);
    return 0;
}
