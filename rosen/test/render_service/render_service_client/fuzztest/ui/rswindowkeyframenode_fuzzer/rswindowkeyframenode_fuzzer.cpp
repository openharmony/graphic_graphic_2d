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

#include "rswindowkeyframenode_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "feature/window_keyframe/rs_window_keyframe_node.h"
#include "ui/rs_ui_context.h"
#include "pipeline/rs_render_node_gc.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(DATA) which size is according to sizeof(T)
 * tips: only support basic type
 */
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

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;
    return true;
}

bool DoGetType(const uint8_t* data, size_t size)
{
    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    RSWindowKeyFrameNode keyFrameNode(isRenderServiceNode, isTextureExportNode);
    keyFrameNode.GetType();
    return true;
}

bool DoCreate(const uint8_t* data, size_t size, std::shared_ptr<RSUIContext>& rsUIContext)
{
    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    RSWindowKeyFrameNode::SharedPtr keyframeNode = RSWindowKeyFrameNode::Create(isRenderServiceNode,
        isTextureExportNode, rsUIContext);
    return true;
}

bool DoSetFreeze(const uint8_t* data, size_t size)
{
    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    RSWindowKeyFrameNode keyFrameNode(isRenderServiceNode, isTextureExportNode);
    keyFrameNode.SetFreeze(GetData<bool>());
    return true;
}

bool DoSetLinkedNodeId(const uint8_t* data, size_t size)
{
    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    NodeId id = GetData<NodeId>();
    RSWindowKeyFrameNode keyFrameNode(isRenderServiceNode, isTextureExportNode);
    keyFrameNode.SetLinkedNodeId(id);
    return true;
}

bool DoGetLinkedNodeId(const uint8_t* data, size_t size)
{
    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    RSWindowKeyFrameNode keyFrameNode(isRenderServiceNode, isTextureExportNode);
    keyFrameNode.GetLinkedNodeId();
    return true;
}

bool DoReadFromParcelAndWriteToParcel(const uint8_t* data, size_t size)
{
    // test
    Parcel parcel1;
    Parcel parcel2;
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    auto keyframeNode = RSWindowKeyFrameNode(isRenderServiceNode, isTextureExportNode);
    keyframeNode.WriteToParcel(parcel1);
    keyframeNode.ReadFromParcel(parcel2);
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
    OHOS::Rosen::DoGetType(data, size);
    auto rsUiCtx = std::make_shared<OHOS::Rosen::RSUIContext>();
    OHOS::Rosen::DoCreate(data, size, rsUiCtx);
    OHOS::Rosen::DoSetFreeze(data, size);
    OHOS::Rosen::DoSetLinkedNodeId(data, size);
    OHOS::Rosen::DoGetLinkedNodeId(data, size);
    OHOS::Rosen::DoReadFromParcelAndWriteToParcel(data, size);
    OHOS::Rosen::RSRenderNodeGC::Instance().ReleaseNodeMemory();
    return 0;
}

