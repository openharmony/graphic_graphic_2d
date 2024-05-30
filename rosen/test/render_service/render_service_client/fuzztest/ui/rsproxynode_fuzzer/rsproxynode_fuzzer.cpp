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

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "ui/rs_proxy_node.h"
#include "ui/rs_canvas_node.h"
#include "transaction/rs_transaction_proxy.h"

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

bool DoRSProxyNode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId targetNodeId = GetData<NodeId>();
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    RSProxyNode proxyNode(targetNodeId, "ProxyNode");
    RSCanvasNode::SharedPtr child1 = RSCanvasNode::Create();
    int index = GetData<int>();
    Vector4f bounds = {GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>()};
    float positionX = GetData<float>();
    float positionY = GetData<float>();
    float width = GetData<float>();
    float height = GetData<float>();
    Vector4f frame = {GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>()};
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
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoRSProxyNode(data, size);
    return 0;
}
