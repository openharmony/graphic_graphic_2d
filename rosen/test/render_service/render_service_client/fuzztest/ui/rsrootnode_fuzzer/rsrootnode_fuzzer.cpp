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

#include "rsrootnode_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"

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

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    return true;
}

bool DoCreate(const uint8_t* data, size_t size)
{
    // test
    bool isRenderServiceNode = GetData<bool>();
    RSRootNode::Create(isRenderServiceNode);
    return true;
}

bool DoGetType(const uint8_t* data, size_t size)
{
    // test
    bool isRenderServiceNode = GetData<bool>();
    auto node = std::make_shared<RSRootNode>(isRenderServiceNode);
    node->GetType();
    return true;
}

bool DoAttachRSSurfaceNode(const uint8_t* data, size_t size)
{
    // test
    bool isRenderServiceNode = GetData<bool>();
    auto node = std::make_shared<RSRootNode>(isRenderServiceNode);
    Rosen::RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    node->AttachRSSurfaceNode(surfaceNode);
    return true;
}

bool DoSetEnableRender(const uint8_t* data, size_t size)
{
    // test
    bool isRenderServiceNode = GetData<bool>();
    auto node = std::make_shared<RSRootNode>(isRenderServiceNode);
    bool flag = GetData<bool>();
    node->SetEnableRender(flag);
    return true;
}

bool DoOnBoundsSizeChanged(const uint8_t* data, size_t size)
{
    // test
    bool isRenderServiceNode = GetData<bool>();
    auto node = std::make_shared<RSRootNode>(isRenderServiceNode);
    node->OnBoundsSizeChanged();
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    
    /* Run your code on data */
    OHOS::Rosen::DoCreate(data, size);
    OHOS::Rosen::DoGetType(data, size);
    OHOS::Rosen::DoAttachRSSurfaceNode(data, size);
    OHOS::Rosen::DoSetEnableRender(data, size);
    OHOS::Rosen::DoOnBoundsSizeChanged(data, size);
    return 0;
}

