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

#include "rswindowkeyframenodecommand_fuzzer.h"

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

#include "render_thread/rs_render_thread_visitor.h"
#include "feature/window_keyframe/rs_window_keyframe_render_node.h"
#include "feature/window_keyframe/rs_window_keyframe_node_command.h"
#include "pipeline/rs_render_node_gc.h"

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

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size, RSContext& rsContext)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;
    NodeId nodeId = GetData<NodeId>();
    NodeId linkNodeId = GetData<NodeId>();
    RSWindowKeyFrameNodeCommandHelper::Create(rsContext, nodeId, GetData<bool>());
    RSWindowKeyFrameNodeCommandHelper::LinkNode(rsContext, nodeId, linkNodeId);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSContext rsContext;
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size, rsContext);
    OHOS::Rosen::RSRenderNodeGC::Instance().ReleaseNodeMemory();
    return 0;
}
