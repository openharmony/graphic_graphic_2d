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

#include "rsproxynode_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "ui/rs_proxy_node.h"
#include "ui/rs_canvas_node.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {

void DoRSProxyNode(FuzzedDataProvider& fdp)
{
    NodeId targetNodeId = fdp.ConsumeIntegral<NodeId>();
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    RSProxyNode proxyNode(targetNodeId, "ProxyNode");
    RSCanvasNode::SharedPtr child1 = RSCanvasNode::Create();
    int index = fdp.ConsumeIntegral<int>();
    Vector4f bounds = {fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
    float positionX = fdp.ConsumeFloatingPoint<float>();
    float positionY = fdp.ConsumeFloatingPoint<float>();
    float width = fdp.ConsumeFloatingPoint<float>();
    float height = fdp.ConsumeFloatingPoint<float>();
    Vector4f frame = {fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
    RSProxyNode::Create(targetNodeId);
    proxyNode.GetType();
    proxyNode.GetName();
    proxyNode.ResetContextVariableCache();
    proxyNode.AddChild(child1, index);
    proxyNode.RemoveChild(child1);
    proxyNode.ClearChildren();
    proxyNode.SetBounds(bounds);
    proxyNode.SetBounds(positionX, positionY, width, height);
    proxyNode.SetBoundsWidth(width);
    proxyNode.SetBoundsHeight(height);
    proxyNode.SetFrame(frame);
    proxyNode.SetFrame(positionX, positionY, width, height);
    proxyNode.SetFramePositionX(positionX);
    proxyNode.SetFramePositionY(positionY);
    proxyNode.GetHierarchyCommandNodeId();
    RSTransactionProxy::instance_ = new RSTransactionProxy();
}

} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);
    OHOS::Rosen::DoRSProxyNode(fdp);
    return 0;
}
