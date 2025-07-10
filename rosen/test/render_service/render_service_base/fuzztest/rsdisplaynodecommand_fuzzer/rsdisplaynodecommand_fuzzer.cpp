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

#include "rsdisplaynodecommand_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "command/rs_display_node_command.h"
#include "pipeline/rs_render_node_gc.h"

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

bool DoDisplayNode(const uint8_t* data, size_t size, RSContext& context)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    NodeId id = GetData<NodeId>();
    uint64_t screenId = GetData<uint64_t>();
    bool isMirrored = GetData<bool>();
    NodeId mirrorNodeId = GetData<NodeId>();
    RSDisplayNodeConfig config = { screenId, isMirrored, mirrorNodeId };
    bool isSecurityDisplay = GetData<bool>();
    bool isBootAnimation = GetData<bool>();
    NodeId id2 = GetData<NodeId>();
    ScreenRotation screenRotation = GetData<ScreenRotation>();
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetScreenId(context, id, screenId);
    DisplayNodeCommandHelper::SetSecurityDisplay(context, id, isSecurityDisplay);
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);
    DisplayNodeCommandHelper::SetBootAnimation(context, id, isBootAnimation);
    DisplayNodeCommandHelper::SetBootAnimation(context, id2, isBootAnimation);
    DisplayNodeCommandHelper::SetScreenRotation(context, id, screenRotation);
    return true;
}
bool DoSetDisplayMode(const uint8_t* data, size_t size, RSContext& context)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    NodeId id = GetData<NodeId>();
    RSDisplayNodeConfig config { 0, false, 0 };
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);

    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);

    config.isMirrored = true;
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);

    NodeId mirrorNodeId = GetData<NodeId>();
    config.mirrorNodeId = mirrorNodeId;
    DisplayNodeCommandHelper::Create(context, mirrorNodeId, config);
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);

    return true;
}
bool DoSetBootAnimation(const uint8_t* data, size_t size, RSContext& context)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    NodeId id = GetData<NodeId>();
    NodeId rID = GetData<NodeId>();
    RSDisplayNodeConfig config { 0, true, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    DisplayNodeCommandHelper::SetBootAnimation(context, id, true);
    DisplayNodeCommandHelper::SetBootAnimation(context, rID, true);

    return true;
}
bool DoSetScbNodePid(const uint8_t* data, size_t size, RSContext& context)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    NodeId id = GetData<NodeId>();
    std::vector<int32_t> oldScbPids = {};
    int32_t currentScbPid = -1;
    DisplayNodeCommandHelper::SetScbNodePid(context, id, oldScbPids, currentScbPid);
    RSDisplayNodeConfig config { 0, true, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
    int32_t pid = GetData<int32_t>();
    oldScbPids.push_back(pid);
    pid = GetData<int32_t>();
    oldScbPids.push_back(pid);
    DisplayNodeCommandHelper::SetScbNodePid(context, id, oldScbPids, currentScbPid);

    return true;
}

bool DoSetVirtualScreenMuteStatus(const uint8_t* data, size_t size, RSContext& context)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    NodeId id = GetData<NodeId>();
    bool muteStatus = GetData<bool>();
    DisplayNodeCommandHelper::SetVirtualScreenMuteStatus(context, id, muteStatus);

    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    std::shared_ptr<OHOS::Rosen::RSContext> context = std::make_shared<OHOS::Rosen::RSContext>();

    /* Run your code on data */
    OHOS::Rosen::DoDisplayNode(data, size, *context);
    OHOS::Rosen::DoSetDisplayMode(data, size, *context);
    OHOS::Rosen::DoSetBootAnimation(data, size, *context);
    OHOS::Rosen::DoSetScbNodePid(data, size, *context);
    OHOS::Rosen::DoSetVirtualScreenMuteStatus(data, size, *context);

    context = nullptr;
    OHOS::Rosen::RSRenderNodeGC::Instance().ReleaseNodeMemory();

    return 0;
}
