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

#include "rsnodemap_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "pipeline/rs_node_map.h"
#include "ui/rs_canvas_node.h"

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

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    RSNodeMap::MutableInstance().GetNode(GetData<NodeId>());
    RSNodeMap::MutableInstance().UnregisterNode(GetData<NodeId>());
    RSNodeMap::MutableInstance().RegisterNodeInstanceId(GetData<NodeId>(), GetData<int32_t>());
    RSNodeMap::MutableInstance().RegisterAnimationInstanceId(
        GetData<AnimationId>(), GetData<NodeId>(), GetData<int32_t>());
    RSNodeMap::MutableInstance().UnregisterAnimation(GetData<AnimationId>());
    RSNodeMap::MutableInstance().GetNodeInstanceId(GetData<NodeId>());
    RSNodeMap::MutableInstance().GetInstanceIdForReleasedNode(GetData<NodeId>());
    RSNodeMap::MutableInstance().GetAnimationFallbackNode();
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
