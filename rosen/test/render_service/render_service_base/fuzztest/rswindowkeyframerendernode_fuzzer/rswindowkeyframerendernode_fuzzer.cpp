/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "rswindowkeyframerendernode_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <memory>
#include <securec.h>
#include <unistd.h>
#include "params/rs_render_params.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "render_thread/rs_render_thread_visitor.h"
#include "feature/window_keyframe/rs_window_keyframe_render_node.h"
namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;
    NodeId nodeId = GetData<NodeId>();
    auto behindWndRect = GetData<RectI>();
    std::shared_ptr<RSRenderThreadVisitor>rsRenderThreadVisitor;
    auto keyframeNode = std::make_shared<RSWindowKeyFrameRenderNode>(nodeId);
    keyframeNode->QuickPrepare(rsRenderThreadVisitor);
    keyframeNode->SetLinkedNodeId(nodeId);
    keyframeNode->ResetLinkedWindowKeyFrameInfo(*keyframeNode);

    RSSurfaceRenderNode surfaceNode{nodeId};
    auto rsContext = std::make_shared<RSContext>();
    NodeId appNodeId = GetData<NodeId>();
    constexpr static NodeId DEFAULT_KEYFRAMENODE_ID = 1;
    constexpr static NodeId DEFAULT_LINKEDNODE_ID = DEFAULT_KEYFRAMENODE_ID + 1;
    auto rootNode = std::make_shared<RSRootRenderNode>(DEFAULT_LINKEDNODE_ID, rsContext->weak_from_this());
    auto appNode = std::make_shared<RSSurfaceRenderNode>(appNodeId, rsContext->weak_from_this());
    appNode->InitRenderParams();
    keyframeNode->SetLinkedNodeId(DEFAULT_LINKEDNODE_ID);
    keyframeNode->CollectLinkedNodeInfo();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.RegisterRenderNode(rootNode);
    nodeMap.RegisterRenderNode(keyframeNode);
    rootNode->parent_ = appNode;
    rootNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(DEFAULT_LINKEDNODE_ID);
    keyframeNode->addedToPendingSyncList_ = true;
    rootNode->addedToPendingSyncList_ = true;
    keyframeNode->PrepareLinkedNodeOffscreen(surfaceNode, *rsContext);

    rootNode->GetMutableRenderProperties().SetBoundsWidth(GetData<float>());
    rootNode->GetMutableRenderProperties().SetBoundsHeight(GetData<float>());
    keyframeNode->GetMutableRenderProperties().SetBoundsWidth(GetData<float>());
    keyframeNode->GetMutableRenderProperties().SetBoundsHeight(GetData<float>());
    keyframeNode->UpdateKeyFrameBehindWindowRegion(surfaceNode, *rsContext, behindWndRect);

    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
